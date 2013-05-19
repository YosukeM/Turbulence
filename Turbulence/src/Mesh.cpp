//
//  Mesh.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Mesh.h"
#include "ofxOscMessage.h"
#include "constants.h"

using namespace boost;

void Mesh::generateMessage(ofxOscMessage& message) {
	message.setAddress("/mesh/positions");
	for (auto p : points) {
		message.addFloatArg(p.x);
		message.addFloatArg(p.y);
	}
}

namespace {
	float cross(const ofVec2f& a, const ofVec2f& b) {
		return a.x * b.y - a.y * b.x;
	}
	
	bool isInsideTriangle(const ofVec2f& p, const ofVec2f& a, const ofVec2f& b, const ofVec2f& c) {
		if (cross(p - a, b - a) < 0.0f) return false;
		if (cross(p - b, c - b) < 0.0f) return false;
		if (cross(p - c, a - c) < 0.0f) return false;
		return true;
	}
}

bool Mesh::isInside(const ofVec2f& vpos) const {
	auto pos = [this] (int x, int y) -> ofVec2f {
		return points[y * COLS_NUM + x];
	};
	
	//ofVec2f vpos = ofVec2f(10, 10);
	
	for (int x = 0; x < COLS_NUM - 1; x++) {
		for (int y = 0; y < ROWS_NUM - 1; y++) {
			if (isInsideTriangle(vpos, pos(x, y), pos(x, y+1), pos(x+1, y))) {
				return true;
			}
			if (isInsideTriangle(vpos, pos(x+1, y+1), pos(x+1, y), pos(x, y+1))) {
				return true;
			}
		}
	}
	return false;
}

void Mesh::load(const property_tree::ptree& ptree) {
	auto key = string("settings.meshes.projector") + (char)(id + '0');
	auto value = ptree.get<string>(key, "");
	
	vector<string> v;
	algorithm::split( v, value, algorithm::is_space() );
	
	if (v.size() >= POINTS_NUM * 2) {
		int i = 0;
		for (auto& p : points) {
			p.x = lexical_cast<float>(v[i + 0]);
			p.y = lexical_cast<float>(v[i + 1]);
			i += 2;
		}
	}
}

void Mesh::save(property_tree::ptree& ptree) {
	auto key = string("settings.meshes.projector") + (char)(id + '0');
	
	string output = "";
	for (auto& p : points) {
		output += lexical_cast<string>(p.x) + " ";
		output += lexical_cast<string>(p.y) + " ";
	}
	output.resize(output.size() - 1);
	
	ptree.put(key, output);
}


void Mesh::normalize() {
	for (auto&p : points) {
		while (p.x < 0) p.x += VIRTUAL_WIDTH;
		while (p.x > VIRTUAL_WIDTH) p.x -= VIRTUAL_WIDTH;
	}
}