//
//  MeshControl.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#ifndef __Turbulence__MeshControl__
#define __Turbulence__MeshControl__

#include "Mesh.h"

#include <boost/property_tree/ptree.hpp>

class OscServer;

class MeshControl {
public:
	static const int NOT_SELECTED = -1;
	
private:
	int selectedMesh = NOT_SELECTED;
	int activeMesh = NOT_SELECTED;
	int selectedPoint = NOT_SELECTED;
	int activePoint = NOT_SELECTED;
	
	bool isDragging = false;
	
	float globalScale;
	ofVec2f dragStartPoint;
	ofVec2f dragStartOrigin;
	
	boost::optional<OscServer&> server;
	
public:
	std::vector<Mesh> meshes;
	
	MeshControl();
	
	void setServer(OscServer& server);
	
	void load(const boost::property_tree::ptree& ptree);
	void save(boost::property_tree::ptree& ptree);
	
	void mouseMoved(const ofVec2f& vpos);
	void mousePressed(const ofVec2f& vpos);
	void mouseReleased(const ofVec2f& vpos, OscServer& oscServer);
	
	int getSelectedMesh() {
		return selectedMesh;
	}
	
	void draw();
};

#endif /* defined(__Turbulence__MeshControl__) */
