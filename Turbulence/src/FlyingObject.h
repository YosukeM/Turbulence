//
//  FlyingObject.h
//  Turbulence
//
//  Created by 森本 陽介 on 2013/05/03.
//
//

#ifndef __Turbulence__FlyingObject__
#define __Turbulence__FlyingObject__

class ofxBox2d;
class ofxBox2dBaseShape;

class ofxOscMessage;

/*
 Abstract Class
 */
class FlyingObject {
public:
	typedef enum {
		AKIBIN,
		AKIBIN_2,
		AKIBIN_3,
		AKIKAN,
		AKIKAN_2,
		AKIKAN_3,
		AKIKAN_4,
		COUPLE,
		DOSEI,
		HANA_1,
		HANA_2,
		HANA_3,
		HANA_4,
		HANA_5,
		HANA_6,
		HANA_7,
		HANA_8,
		HAPPA_1,
		HAPPA_2,
		HAPPA_3,
		HAPPA_4,
		HAPPA_5,
		HIKOUKI,
		KAREHA,
		KIRABOSI,
		KURUMA,
		NAGAREBOSI,
		OJISAN,
		PETBOTTLE,
		PETBOTTLE_2,
		PETBOTTLE_3,
		PETBOTTLE_4,
		TOILET_PAPER,
		UFO,
		UFO_2,
		UFO_3,
		UMA_1,
		UMA_2,
		UMA_3,
		UNKO_1,
		UNKO_2,
		USHI,
		KIND_MAX = USHI
	} E_KIND;
	
protected:
	E_KIND kind;
	FlyingObject(E_KIND kind) : kind(kind) {};
	
public:
	virtual ~FlyingObject() {};
	virtual void destroy(ofxBox2d& box2d) {};
	virtual bool update() {};
	virtual void draw() {};
	virtual void generateArgs(ofxOscMessage& message) {};
};

#endif /* defined(__Turbulence__FlyingObject__) */
