#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/property_tree/ptree.hpp>

#include "ofMain.h"
#include "ofxBox2d.h"
#include "FlyingObject.h"
#include "OscServer.h"
#include "MeshControl.h"
#include "Pusher.h"
#include "BorderControl.h"

class Cloud;

class testApp : public ofBaseApp{
	boost::mutex mutex;
	ofxBox2d box2d;

	std::vector<Cloud*> clouds;
	std::list<FlyingObject*> fobjects;
	OscServer server;
	Pusher pusher;
	
	float globalScale;
	boost::property_tree::ptree settings;
	
	MeshControl meshControl;
	BorderControl borderControl;
	
	FlyingObject* createFlyingObject(FlyingObject::E_KIND kind, float x);
	
	void sendFlyingObjectStatus();
	
	float compassToX(float);
	
	ofVec2f virtualPos(int x, int y) const;

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
};
