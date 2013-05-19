//
//  Mesh.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#ifndef __Turbulence__Mesh__
#define __Turbulence__Mesh__

#include <ofMain.h>
#include <boost/property_tree/ptree.hpp>

class ofxOscMessage;

class Mesh {
public:
	static const int COLS_NUM = 4;
	static const int ROWS_NUM = 4;
	static const int POINTS_NUM = 16;
	int id;
	bool isLocked = false;
	ofVec2f points[POINTS_NUM];
	
	void load(const boost::property_tree::ptree& ptree);
	void save(boost::property_tree::ptree& ptree);
	void generateMessage(ofxOscMessage& message);
	
	bool isInside(const ofVec2f& vpos) const;
	
	void normalize();
};

#endif /* defined(__Turbulence__Mesh__) */
