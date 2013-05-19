//
//  Border.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/10.
//
//

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "Border.h"
#include "ofxOscMessage.h"

#include "constants.h"
using namespace boost;

const float Border::EDGE = 30.0f;

Border::Border() {
	
}

void Border::setup(int id) {
	this->id = id;
	points[0] = ofVec2f(EDGE, EDGE);
	points[1] = ofVec2f(CLIENT_WIDTH - EDGE, EDGE);
	points[2] = ofVec2f(CLIENT_WIDTH - EDGE, CLIENT_HEIGHT - EDGE);
	points[3] = ofVec2f(EDGE, CLIENT_HEIGHT - EDGE);
}

void Border::generateMessage(ofxOscMessage& message) {
	message.setAddress("/border/positions");
	for (const auto& p : points) {
		message.addFloatArg(p.x);
		message.addFloatArg(p.y);
	}
}

void Border::load(const boost::property_tree::ptree& ptree) {
	auto value = ptree.get<string>("settings.borders.projector" + lexical_cast<string>(id), "");

	vector<string> v;
	algorithm::split( v, value, algorithm::is_space() );
	
	if (v.size() >= 4 * 2) {
		int i = 0;
		for (auto& p : points) {
			p.x = lexical_cast<float>(v[i + 0]);
			p.y = lexical_cast<float>(v[i + 1]);
			i += 2;
		}
	}
}

void Border::save(boost::property_tree::ptree& ptree) {
	auto key = string("settings.borders.projector") + (char)(id + '0');
	
	string output = "";
	for (auto& p : points) {
		output += lexical_cast<string>(p.x) + " ";
		output += lexical_cast<string>(p.y) + " ";
	}
	output.resize(output.size() - 1);
	
	ptree.put(key, output);
	
}