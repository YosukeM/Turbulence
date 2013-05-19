//
//  OscServer.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/07.
//
//

#include "OscServer.h"

using namespace std;
using namespace std::tr1;


const int OscServer::RECEIVING_PORT = 12000;
const int OscServer::SENDING_PORT = 32000;

void OscServer::setup() {
	receiver.setup(RECEIVING_PORT);
}

OscServer::~OscServer() {
	for (auto& connection : connections) {
		delete connection.second;
	}
}

void OscServer::update() {
	ofxOscMessage message;
	while (receiver.hasWaitingMessages()) {
		receiver.getNextMessage(&message);
		cout << message.getAddress() << endl;
		if (message.getAddress() == "/server/connect") {
			Sender* sender = new Sender();
			sender->setup(message.getRemoteIp(), SENDING_PORT);
			sender->id = message.getArgAsInt32(0);
			pair<string, Sender*> pair(message.getRemoteIp(), sender);
			connections.insert(pair);
			
			onConnectFunc(sender->id);
		} else if (message.getAddress() == "/server/disconnect") {
			connections.erase(message.getRemoteIp());
		}
	}
}

void OscServer::sendMessageTo(ofxOscMessage& message, int id) {
	cout << "messageTo:" << id << endl;
	cout << message.getAddress() << endl;
	for (auto& connection : connections) {
		if (connection.second->id == id) {
			connection.second->sendMessage(message);
		}
	}
}

void OscServer::sendMessageToAll(ofxOscMessage& message) {
	for (auto& connection : connections) {
		connection.second->sendMessage(message);
	}
}

void OscServer::setOnConnect(onconnect_func_t func) {
	onConnectFunc = func;
}