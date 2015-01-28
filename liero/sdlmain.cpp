#include <gvl/support/platform.hpp>
#include <exception>
#include <string>

void single_iter_for_emscripten_arg(void* arg);




#include <SDL/SDL.h>
#include "gfx.hpp"
#include <SDL/SDL.h>
#if !SDL13
#include <SDL/SDL_stdinc.h>
//#include <SDL/SDL_getenv.h>
#endif

#include "gfx.hpp"
#include "sfx.hpp"
#include "sys.hpp"
#include "game.hpp"
#include "viewport.hpp"
#include "worm.hpp"
#include "reader.hpp"
#include "filesystem.hpp"
#include "text.hpp"
#include "keys.hpp"
#include "constants.hpp"
#include "math.hpp"
#include "console.hpp"
#include <gvl/support/platform.hpp>

#include <iostream>
#include <ctime>
#include <exception>
#include <gvl/math/cmwc.hpp>
																																																																																																							
#include <emscripten.h>
#include "sdlmain.hpp"

#ifdef main
#undef main
#endif

loopStatusStruct globalLoopStatus;


int main(int argc, char *argv[])
{
	// TODO: Better PRNG seeding
	gfx.rand.seed(Uint32(std::time(0)));
	
	bool exeSet = false;
	
	std::string exePath;
	
	for(int i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'v':
				// SDL_putenv seems to take char* in linux, STOOPID
				SDL_putenv(const_cast<char*>((std::string("SDL_VIDEODRIVER=") + &argv[i][2]).c_str()));
			break;
			}
		}
		else
		{
			exePath = argv[i];
			exeSet = true;
		}
	}
	
	if(!exeSet)
		exePath = "LIERO.EXE";

	setLieroEXE(exePath);
	
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	
	initKeys();
	
	loadTablesFromEXE();
	
	gvl::shared_ptr<Common> common(new Common(exePath));
	gfx.common = common;
	
	gfx.loadPalette(*common); // This gets the palette from common
	
	gfx.loadMenus();
	
	
	gfx.init();
	
	if(!gfx.loadSettings(joinPath(lieroEXERoot, "LIERO")))
	{
	
		gfx.settings.reset(new Settings);
	
		gfx.saveSettings(joinPath(lieroEXERoot, "LIERO"));
	}
	
	gfx.setVideoMode();
	sfx.init();
	




	#ifdef EMSCRIPTEN
	
	  emscripten_set_main_loop_arg(single_iter_for_emscripten_arg,&globalLoopStatus, 0, 1);
	
	#else
	  while (1)
	  {
	    single_iter_for_emscripten();
	  }
	#endif   
 	
	
	gfx.settings->save(joinPath(lieroEXERoot, "LIERO.DAT"), gfx.rand);
	
	sfx.deinit();
	SDL_Quit();
	
	//gvl::present_profile(std::cout);
	
	return 0;
}


	

void single_iter_for_emscripten_arg(void *arg){
	gfx.mainLoop((loopStatusStruct*)arg);
}