//
//  BorderControl.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/10.
//
//

#ifndef __Turbulence__BorderControl__
#define __Turbulence__BorderControl__

#include <boost/optional.hpp>
#include "Border.h"

class MeshControl;
class OscServer;

class BorderControl {
	boost::optional<MeshControl&> meshControl;
	boost::optional<OscServer&> server;
	int selectedClient = -1;
	int moving = -1;
	
public:
	static const int BORDERS_NUM = 6;
	Border borders[BORDERS_NUM];
	
private:
	static ofVec2f windowToClient(int x, int y);
	static ofVec2f clientToWindow(const ofVec2f& v);
	
public:
	BorderControl();
	void setup(MeshControl& meshControl, OscServer& server);
	
	void mousePressed(int x, int y);
	void mouseDragged(int x, int y);
	void mouseReleased(int x, int y);
	
	void draw();
	
	void load(const boost::property_tree::ptree& ptree);
	void save(boost::property_tree::ptree& ptree);
};

#endif /* defined(__Turbulence__BorderControl__) */
