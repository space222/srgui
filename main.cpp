#include <stdio.h>
#include <cairo.h>
#include <cstring>
#include <cctype>
#include <utility>
#include <SDL.h>
#include "srgui.h"
#include "srWindow.h"
#include "srDrawSurface.h"
#include "srButton.h"
#include "srCheckbox.h"
#include "srLabel.h"
#include "srRadioButton.h"
#include "srSpinner.h"
#include "srTextField.h"
#include "srProgressBar.h"

bool MainRunning = true;

typedef std::pair<SDL_Surface*, srgui::srPoint> UserSurface;

int main(int argc, char** args)
{

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* MainWindow = SDL_CreateWindow("SRGUI Tests", 0, 0, 1200,720, 0); //SDL_WINDOW_OPENGL
	SDL_Surface* MainWindowSurf = SDL_GetWindowSurface(MainWindow);

	srgui::initialize();

	srgui::srWindow* srwin = srgui::CreateWindow({100,150,300,300}, nullptr, "Window <span color='red'>Test</span> 1");
	srgui::srCheckbox* chk = new srgui::srCheckbox();
	srwin->addChild(chk);
	srgui::srButton* but1 = new srgui::srButton();
	srwin->addChild(but1);
	srgui::srButton* mcl = new srgui::srButton();
	srwin->addChild(mcl);

	mcl->setArea({ 270, 10, 20, 20 });
	mcl->setText("X");
	mcl->onClick = []{ exit(0); };
	chk->setArea({ 50, 110, 100, 20 });
	chk->setText("This is <span color='blue'>checkbox</span>");
	but1->setArea({ 50, 50, 100, 50 });
	but1->setText("button 1");
	but1->onClick = [=]{ if( chk->checked() ) std::puts("It's checked."); else std::puts("It is not."); };
	srgui::srWindow* win2 = srgui::CreateWindow({ 400, 450, 400, 250 }, nullptr, "Window Test 2");

	srgui::srRadioButton* rb1 = new srgui::srRadioButton();
	srgui::srRadioButton* rb2 = new srgui::srRadioButton();
	srgui::srRadioButton* rb3 = new srgui::srRadioButton();
	srgui::srLabel* lbl = new srgui::srLabel();
	win2->addChild(lbl);
	win2->addChild(rb1);
	win2->addChild(rb2); win2->addChild(rb3);
	lbl->setArea({ 50, 132, 100, 20});
	lbl->setText("Labels! <b>Yay!</b>");
	rb1->setArea({ 50, 175, 100, 20});
	rb1->setText("radio b1!");
	rb2->setArea({ 50, 200, 100, 20});
	rb2->setText("radio b2!");
	rb3->setArea({ 50, 227, 100, 20});
	rb3->setText("radio b3!");
	srgui::srRadioGroup* rg = new srgui::srRadioGroup{ rb1, rb2, rb3  };

	srgui::srWindow* win3 = srgui::CreateWindow({ 100,500,350,350 }, nullptr, "Banana 3");
	srgui::srSpinner* spin = new srgui::srSpinner({ 20, 50, 50, 50 });
	srgui::srTextField* fld = new srgui::srTextField();
	srgui::srProgressBar* pb = new srgui::srProgressBar();
	pb->setProgress(66);
	pb->setArea({ 20, 160, 200, 30 });
	fld->setArea({ 20, 120, 230, 30 });
	fld->setText("and ");
	win3->addChild(fld);
	win3->addChild(spin);
	win3->addChild(pb);

	while( MainRunning )
	{
		SDL_Event event;
		while( SDL_PollEvent(&event) ) 
		{
			switch( event.type )
			{
			case SDL_KEYDOWN:
				{
					int k = event.key.keysym.sym;
					int m = 0;
					if( event.key.keysym.sym & 0x40000000 ) break;
					if( event.key.keysym.scancode == SDL_SCANCODE_DELETE )
					{
						m |= (1<<16);
						k = 0;
					} else if( event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE ) {
						m |= (1<<17);
						k = 0;
					} else if( event.key.keysym.mod & KMOD_SHIFT ) {
						k = std::toupper(k);
					}

					srgui::SendEvent(srgui::SR_EVENT_KEY_PRESS, event.key.keysym.mod|m, k, 0,0);
				}
				break;
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

