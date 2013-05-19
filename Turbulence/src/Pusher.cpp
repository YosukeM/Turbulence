//
//  Pusher.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/09.
//
//

#include "Pusher.h"

using namespace std;
using namespace std::tr1;

const string Pusher::PUSHER_CLIENT = "openframeworks-ofxPusher";
const string Pusher::VERSION = "0.1";
const string Pusher::HOST = "ws.pusherapp.com";

Pusher::Pusher() {
	
}

void Pusher::setup(const string& key) {
	apiKey = key;
}


void Pusher::sendSubscribeMessage(Channel& channel) {
	channel.send("pusher:subscribe", Json::Value());
}

void Pusher::sendUnsubscribeMessage(Channel& channel) {
	channel.send("pusher:unsubscribe", Json::Value());
}

void Pusher::connect() {
	_shouldReconnect = false;
	websocket.addListener(this);
	
	auto options = ofxLibwebsockets::defaultClientOptions();
	options.host = HOST;
	options.channel = "/app/" + apiKey + "?client=" + PUSHER_CLIENT + "&version=" + VERSION;
	//options.protocol = "5";
	websocket.connect(options);
}

void Pusher::disconnect() {
	websocket.close();
	_shouldReconnect = false;
}

void Pusher::subscribe(const string& channelName) {
	Channel c(this, channelName);
	sendSubscribeMessage(c);
	channels.insert(pair<string, Channel>(channelName, c));
}

void Pusher::unsubscribe(const string& channelName) {
	auto itr = channels.find(channelName);
	if (itr != channels.end()) {
		sendUnsubscribeMessage(itr->second);
		channels.erase(itr);
	}
}

void Pusher::setListener(const std::string& event, listener_t listener) {
	listeners[event] = listener;
}

void Pusher::onConnect(ofxLibwebsockets::Event&) {
	
}

void Pusher::onOpen(ofxLibwebsockets::Event&) {
	
}

void Pusher::onClose(ofxLibwebsockets::Event&) {
	
}

void Pusher::onIdle(ofxLibwebsockets::Event&) {
	
}

void Pusher::dispatchChannelEvent(const ofxLibwebsockets::Event& e) {
	string event = e.json.get("event", Json::Value::null).asString();
	auto channelName = e.json.get("channel", Json::Value::null).asString();
	auto itr = channels.find(channelName);
	if (itr != channels.end()) {
		auto& channel = itr->second;
		auto itr2 = channel.listeners.find(event);
		if (itr2 != channel.listeners.end()) {
			itr2->second(e.message);
		}
	}
}


void Pusher::onMessage(ofxLibwebsockets::Event& e) {
	Json::Value json;
	Json::Reader reader;
	auto data = e.json.get("data", Json::Value::null);
	
	if (data.type() == Json::objectValue) {
		cout << data.toStyledString() << endl;
		if (data.get("code", Json::Value::null).asInt() == 4201) {
			_shouldReconnect = true;
		}
	} else {
		reader.parse(data.asString(), json, false);
		
		string event = e.json.get("event", Json::Value::null).asString();
		
		cout << event << endl;
		
		if (event == "pusher:ping") {
			websocket.send("{\"event\":\"pusher:pong\"}");
		} else {
			auto itr = listeners.find(event);
			if (itr != listeners.end()) {
				itr->second(json);
			}
			// dispatchChannelEvent(e);
		}
	}
}

bool Pusher::isConnected() {
	return websocket.getConnection() != NULL;
}

void Pusher::onBroadcast(ofxLibwebsockets::Event&) {
	
}


Pusher::Channel::Channel(Pusher* parent, const string& name) {
	this->parent = parent;
	channelName = name;
}

void Pusher::Channel::send(const string& eventName, const Json::Value& data) {
	Json::Value message;
	Json::Value data2 = data;
	data2["channel"] = channelName;
	message["event"] = eventName;
	message["data"] = data2;
	parent->websocket.send(message.toStyledString());
}

void Pusher::Channel::bind(const std::string& eventName, listener_t listener) {
	listeners[eventName] = listener;
}