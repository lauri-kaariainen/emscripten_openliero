
#ifndef SDLMAIN_HPP
#define SDLMAIN_HPP


#include <string>

#include "worm.hpp"



#ifndef LOOPSTATUSSTRUCT_H
#define LOOPSTATUSSTRUCT_H


struct loopStatusStruct {
	std::string loopStatusVar = "initial";
	std::string loopStatusVarPrevious = "initial";
	int menuselect = -1;
	int profileMenuSelect = 0;
	WormSettings* wormSettingsWeAreHandling;
	//void* fileSelectorWeAreHandling;
};

extern loopStatusStruct globalLoopStatus;

#endif

#endif

