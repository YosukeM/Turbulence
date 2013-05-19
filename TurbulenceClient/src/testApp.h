#pragma once

#include "ofMain.h"
#include "OscClient.h"

#include <boost/property_tree/ptree.hpp>

class testApp : public ofBaseApp{
	OscClient oscClient;
	
	boost::property_tree::ptree ptree;
	ofxOscMessage renderedMessage;
	
	ofFbo fbo;
	ofRectangle fboRegion;
	
	static const int COLS_NUM = 4;
	static const int ROWS_NUM = 4;
	static const int POINTS_NUM = 4*4;
	vector<float> verts;
	vector<float> texCoords;
	
	static const int NOT_DRAGGING = -1;
	ofVec2f faderEdges[4];	// clockwise
	vector<float> faderVerts;
	vector<float> faderColors;
	int draggingEdge = NOT_DRAGGING;
	int activeEdge = NOT_DRAGGING;
	
	void setupOscListeners();
	void renderOnFbo();
	void resetMesh(const ofxOscMessage&);
	
	void drawBasically();
	
	void onChangeFader();

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
