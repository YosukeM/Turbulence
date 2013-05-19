//
//  FlyingObjectPhysics.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//

#include "FlyingObjectPhysics.h"

#include "constants.h"

#include "ofxBox2d.h"
#include "ofxBox2dBaseShape.h"

#include <ofMain.h>
#include "Resource.h"

#include "ofxOscMessage.h"

using namespace std;

FlyingObjectPhysics::FlyingObjectPhysics(E_KIND kind)
: FlyingObject(kind)
{
	
}

void FlyingObjectPhysics::setup(ofxBox2d& box2d, int x, int y, boost::property_tree::ptree& ptree, float lifetime) {
	ofxBox2dCircle* circle = new ofxBox2dCircle();
	circle->setPhysics(ptree.get<float>("density", 1.0f), ptree.get<float>("bounce", 1.0f), ptree.get<float>("friction", 1.0f));
	auto radius = ptree.get<float>("radius", 60.0f);
	circle->setup(box2d.getWorld(), x, y - radius, radius);
	
	circle->setDamping(ptree.get<float>("damping", 60.0f));
	
	auto imageName = ptree.get<string>("image", "");
	auto soundName = ptree.get<string>("sound", "");
	auto hitSoundName = ptree.get<string>("hitsound", "");
	
	circle->setVelocity(ofRandom(-2.0f, 2.0f), 10.0f + ofRandom(-2.0f, 2.0f));
	
	
	if (imageName != "") {
		image = Resource::getInstance()->getImage(imageName);
		centerVector.x = ptree.get<float>("centerx", image->getWidth() * 0.5f);
		centerVector.y = ptree.get<float>("centery", image->getHeight() * 0.5f);
	}
	
	if (soundName != "") {
		Resource::getInstance()->getSound(soundName).play();
	}
	
	shape = circle;
	setupTime = ofGetElapsedTimeMillis();
	this->lifetime = lifetime;
	
	circle->setData(this);
	circle->body->SetBullet(true);
	
	if (hitSoundName != "") {
		hitSound = Resource::getInstance()->getSound(hitSoundName);
	}
}

void FlyingObjectPhysics::onCollide(const FlyingObjectPhysics* fbp) {
	if (hitSound && (prevVelocity - shape->getVelocity()).length() > 10.0f ) {
		if (
			(hitSound->getIsPlaying() && hitSound->getPosition() > 0.1f)
			|| (!hitSound->getIsPlaying())
		) {
			if (fbp != NULL || shape->getPosition().y > VIRTUAL_HEIGHT * 0.5f) {
				hitSound->play();
			}
		}
	}
}


void FlyingObjectPhysics::destroy(ofxBox2d& box2d) {
	shape->destroy();
}

bool FlyingObjectPhysics::update() {
	shape->update();
	
	prevVelocity = shape->getVelocity();
	
	if (shape->getPosition().x < 0) {
		auto velocity = shape->getVelocity();
		shape->setPosition(shape->getPosition() + ofVec2f(VIRTUAL_WIDTH, 0.0f));
		shape->setVelocity(velocity);
	}
	
	if (ofGetElapsedTimeMillis() - setupTime > lifetime * 1000.0f) {
		return true;
	} else {
		return false;
	}
}

float FlyingObjectPhysics::getAlpha() {
	const float FADE_TIME = 2.0f;
	float alpha;
	int elapsed = (ofGetElapsedTimeMillis() - setupTime) / 1000.0f;
	if (elapsed < lifetime - FADE_TIME) {
		alpha = 1.0f;
	} else {
		alpha = (lifetime - elapsed) / FADE_TIME;
	}
	return alpha;
}

void FlyingObjectPhysics::draw() {
	if (image) {
		ofPushMatrix();
		ofPushStyle();
		
		ofSetColor(255, 255, 255, getAlpha() * 255);
		
		ofTranslate(shape->getPosition());
		ofRotate(shape->getRotation());
		image->draw(- centerVector.x, - centerVector.y);
		
		ofPopStyle();
		ofPopMatrix();
	} else {
		shape->draw();
	}
}

void FlyingObjectPhysics::generateArgs(ofxOscMessage& message) {
	// 種類
	message.addIntArg(kind);
	// 位置
	message.addFloatArg(shape->getPosition().x);
	message.addFloatArg(shape->getPosition().y);
	// 回転
	message.addFloatArg(shape->getRotation());
	// スケール
	message.addFloatArg(1);
	// アルファ
	message.addFloatArg(getAlpha());
}