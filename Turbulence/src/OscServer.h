//
//  OscServer.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#ifndef __Turbulence__OscServer__
#define __Turbulence__OscServer__

#include "ofxOscReceiver.h"
#include "ofxOscSender.h"
#include <map>
#include <tr1/tuple>

class OscServer {
public:
	static const int RECEIVING_PORT;
	static const int SENDING_PORT;
	
	typedef std::tr1::function<void(int)> onconnect_func_t;
	
private:
	ofxOscReceiver receiver;
	
	struct Sender : public ofxOscSender {
		int id;
	};
	
	std::map<std::string, Sender*> connections;
	onconnect_func_t onConnectFunc;

public:
	~OscServer();
	void setup();
	void update();
	void sendMessageTo(ofxOscMessage& message, int id);
	void sendMessageToAll(ofxOscMessage& message);
	void setOnConnect(onconnect_func_t func);
};

#endif /* defined(__Turbulence__OscServer__) */
