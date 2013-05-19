//
//  FlyingObjectPhysics.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//

#ifndef __Turbulence__FlyingObjectPhysics__
#define __Turbulence__FlyingObjectPhysics__

#include "FlyingObject.h"
#include <boost/optional.hpp>
#include <ofMain.h>
#include <boost/property_tree/ptree.hpp>

class FlyingObjectPhysics : public FlyingObject {
protected:
	ofxBox2dBaseShape* shape = NULL;
	float lifetime = 0.0f;
	unsigned long long setupTime;
	boost::optional<ofImage&> image;
	boost::optional<ofSoundPlayer&> hitSound;
	ofVec2f centerVector;
	ofVec2f prevVelocity;
	
	float getAlpha();
	
public:
	FlyingObjectPhysics(E_KIND kind);
	virtual ~FlyingObjectPhysics() {};
	
	virtual void setup(ofxBox2d& box2d, int x, int y, boost::property_tree::ptree& physicsSettings, float lifetime);
	virtual void destroy(ofxBox2d& box2d);
	virtual bool update();
	virtual void draw();
	
	virtual void onCollide(const FlyingObjectPhysics* fbp);
	
	virtual void generateArgs(ofxOscMessage& message);
};

#endif /* defined(__Turbulence__FlyingObjectPhysics__) */
