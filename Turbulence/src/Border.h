//
//  Border.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/10.
//
//

#ifndef __Turbulence__Border__
#define __Turbulence__Border__

#include <boost/property_tree/ptree.hpp>
#include <ofMain.h>
class ofxOscMessage;

class Border {
	static const float EDGE;
public:
	int id;
	ofVec2f points[4];
	Border();
	
	void setup(int id);
	
	void generateMessage(ofxOscMessage& message);
	
	void load(const boost::property_tree::ptree& ptree);
	
	void save(boost::property_tree::ptree& ptree);
};

#endif /* defined(__Turbulence__Border__) */
