//
//  OscClient.cpp
//  TurbulenceClient
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#include "OscClient.h"
const int OscClient::RECEIVING_PORT = 32000;
const int OscClient::SENDING_PORT = 12000;
	
OscClient::OscClient() {
	
}

void OscClient::connect(const std::string& serverIp, int clientId) {
	sender.setup(serverIp, SENDING_PORT);
	receiver.setup(RECEIVING_PORT);
	
	ofxOscMessage message;
	message.setAddress("/server/connect");
	message.addIntArg(clientId);
	sender.sendMessage(message);
	
	hasConnected = true;
}

void OscClient::disconnect() {
	ofxOscMessage message;
	message.setAddress("/server/disconnect");
	sender.sendMessage(message);
	hasConnected = false;
}

void OscClient::update() {
	ofxOscMessage message;
	while (receiver.hasWaitingMessages()) {
		receiver.getNextMessage(&message);
		cout << message.getAddress() << endl;
		if (listeners.find(message.getAddress()) != listeners.end()) {
			listeners[message.getAddress()](message);
		}
	}
}

void OscClient::setListener(const std::string& address, listener_t func) {
	listeners[address] = func;
}
