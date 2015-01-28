
#ifndef LOOPSTATUS_HPP
#define LOOPSTATUS_HPP



#include "worm.hpp"
#include <string>



struct loopStatusStruct {
	std::string loopStatusVar = "initial";
	std::string loopStatusVarPrevious = "initial";
	int menuselect = -1;
	WormSettings wormSettingsWeAreHandling;
	void* fileSelectorWeAreHandling;
	
};



#endif

extern loopStatusStruct globalLoopStatus;