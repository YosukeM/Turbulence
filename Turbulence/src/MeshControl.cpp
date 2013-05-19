//
//  MeshControl.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#include "MeshControl.h"
#include "constants.h"
#include "ofxOscMessage.h"
#include "OscServer.h"

MeshControl::MeshControl() {
	meshes.resize(6);
	int i = 0;
	for (auto& mesh : meshes) {
		mesh.id = i;
		i++;
	}
	
	globalScale = WINDOW_WIDTH / VIRTUAL_WIDTH;
}

void MeshControl::setServer(OscServer& server) {
	this->server = server;
}

void MeshControl::load(const boost::property_tree::ptree& ptree) {
	for (auto& mesh : meshes) {
		mesh.load(ptree);
	}
}

void MeshControl::save(boost::property_tree::ptree& ptree) {
	for (auto& mesh : meshes) {
		mesh.save(ptree);
	}
}

void MeshControl::mouseMoved(const ofVec2f& vpos) {
	if (isDragging) {
		if (selectedPoint == NOT_SELECTED) {
			// mesh is dragged
			auto& mesh = meshes[selectedMesh];
			auto deltaPos = vpos - dragStartPoint;
			
			auto currentMove = mesh.points[0] - dragStartOrigin;
			
			for (auto& p : mesh.points) {
				p = p - currentMove + deltaPos;
			}
			
			if (server) {
				ofxOscMessage message;
				mesh.generateMessage(message);
				server->sendMessageTo(message, mesh.id);
			}
			
		} else {
			// point is dragged
			auto& mesh = meshes[selectedMesh];
			auto& p = mesh.points[selectedPoint];
			auto deltaPos = vpos - dragStartPoint;
			p = dragStartOrigin + deltaPos;
			
			if (server) {
				ofxOscMessage message;
				mesh.generateMessage(message);
				server->sendMessageTo(message, mesh.id);
			}
		}
	} else {
		if (selectedMesh == NOT_SELECTED) {
			// which mesh is inside?
			for (auto& mesh : meshes) {
				if (mesh.isInside(vpos)) {
					activeMesh = mesh.id;
					break;
				}
			}
		} else {
			activePoint = NOT_SELECTED;
			auto& mesh = meshes[selectedMesh];
			int i = 0;
			for (auto& p : mesh.points) {
				if (p.distance(vpos) < 10.0f / globalScale) {
					activePoint = i;
					break;
				}
				i++;
			}
		}
	}
}

void MeshControl::mousePressed(const ofVec2f& vpos) {
	if (selectedMesh == NOT_SELECTED) {
		if (activeMesh != NOT_SELECTED) {
			selectedMesh = activeMesh;
			dragStartPoint = vpos;
			dragStartOrigin = meshes[selectedMesh].points[0];
			isDragging = true;
		}
	} else {
		if (activePoint != NOT_SELECTED) {
			selectedPoint = activePoint;
			dragStartPoint = vpos;
			dragStartOrigin = meshes[selectedMesh].points[selectedPoint];
			isDragging = true;
		} else if (meshes[selectedMesh].isInside(vpos)) {
			activePoint = selectedPoint = NOT_SELECTED;
			dragStartPoint = vpos;
			dragStartOrigin = meshes[selectedMesh].points[0];
			isDragging = true;
		} else {
			selectedMesh = NOT_SELECTED;
			selectedPoint = NOT_SELECTED;
		}
	}
}

void MeshControl::mouseReleased(const ofVec2f& vpos, OscServer& oscServer) {
	if (isDragging) {
		meshes[selectedMesh].normalize();
	}
	isDragging = false;
}

void MeshControl::draw() {
	auto drawLine = [] (const Mesh& mesh, int x1, int y1, int x2, int y2) {
		if (x2 >= Mesh::COLS_NUM || x2 < 0) return;
		if (y2 >= Mesh::ROWS_NUM || y2 < 0) return;
		
		auto p1 = mesh.points[x1 + y1 * Mesh::COLS_NUM];
		auto p2 = mesh.points[x2 + y2 * Mesh::COLS_NUM];
		
		if (p1.x < p2.x && p2.x - p1.x > VIRTUAL_WIDTH * 0.5f) {
			ofLine(p1 + ofVec2f(VIRTUAL_WIDTH, 0), p2);
			ofLine(p1, p2 - ofVec2f(VIRTUAL_WIDTH, 0));
		} else if (p1.x > p2.x && p1.x - p2.x > VIRTUAL_WIDTH * 0.5f) {
			ofLine(p2 + ofVec2f(VIRTUAL_WIDTH, 0), p1);
			ofLine(p2, p1 - ofVec2f(VIRTUAL_WIDTH, 0));
		} else {
			ofLine(p1, p2);
		}
	};
	
	for (auto& mesh : meshes) {
		ofPushStyle();
		
		ofColor color;
		if (mesh.id == selectedMesh) {
			color = ofColor::fromHsb(45 * mesh.id, 255, 255);
			color.a = 255;
		} else if (mesh.id == activeMesh) {
			color = ofColor::fromHsb(45 * mesh.id, 64, 255);
			color.a = 255;
		} else {
			color = ofColor::fromHsb(45 * mesh.id, 64, 255);
			color.a = 128;
		}
		ofSetColor(color);
		
		for (int j = 0; j < Mesh::POINTS_NUM; j++) {
			int x = j % Mesh::COLS_NUM;
			int y = j / Mesh::COLS_NUM;
			
			drawLine(mesh, x, y, x+1, y);
			drawLine(mesh, x, y, x-1, y);
			drawLine(mesh, x, y, x, y+1);
			drawLine(mesh, x, y, x, y-1);
		}
		
		int pointId = 0;
		for (const auto& p : mesh.points) {
			float size = 3.0f;
			if (mesh.id == selectedMesh && pointId == activePoint) {
				size = 5.0f;
			}
			ofCircle(p, size / globalScale);
			pointId++;
		}
		
		ofPopStyle();
	}
}