//
//  Resources.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//

#ifndef __Turbulence__Resources__
#define __Turbulence__Resources__

#include <string>
#include <tr1/memory>
#include <tr1/unordered_map>

#include <ofMain.h>

class Resource {
	template <typename T> using shared_ptr = std::tr1::shared_ptr<T>;
	template <typename S, typename T> using unordered_map = std::tr1::unordered_map<S, T>;
public:
	static shared_ptr<Resource> getInstance();
	
private:
	
	static shared_ptr<Resource> instance;
	
	unordered_map<std::string, ofImage> images;
	unordered_map<std::string, ofSoundPlayer> sounds;
	
	Resource();
	Resource(const Resource& resource);
public:
	~Resource();
	
public:
	void loadFolder(const char * folderName);
	
	ofImage& getImage(const std::string& fileName);
	
	ofSoundPlayer& getSound(const std::string& fileName);
	
	/*ofImage* getImage(char const * const fileName) const {
		return getImage(std::string(fileName));
	};
	
	ofSoundPlayer* getSound(char const * const fileName) const {
		return getSound(std::string(fileName));
	};*/
};

#endif /* defined(__Turbulence__Resources__) */
