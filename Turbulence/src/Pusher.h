//
//  Pusher.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/09.
//
//

#ifndef __Turbulence__Pusher__
#define __Turbulence__Pusher__

#include <string>
#include <tr1/functional>
#include <tr1/unordered_map>
#include "ofxLibwebsockets.h"

class Pusher {
public:
	typedef std::tr1::function<void(const Json::Value&)> listener_t;
	
private:
	class Channel {
		friend class Pusher;
		Pusher* parent;
		std::string channelName;
		std::tr1::unordered_map<std::string, listener_t> listeners;
	public:
		Channel(Pusher* parent, const std::string& name);
		void send(const std::string& eventName, const Json::Value& data);
		void bind(const std::string& eventName, listener_t listener);
	};
	
	static const std::string PUSHER_CLIENT;
	static const std::string VERSION;
	static const std::string HOST;
	
	ofxLibwebsockets::Client websocket;
	std::string apiKey;
	std::tr1::unordered_map<std::string, Channel> channels;
	std::tr1::unordered_map<std::string, listener_t> listeners;
	bool _shouldReconnect = false;
	
	Pusher(Pusher&);
	void sendSubscribeMessage(Channel& channel);
	void sendUnsubscribeMessage(Channel& channel);
	void dispatchChannelEvent(const ofxLibwebsockets::Event& event);
	
public:
	Pusher();
	void setup(const std::string& key);
	
	bool shouldReconnect() {
		return _shouldReconnect;
	}
	
	void connect();
	void disconnect();
	
	bool isConnected();
	
	void subscribe(const std::string& channel);
	void unsubscribe(const std::string& channel);
	
	void setListener(const std::string& event, listener_t listener);
	
	void onConnect(ofxLibwebsockets::Event&);
	void onOpen(ofxLibwebsockets::Event&);
	void onClose(ofxLibwebsockets::Event&);
	void onIdle(ofxLibwebsockets::Event&);
	void onMessage(ofxLibwebsockets::Event&);
	void onBroadcast(ofxLibwebsockets::Event&);
};

#endif /* defined(__Turbulence__Pusher__) */
