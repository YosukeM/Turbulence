//
//  OscClient.h
//  TurbulenceClient
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#ifndef __TurbulenceClient__OscClient__
#define __TurbulenceClient__OscClient__

#include "ofxOscSender.h"
#include "ofxOscReceiver.h"

#include <tr1/functional>
#include <map>

class testApp;

class OscClient {
public:
	typedef std::tr1::function<void(const ofxOscMessage&)> listener_t;
	
private:
	static const int SENDING_PORT;
	static const int RECEIVING_PORT;
	
	bool hasConnected = false;
	std::map<std::string, listener_t> listeners;
	ofxOscReceiver receiver;
	ofxOscSender sender;
	
public:
	OscClient();
	
	void connect(const std::string& serverIp, int clientId);
	void disconnect();
	
	bool isConnected() {
		return hasConnected;
	}
	
	void update();
	
	void setListener(const std::string& , listener_t func);
};

#endif /* defined(__TurbulenceClient__OscClient__) */
