//
//  BorderControl.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/10.
//
//

#include "BorderControl.h"
#include "constants.h"
#include "MeshControl.h"
#include "OscServer.h"

ofVec2f BorderControl::windowToClient(int x, int y) {
	return ofVec2f(x * CLIENT_WIDTH / WINDOW_WIDTH, y * CLIENT_HEIGHT / WINDOW_HEIGHT);
}

ofVec2f BorderControl::clientToWindow(const ofVec2f& v) {
	return ofVec2f(v.x * WINDOW_WIDTH / CLIENT_WIDTH, v.y * WINDOW_HEIGHT / CLIENT_HEIGHT);
}

BorderControl::BorderControl() {
	
}

void BorderControl::setup(MeshControl& meshControl, OscServer& server) {
	this->meshControl = meshControl;
	this->server = server;
	for (int i = 0; i < 6; i++) {
		borders[i].setup(i);
	}
}

void BorderControl::mousePressed(int x, int y) {
	if (!meshControl) return;
	
	selectedClient = meshControl->getSelectedMesh();
	if (selectedClient == MeshControl::NOT_SELECTED) return;
	
	auto mapped = windowToClient(x, y);
	
	if (mapped.x < CLIENT_WIDTH * 0.5f) {
		if (mapped.y < CLIENT_HEIGHT * 0.5f) {
			moving = 0;
		} else {
			moving = 3;
		}
	} else {
		if (mapped.y < CLIENT_HEIGHT * 0.5f) {
			moving = 1;
		} else {
			moving = 2;
		}
	}
}

void BorderControl::mouseDragged(int x, int y) {
	if (selectedClient != -1 && moving != -1) {
		borders[selectedClient].points[moving] = windowToClient(x, y);
		ofxOscMessage message;
		borders[selectedClient].generateMessage(message);
		server->sendMessageTo(message, selectedClient);
	}
}

void BorderControl::mouseReleased(int x, int y) {
	moving = -1;
}

void BorderControl::draw() {
	if (moving != -1) {
		for (const auto& p : borders[selectedClient].points) {
			ofLine(clientToWindow(p - ofVec2f(0, 30)), clientToWindow(p + ofVec2f(0, 30)));
			ofLine(clientToWindow(p - ofVec2f(10, 0)), clientToWindow(p + ofVec2f(10, 0)));
		}
	}
}

void BorderControl::load(const boost::property_tree::ptree& ptree) {
	for (int i = 0; i < 6; i++) {
		borders[i].load(ptree);
	}
}

void BorderControl::save(boost::property_tree::ptree& ptree){
	for (int i = 0; i < 6; i++) {
		borders[i].save(ptree);
	}
}