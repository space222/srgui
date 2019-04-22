#include <stdio.h>
#include <cairo.h>
#include <cstring>
#include <utility>
#include <SDL.h>
#include "srgui.h"
#include "srWindow.h"
#include "srDrawSurface.h"
#include "srButton.h"

bool MainRunning = true;

typedef std::pair<SDL_Surface*, srgui::srPoint> UserSurface;

int main(int argc, char** args)
{

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* MainWindow = SDL_CreateWindow("SRGUI Tests", 0, 0, 1200,720, 0); //SDL_WINDOW_OPENGL
	SDL_Surface* MainWindowSurf = SDL_GetWindowSurface(MainWindow);

	srgui::initialize();

	srgui::srWindow* srwin = srgui::CreateWindow({100,150,300,300}, nullptr, "Window Test 1");
	srgui::srButton* but1 = new srgui::srButton();
	srwin->addChild(but1);

	but1->setArea({ 50, 50, 100, 50 });
	but1->setText("clicky");
	but1->onClick = []{ std::puts("He clicked me!"); };
	srgui::srWindow* win2 = srgui::CreateWindow({ 400, 450, 400, 250 }, nullptr, "Window Test 2");

	srgui::CreateWindow({ 500,500,200,200 }, nullptr, "Banana 3");

	while( MainRunning ) 
	{
		SDL_Event event;
		while( SDL_PollEvent(&event) ) 
		{
			switch( event.type )
			{
			case SDL_MOUSEMOTION:
			srgui::SendEvent(srgui::SR_EVENT_MOUSE_MOVE, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
				break;
			case SDL_MOUSEBUTTONDOWN:
				srgui::SendEvent(srgui::SR_EVENT_MOUSE_DOWN, event.button.button, event.button.x, event.button.y, 0);
				break;
			case SDL_MOUSEBUTTONUP:
				srgui::SendEvent(srgui::SR_EVENT_MOUSE_UP, event.button.button, event.button.x, event.button.y, 0);
				break;
			case SDL_WINDOWEVENT:
				if( event.window.event == SDL_WINDOWEVENT_CLOSE )
				{
					MainRunning = false;
				}
				break;
			case SDL_QUIT:
				MainRunning = false;
				break;
			}
		}
		
		SDL_FillRect(MainWindowSurf, nullptr, 0xE0E0E0);

		std::vector<srgui::srRenderTask> tasks;
		srgui::generateDrawList(tasks);

		for(int i = 0; i < tasks.size(); ++i)
		{
			UserSurface *surf = (UserSurface*) tasks[i].surface->getUserData();		
			if( !surf )
			{
				srgui::srCairoDrawSurface* cds =(srgui::srCairoDrawSurface*) tasks[i].surface;
				uint8_t* buf = cds->getBuffer();
				int stride = cds->getStride();
				SDL_Surface* sdlsurf = SDL_CreateRGBSurfaceFrom((void*)buf, tasks[i].extents.width, tasks[i].extents.height, 
						32, stride, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

				srgui::srPoint p = { (int)tasks[i].extents.width, (int)tasks[i].extents.height };
				surf = new UserSurface(sdlsurf, p);

				cds->setUserData(surf);
			} else if( tasks[i].dirty ) {
				UserSurface& p = *(UserSurface*) tasks[i].surface->getUserData();
				if( p.second.x != tasks[i].extents.width || p.second.y != tasks[i].extents.height )
				{
					tasks[i].surface->setSize(p.second.x, p.second.y);
					SDL_FreeSurface(p.first);
					srgui::srCairoDrawSurface* cds =(srgui::srCairoDrawSurface*) tasks[i].surface;
					uint8_t* buf = cds->getBuffer();
					int stride = cds->getStride();
					SDL_Surface *sdlsurf = SDL_CreateRGBSurfaceFrom((void*)buf, tasks[i].extents.width, 
							tasks[i].extents.height, 32, stride, 0xFF0000, 0xFF00, 0xFF, 0xFF000000);

					p.first = sdlsurf;
					p.second = { (int)tasks[i].extents.width, (int)tasks[i].extents.height };
				} else {
					
					SDL_LockSurface(surf->first);
					srgui::srCairoDrawSurface* cds =(srgui::srCairoDrawSurface*) tasks[i].surface;
					uint8_t* buf = cds->getBuffer();
					int stride = cds->getStride();
					std::memcpy(surf->first->pixels, buf, stride*tasks[i].extents.height*4);

					SDL_UnlockSurface(surf->first);
				}
			}
			SDL_BlitSurface(surf->first, nullptr, MainWindowSurf, (SDL_Rect*) &tasks[i].extents);

		}
		
		//SDL_Rect r{ tasks[i].extents.x, tasks[i].extents.y, tasks[i].extents.width, tasks[i].extents.height };
		
		SDL_UpdateWindowSurface(MainWindow);	
	}

	return 0;
}

