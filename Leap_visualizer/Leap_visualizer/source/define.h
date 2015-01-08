#pragma once

#include <Leap.h>
#include <iostream>

#define PI				3.141592

#define SWAP(a,b,t)		((t)=(a), (a)=(b), (b)=(t))

typedef struct _FingerStruct{
	Leap::Vector FingerBone[4];
}FingerStruct;

typedef struct _HandStruct{
	int32_t hid;
	Leap::Vector Palmpos;
	Leap::Vector Wristpos;
	Leap::Vector Elbowpos;
	//FingersStruct - thumb, index, middle, ring, pinky ¼ø¼­
	FingerStruct Fingers[5];
}HandStruct;

typedef struct _HandsStruct{
	bool bleft;
	bool bright;
	HandStruct LeftHand;
	HandStruct RightHand;
}HandsStruct;