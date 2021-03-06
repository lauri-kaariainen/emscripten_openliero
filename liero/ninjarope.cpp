#include "worm.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "gfx/color.hpp"
#include "math.hpp"
//#include <cmath>

void Ninjarope::process(Worm& owner, Game& game)
{
	Common& common = *game.common;
	
	if(out)
	{
		x += velX;
		y += velY;
		
		int ix = ftoi(x), iy = ftoi(y);
		
		anchor = 0;
		for(std::size_t i = 0; i < game.worms.size(); ++i)
		{
			Worm& w = *game.worms[i];
			
			if(&w != &owner
			&& checkForSpecWormHit(game, ix, iy, 1, w))
			{
				anchor = &w;
				break;
			}
		}
		
		fixed forceX, forceY;
		
		fixed diffX = x - owner.x;
		fixed diffY = y - owner.y;
		
		forceX = (diffX << LC(NRForceShlX)) / LC(NRForceDivX);
		forceY = (diffY << LC(NRForceShlY)) / LC(NRForceDivY);
		
		curLen = (vectorLength(ftoi(diffX), ftoi(diffY)) + 1) << LC(NRForceLenShl);
		
		if(ix <= 0
		|| ix >= game.level.width - 1
		|| iy <= 0
		|| iy >= game.level.height - 1
		|| game.level.mat(ix, iy).dirtRock())
		{
			if(!attached)
			{
				length = LC(NRAttachLength);
				attached = true;
				
				if(game.level.inside(ix, iy))
				{
					if(game.level.mat(ix, iy).anyDirt())
					{
						PalIdx pix = game.level.pixel(ix, iy);
						for(int i = 0; i < 11; ++i) // TODO: Check 11 and read from exe
						{
							common.nobjectTypes[2].create2(
								game,
								game.rand(128),
								0, 0,
								x, y,
								pix,
								owner.index,
								0);
						}
					}
				}
			}
			
			
			velX = 0;
			velY = 0;
		}
		else if(anchor)
		{
			if(!attached)
			{
				length = LC(NRAttachLength); // TODO: Should this value be separate from the non-worm attaching?
				attached = true;
			}
			
			if(curLen > length)
			{
				anchor->velX -= forceX / curLen;
				anchor->velY -= forceY / curLen;
			}
			
			velX = anchor->velX;
			velY = anchor->velY;
			x = anchor->x;
			y = anchor->y;
		}
		else
		{
			attached = false;
		}
		
		if(attached)
		{
			// curLen can't be 0

			if(curLen > length)
			{
				owner.velX += forceX / curLen;
				owner.velY += forceY / curLen;
			}
		}
		else
		{
			velY += LC(NinjaropeGravity);

			if(curLen > length)
			{
				velX -= forceX / curLen;
				velY -= forceY / curLen;
			}
		}
	}
}
