#include <boost/property_tree/xml_parser.hpp>
#include <boost/lexical_cast.hpp>

#include "testApp.h"
#include "FlyingObject.h"
#include "Resource.h"
#include "constants.h"
#include "FlyingObjectPhysics.h"
#include "json.h"

#include "Cloud.h"

using namespace boost;

//--------------------------------------------------------------
void testApp::setup(){	
	globalScale = WINDOW_WIDTH / VIRTUAL_WIDTH;
	const float GROUND_HEIGHT = 117.0f;
	const float SKY_HEIGHT = 300.0f;
	// ofSetWindowShape(BG_WIDTH * globalScale, BG_HEIGHT * globalScale);
	
	property_tree::read_xml(ofToDataPath("settings.xml"), settings);
	
	ofSetFrameRate(60);
	
	box2d.init();
	box2d.setGravity(- 5.0f,1.0f);
	box2d.createGround(0.0f, VIRTUAL_HEIGHT - GROUND_HEIGHT, VIRTUAL_WIDTH, VIRTUAL_HEIGHT - GROUND_HEIGHT);
	box2d.createGround(0.0f, - SKY_HEIGHT, VIRTUAL_WIDTH, - SKY_HEIGHT);
	box2d.setFPS(60);
	
	Resource::getInstance()->loadFolder("image");
	Resource::getInstance()->loadFolder("bgimage");
	Resource::getInstance()->loadFolder("sound");
	
	meshControl.load(settings);
	meshControl.setServer(server);
	borderControl.setup(meshControl, server);
	borderControl.load(settings);
	
	server.setup();
	server.setOnConnect([this] (int id) {
		ofxOscMessage message, message2;
		meshControl.meshes[id].generateMessage(message);
		borderControl.borders[id].generateMessage(message2);
		server.sendMessageTo(message, id);
		server.sendMessageTo(message2, id);
	});
	
	pusher.setup(settings.get<string>("settings.pusher.apiKey"));
	pusher.setListener(settings.get<string>("settings.pusher.event"), [this] (const Json::Value& data) {
		cout << data.toStyledString() << endl;
		
		auto compassValue = data.get("compass", Json::Value::null);
		auto kindValue = data.get("item", Json::Value::null);
		
		if (
			compassValue.isConvertibleTo(Json::ValueType::realValue)
			&& compassValue.isConvertibleTo(Json::ValueType::intValue)
		) {
			float compass = compassValue.asFloat();
			int kind = kindValue.asInt();
			
			if (0 <= kind && kind <= FlyingObject::KIND_MAX) {
				mutex::scoped_lock lock(mutex);
				fobjects.push_back(createFlyingObject((FlyingObject::E_KIND)kind, compassToX(compass)));
			}
		}
	});
	
	pusher.setListener("pusher:connection_established", [this] (const Json::Value& socket_id) {
		pusher.subscribe(settings.get<string>("settings.pusher.channel"));
	});
	pusher.connect();
	
	auto& cloudProps = settings.get_child("settings.clouds");
	for (auto& prop : cloudProps) {
		clouds.push_back(new Cloud(prop.second));
	}
}

float testApp::compassToX(float compass) {
	auto x = VIRTUAL_WIDTH * 0.5f - compass / 360.0f * VIRTUAL_WIDTH;
	while (x < 0.0f) x += VIRTUAL_WIDTH;
	while (x > VIRTUAL_WIDTH) x -= VIRTUAL_WIDTH;
	return x;
}

void testApp::exit() {
	pusher.disconnect();
	for (auto fobj : fobjects) {
		fobj->destroy(box2d);
		delete fobj;
	}
	for (auto cloud : clouds) {
		delete cloud;
	}
}

//--------------------------------------------------------------
void testApp::update(){
	mutex::scoped_lock lock(mutex);
	box2d.update();
	
	if (pusher.shouldReconnect()) {
		pusher.disconnect();
		pusher.connect();
	}
	
	auto contact = box2d.getWorld()->GetContactList();
	
	while (contact) {
		auto a = static_cast<FlyingObjectPhysics*>(contact->GetFixtureA()->GetBody()->GetUserData());
		auto b = static_cast<FlyingObjectPhysics*>(contact->GetFixtureB()->GetBody()->GetUserData());
		if (a) a->onCollide(b);
		if (b) b->onCollide(a);
		contact = contact->GetNext();
	}
	
	for (auto cloud : clouds) {
		cloud->update();
	}
	
	{
		for (auto itr = fobjects.begin(); itr != fobjects.end(); ) {
			if ((*itr)->update()) {
				(*itr)->destroy(box2d);
				delete *itr;
				itr = fobjects.erase(itr);
			} else {
				itr++;
			}
		}
	}
	
	server.update();
	
	sendFlyingObjectStatus();
}

void testApp::sendFlyingObjectStatus() {
	const uint64_t SPLITTER = 0x0;
	ofxOscMessage message;
	message.setAddress("/flyingobjects/update");
	for (auto cloud : clouds) {
		cloud->generageArgs(message);
		message.addInt64Arg(SPLITTER);
	}
	for (auto& fobj : fobjects) {
		fobj->generateArgs(message);
		message.addInt64Arg(SPLITTER);
	}
	server.sendMessageToAll(message);
}

FlyingObject* testApp::createFlyingObject(FlyingObject::E_KIND kind, float x) {
	switch (kind) {
		default: {
			FlyingObjectPhysics* fo = new FlyingObjectPhysics(kind);
			
			// find matching property from the settings
			auto& itemProps = settings.get_child("settings.flyingobjects");
			auto itr = itemProps.begin();
			std::advance(itr, kind);
			auto& prop = itr->second;
			
			fo->setup(box2d, x, 0, prop, 20.0f);
			
			return fo;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0, 0, 0);
	
	ofPushStyle();
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);
	
	ofPushMatrix();
	ofTranslate(0, WINDOW_HEIGHT * 0.25f);
	ofScale(globalScale, globalScale);
	
	ofSetColor(255, 255, 255);
	for (int i = 1, x = 0; i <= 6; i++) {
		ofImage& image = Resource::getInstance()->getImage(std::string("enkei_0") + (char)('0' + i) + ".png");
		image.draw(x, 0);
		x += image.getWidth();
	}
	
	for (int i = -1; i <= 1; i++) {
		ofPushMatrix();
		ofTranslate(VIRTUAL_WIDTH * i, 0.0f);
		{
			for (auto cloud : clouds) {
				cloud->draw();
			}
			for (auto fobject : fobjects) {
				fobject->draw();
			}
		}
		ofPopMatrix();
	}
	
	//box2d.draw();
	
	meshControl.draw();
	
	ofPopMatrix();
	ofPopStyle();
	
	borderControl.draw();
}


//--------------------------------------------------------------
ofVec2f testApp::virtualPos(int x, int y) const {
	return ofVec2f(x / globalScale, (y - WINDOW_HEIGHT * 0.25f) / globalScale);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 's') {
		meshControl.save(settings);
		borderControl.save(settings);
		property_tree::write_xml(ofToDataPath("settings.xml"), settings);
	}
	if (key == 'r') {
		for (int id = 0; id < 6; id++) {
			ofxOscMessage message, message2;
			meshControl.meshes[id].generateMessage(message);
			borderControl.borders[id].generateMessage(message2);
			server.sendMessageTo(message, id);
			server.sendMessageTo(message2, id);
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	meshControl.mouseMoved(virtualPos(x, y));
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if (button == 0) {
		meshControl.mouseMoved(virtualPos(x, y));
	} else if (button == 1) {
		borderControl.mouseDragged(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (button == 2) {
		int rand = ceil(ofRandom(FlyingObject::KIND_MAX));
		
		FlyingObject* fobj = createFlyingObject((FlyingObject::E_KIND)rand, x / globalScale);
		fobjects.push_back(fobj);
	} else if (button == 0) {
		meshControl.mousePressed(virtualPos(x, y));
	} else if (button == 1) {
		borderControl.mousePressed(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if (button == 0) {
		meshControl.mouseReleased(virtualPos(x, y), server);
	} else if (button == 1) {
		borderControl.mouseReleased(x, y);
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}