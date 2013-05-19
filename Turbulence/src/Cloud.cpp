//
//  Cloud.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/09.
//
//

#include "Cloud.h"

#include "Resource.h"

#include "ofxOscMessage.h"
#include "constants.h"

Cloud::Cloud(const boost::property_tree::ptree& ptree) {
	position.x = ptree.get<float>("x");
	position.y = ptree.get<float>("y");
	scale = ptree.get<float>("scale");
	velocity = ptree.get<float>("velocity");
	
	image = Resource::getInstance()->getImage("kumo.png");
}

void Cloud::update() {
	position.x -= velocity * 1.0f / 6.0f;
	if (position.x < 0) position.x += VIRTUAL_WIDTH;
}

void Cloud::draw() {
	ofPushMatrix();
	ofTranslate(position);
	
	ofScale(scale, scale);
	image->draw(0, 0);
	
	ofPopMatrix();
}

void Cloud::generageArgs(ofxOscMessage& message) {
	// 種類
	message.addIntArg(-1);
	
	// 位置
	message.addFloatArg(position.x);
	message.addFloatArg(position.y);
	
	// 回転
	message.addFloatArg(0);
	
	// スケール
	message.addFloatArg(scale);
	
	// アルファ
	message.addFloatArg(1);
}
