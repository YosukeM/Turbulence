//
//  Cloud.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/09.
//
//

#ifndef __Turbulence__Cloud__
#define __Turbulence__Cloud__

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>

#include <ofMain.h>

class ofxOscMessage;

class Cloud {
	ofVec2f position;
	float velocity;
	float scale;
	
	boost::optional<ofImage&> image;
	
public:
	Cloud(const boost::property_tree::ptree& ptree);
	
	void update();
	void draw();
	
	void generageArgs(ofxOscMessage& message);
};

#endif /* defined(__Turbulence__Cloud__) */
