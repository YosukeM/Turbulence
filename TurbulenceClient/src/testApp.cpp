#include "testApp.h"

#include <boost/optional.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/range.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/range/algorithm.hpp>

#include "Resource.h"
#include "ofxOscMessage.h"
#include "constants.h"

using namespace boost;

//--------------------------------------------------------------
void testApp::setup(){
	property_tree::read_xml(ofToDataPath("settings.xml"), ptree);
	ofSetFrameRate(60);
	
	Resource::getInstance()->loadFolder("image");
	Resource::getInstance()->loadFolder("bgimage");
	
	oscClient.connect(ptree.get<string>("settings.network.server", "localhost"), ptree.get<int>("settings.network.clientid", 0));
	setupOscListeners();
	
	faderEdges[0] = ofVec2f(30.0f, 30.0f);
	faderEdges[1] = ofVec2f(ofGetWidth() - 30.0f, 30.0f);
	faderEdges[2] = ofVec2f(ofGetWidth() - 30.0f, ofGetHeight() - 30.0f);
	faderEdges[3] = ofVec2f(30.0f, ofGetHeight() - 30.0f);
	onChangeFader();
	
	ofSetVerticalSync(true);
	
	ofSetFullscreen(true);
	ofHideCursor();
}


//--------------------------------------------------------------
void testApp::drawBasically() {
	ofPushStyle();
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA);
	
	// draw items
	auto& itemProps = ptree.get_child("settings.flyingobjects");
	
	int numArgs = renderedMessage.getNumArgs();
	for (int i = 0; i < numArgs; i += 7) {
		int kind = renderedMessage.getArgAsInt32(i);
		float x = renderedMessage.getArgAsFloat(i+1);
		float y = renderedMessage.getArgAsFloat(i+2);
		float rotation = renderedMessage.getArgAsFloat(i+3);
		float scale = renderedMessage.getArgAsFloat(i+4);
		float alpha = renderedMessage.getArgAsFloat(i+5);
		
		boost::optional<ofImage&> image;
		ofVec2f centerVector;
		
		if (kind == -1) {
			image = Resource::getInstance()->getImage("kumo.png");
			
		} else {
			auto itr = itemProps.begin();
			std::advance(itr, kind);
			auto& prop = itr->second;
			
			auto imageName = prop.get<string>("image", "");
			if (imageName != "") {
				image = Resource::getInstance()->getImage(imageName);
				centerVector.x = prop.get<float>("centerx", image->getWidth() * 0.5f);
				centerVector.y = prop.get<float>("centery", image->getHeight() * 0.5f);
			}
		}
		
		ofPushMatrix();
		ofPushStyle();
		ofSetColor(255, 255, 255, alpha * 255);
		ofTranslate(x, y);
		ofRotate(rotation);
		ofScale(scale, scale);
		image->draw(- centerVector.x, - centerVector.y);
		ofPopStyle();
		ofPopMatrix();
	}
	ofPopStyle();
}


//--------------------------------------------------------------
void testApp::renderOnFbo() {
	fbo.begin();
	
	ofBackground(ofColor::black);
	
	ofPushMatrix();
	
	ofTranslate(- fboRegion.x, - fboRegion.y);
	
	for (int i = -1; i <= 1; i++) {
		ofPushMatrix();
		ofTranslate(VIRTUAL_WIDTH * i, 0.0f);
		// draw background
		ofSetColor(255, 255, 255);
		for (int i = 1, x = 0; i <= 6; i++) {
			ofImage& image = Resource::getInstance()->getImage(std::string("enkei_0") + (char)('0' + i) + ".png");
			image.draw(x, 0);
			x += image.getWidth();
		}
		ofPopMatrix();
	}
	
	for (int i = -1; i <= 1; i++) {
		ofPushMatrix();
		ofTranslate(VIRTUAL_WIDTH * i, 0.0f);
		drawBasically();
		ofPopMatrix();
	}
	
	// letterbox
	ofPushStyle();
		ofSetColor(ofColor::black);
		ofRect(fboRegion.x, - VIRTUAL_HEIGHT, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
		ofRect(fboRegion.x, VIRTUAL_HEIGHT, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
	ofPopStyle();
	
	ofPopMatrix();
	
	fbo.end();
}

void testApp::onChangeFader() {
	faderVerts.clear();
	faderColors.clear();
	
	auto addVerts = [&] (const ofVec2f& t1, const ofVec2f& t2, const ofVec2f& b3, const ofVec2f& b4) {
		faderVerts.push_back(t1.x); faderVerts.push_back(t1.y);
		faderVerts.push_back(t2.x); faderVerts.push_back(t2.y);
		faderVerts.push_back(b3.x); faderVerts.push_back(b3.y);
		faderVerts.push_back(b4.x); faderVerts.push_back(b4.y);
		
		faderColors.push_back(1.0f); faderColors.push_back(1.0f); faderColors.push_back(1.0f);
		faderColors.push_back(1.0f); faderColors.push_back(1.0f); faderColors.push_back(1.0f);
		faderColors.push_back(0.0f); faderColors.push_back(0.0f); faderColors.push_back(0.0f);
		faderColors.push_back(0.0f); faderColors.push_back(0.0f); faderColors.push_back(0.0f);
	};
	
	addVerts(faderEdges[0], faderEdges[1], ofVec2f(ofGetWidth(), 0), ofVec2f(0, 0));
	addVerts(faderEdges[1], faderEdges[2], ofVec2f(ofGetWidth(), ofGetHeight()), ofVec2f(ofGetWidth(), 0));
	addVerts(faderEdges[2], faderEdges[3], ofVec2f(0, ofGetHeight()), ofVec2f(ofGetWidth(), ofGetHeight()));
	addVerts(faderEdges[3], faderEdges[0], ofVec2f(0, 0), ofVec2f(0, ofGetHeight()));
}

//--------------------------------------------------------------
void testApp::resetMesh(const ofxOscMessage& message) {
	if (message.getNumArgs() != POINTS_NUM * 2) {
		cout << "osc message is not valid" << endl;
		return;
	}
	
	// read message
	vector<ofVec2f> points;
	points.resize(message.getNumArgs() / 2);
	for (int i = 0; i < message.getNumArgs(); i += 2) {
		points[i/2].x = message.getArgAsFloat(i);
		points[i/2].y = message.getArgAsFloat(i+1);
	}
	
	// normalize point x
	for (auto& p : points) {
		while (p.x - points[0].x >= VIRTUAL_WIDTH * 0.5f) {
			p.x -= VIRTUAL_WIDTH;
		}
		while (p.x - points[0].x < -VIRTUAL_WIDTH * 0.5f) {
			p.x += VIRTUAL_WIDTH;
		}
	}
	
	vector<float> point_xs, point_ys;
	point_xs.resize(message.getNumArgs() / 2);
	point_ys.resize(message.getNumArgs() / 2);
	transform(points, point_xs.begin(), [] (const ofVec2f& v) -> float { return v.x; });
	transform(points, point_ys.begin(), [] (const ofVec2f& v) -> float { return v.y; });
	
	
	// determine fboRegion
	fboRegion.x = *min_element(point_xs);
	fboRegion.y = *min_element(point_ys);
	float width = *max_element(point_xs) - fboRegion.x;
	float height = *max_element(point_ys) - fboRegion.y;
	fboRegion.width = width;
	fboRegion.height = height;
	fbo.allocate(width, height);
	
	// create vert pairs
	pair<ofVec2f, ofVec2f> vertPairs[POINTS_NUM];
	for (int x = 0; x < COLS_NUM; x++) {
		for (int y = 0; y < ROWS_NUM; y++) {
			auto& pair = vertPairs[y * COLS_NUM + x];
			const auto& point = points[y * COLS_NUM + x];
			pair.first = ofVec2f(ofGetWidth() * x / (COLS_NUM - 1), ofGetHeight() * y / (ROWS_NUM - 1));
			pair.second = ofVec2f(
				(point.x - fboRegion.x),
				(point.y - fboRegion.y)
			);
		}		
	}
	
	verts.clear();
	texCoords.clear();
	for (int x = 0; x < COLS_NUM - 1; x++) {
		for (int y = 0; y < ROWS_NUM - 1; y++) {
			const auto& a = vertPairs[y * COLS_NUM + x];
			const auto& b = vertPairs[y * COLS_NUM + x+1];
			const auto& c = vertPairs[(y+1) * COLS_NUM + x];
			const auto& d = vertPairs[(y+1) * COLS_NUM + x+1];
			
			verts.push_back(a.first.x); verts.push_back(a.first.y);
			verts.push_back(b.first.x); verts.push_back(b.first.y);
			verts.push_back(c.first.x); verts.push_back(c.first.y);
			texCoords.push_back(a.second.x); texCoords.push_back(a.second.y);
			texCoords.push_back(b.second.x); texCoords.push_back(b.second.y);
			texCoords.push_back(c.second.x); texCoords.push_back(c.second.y);
			
			verts.push_back(b.first.x); verts.push_back(b.first.y);
			verts.push_back(c.first.x); verts.push_back(c.first.y);
			verts.push_back(d.first.x); verts.push_back(d.first.y);
			texCoords.push_back(b.second.x); texCoords.push_back(b.second.y);
			texCoords.push_back(c.second.x); texCoords.push_back(c.second.y);
			texCoords.push_back(d.second.x); texCoords.push_back(d.second.y);
		}
	}
}

//--------------------------------------------------------------
void testApp::update(){
	oscClient.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	renderOnFbo();
	
	//fbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	if (fbo.isAllocated()) {
		ofPushStyle();
		fbo.getTextureReference().bind();
	
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0] );
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &verts[0] );
		glDrawArrays( GL_TRIANGLES, 0, verts.size() / 2 );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	
		fbo.getTextureReference().unbind();
		
		ofPopStyle();
	}
	
	{
		ofPushStyle();
		
		ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_MULTIPLY);
		
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer(3, GL_FLOAT, 0, &faderColors[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &faderVerts[0] );
		glDrawArrays( GL_QUADS, 0, faderVerts.size() / 2 );
		glDisableClientState( GL_COLOR_ARRAY );
		glDisableClientState( GL_VERTEX_ARRAY );
		
		ofDisableBlendMode();
		
		ofPopStyle();
	}
	
	if (activeEdge != NOT_DRAGGING) {
		ofPushStyle();
		ofSetColor(ofColor::white);
		ofCircle(faderEdges[activeEdge], 10.0f);
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void testApp::exit(){
	oscClient.disconnect();
}

//--------------------------------------------------------------
void testApp::setupOscListeners() {
	oscClient.setListener("/flyingobjects/update", [this] (const ofxOscMessage& message) {
		renderedMessage = message;
	});
	oscClient.setListener("/mesh/positions", [this] (const ofxOscMessage& message) {
		resetMesh(message);
	});
	oscClient.setListener("/border/positions", [this] (const ofxOscMessage& message) {
		for (int i = 0; i < 4; i++) {
			faderEdges[i].x = message.getArgAsFloat(i*2);
			faderEdges[i].y = message.getArgAsFloat(i*2+1);
		}
		onChangeFader();
	});
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 't') {
		ofToggleFullscreen();
		if (ofGetWindowMode() == OF_WINDOW) {
			ofShowCursor();
		} else {
			ofHideCursor();
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	activeEdge = NOT_DRAGGING;
	
	int i = 0;
	for (const auto& p : faderEdges) {
		if (p.distance(ofVec2f(x, y)) < 10.0f) {
			activeEdge = i;
			break;
		}
		i++;
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if (button == 0) {
		faderEdges[draggingEdge].x = x;
		faderEdges[draggingEdge].y = y;
		onChangeFader();
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (button == 0) {
		draggingEdge = activeEdge;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if (button == 0) {
		draggingEdge = NOT_DRAGGING;
		activeEdge = NOT_DRAGGING;		
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