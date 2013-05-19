//
//  Resources.cpp
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/06.
//
//

#include "Resource.h"

template <typename T> using shared_ptr = std::tr1::shared_ptr<T>;

shared_ptr<Resource> Resource::instance;

shared_ptr<Resource> Resource::getInstance() {
	if (!instance) {
		instance.reset(new Resource());
	}
	return instance;
}

Resource::Resource() {
	
}

Resource::~Resource() {
}

void Resource::loadFolder(const char * folderName) {
	ofDirectory dir;
	dir.open(ofToDataPath(folderName, true));
	dir.listDir();
	
	for (auto& file : dir.getFiles()) {
		if (file.getExtension() == "png") {
			images.insert(decltype(images)::value_type(file.getFileName(), ofImage()))
				.first->second.loadImage(file.path());
		} else if (file.getExtension() == "wav") {
			sounds.insert(decltype(sounds)::value_type(file.getFileName(), ofSoundPlayer()))
			.first->second.loadSound(file.path());
		}
	}
	
	dir.close();
}

ofImage& Resource::getImage(const std::string& fileName) {
	return images[fileName];
}

ofSoundPlayer& Resource::getSound(const std::string& fileName) {
	return sounds[fileName];
}
