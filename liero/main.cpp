#ifndef UUID_DC1D9513CDD34960AB8A648004DA149D
#define UUID_DC1D9513CDD34960AB8A648004DA149D

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

#if __APPLE__
#define gameEntry SDL_main
#endif



int gameEntry(int argc, char* argv[])
try
{
	// TODO: Better PRNG seeding
	gfx.rand.seed(Uint32(std::time(0)));
	
	bool exeSet = false;
	
	std::string exePath;
	std::cout << "first step" << std::endl;
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
	std::cout << "second step" << std::endl;
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	
	initKeys();
	std::cout << "2.1" << std::endl;
	loadTablesFromEXE();
	std::cout << "2.2" << std::endl;
	gvl::shared_ptr<Common> common(new Common(exePath));
	gfx.common = common;
	std::cout << "2.3" << std::endl;
	gfx.loadPalette(*common); // This gets the palette from common
	std::cout << "2.4" << std::endl;
	gfx.loadMenus();
	
	std::cout << "third step" << std::endl;
	gfx.init();
	std::cout << "3.1" << std::endl;
	if(!gfx.loadSettings(joinPath(lieroEXERoot, "LIERO")))
	{
		std::cout << "3.2" << std::endl;
		gfx.settings.reset(new Settings);
		std::cout << "3.3" << std::endl;
		gfx.saveSettings(joinPath(lieroEXERoot, "LIERO"));
	}
	std::cout << "fourth step" << std::endl;
	gfx.setVideoMode();
	sfx.init();
	std::cout << "fifth step" << std::endl;


 	//gfx.mainLoop();
	std::cout << "5.1" << std::endl;
	gfx.settings->save(joinPath(lieroEXERoot, "LIERO.DAT"), gfx.rand);
	std::cout << "sixth step" << std::endl;
	sfx.deinit();
	SDL_Quit();
	
	//gvl::present_profile(std::cout);
	
	return 0;
}

catch(std::exception& e)
{
	std::cout << e.what()<< std::endl;
	SDL_Quit();
	throw;
}

#endif // UUID_DC1D9513CDD34960AB8A648004DA149D
