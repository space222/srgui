#include <stdio.h>
#include <cairo.h>
#include <cstring>
#include <cctype>
#include <cstdint>
#include <utility>
#include <memory>
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
#include "srGroupBox.h"
#include "srMenuBar.h"
#include "srHScrollBar.h"

bool MainRunning = true;

typedef std::pair<SDL_Surface*, srgui::srPoint> UserSurface;
using std::unique_ptr;

void sdl_text_focus_callback(bool on)
{
	if( on )
	{
		SDL_StartTextInput();
	} else {
		SDL_StopTextInput();
	}
	return;
}

int main(int argc, char** args)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* MainWindow = SDL_CreateWindow("SRGUI Tests", 0, 0, 1200,720, 0); //SDL_WINDOW_OPENGL
	SDL_Surface* MainWindowSurf = SDL_GetWindowSurface(MainWindow);

	srgui::initialize();

	srgui::setTextFocusCallback(sdl_text_focus_callback);

	unique_ptr<srgui::srWindow> srwin(srgui::CreateWindow({100,150,300,300}, nullptr, "Window <span color='red'>Test</span> 1"));
	srgui::srCheckbox* chk = new srgui::srCheckbox();
	srwin->addChild(chk);
	srgui::srButton* but1 = new srgui::srButton("button 1", {50,80});
	srwin->addChild(but1);
	srgui::srButton* mcl = new srgui::srButton("X", {270,10});
	mcl->setSize(20, 20);
	srwin->addChild(mcl);
	mcl->onClick = []{ exit(0); };

	srgui::srMenuBar* mbar = new srgui::srMenuBar();
	srwin->addChild(mbar);
	mbar->setArea({ 0, 0, 300, 20 });

	srgui::srMenu* file = new srgui::srMenu("File", {{ "Open", [=]{ puts("open clicked."); }}, {"Close", [=]{puts("Close clicked");}}});
	srgui::srMenu* view = new srgui::srMenu("View", {{ "Source", [=]{ puts("src clicked."); }}, {"Data", [=]{puts("data clicked");}}});
	mbar->add(file);
	mbar->add(view);

	chk->setArea({ 50, 140, 160, 20 });
	chk->setText("This is <span color='blue'>checkbox</span>");
	
	but1->onClick = [=]{ if( chk->checked() ) std::puts("It's checked."); else std::puts("It is not."); };
	unique_ptr<srgui::srWindow> win2(srgui::CreateWindow({ 400, 450, 400, 250 }, nullptr, "Window Test 2"));

	srgui::srRadioButton* rb1 = new srgui::srRadioButton();
	srgui::srRadioButton* rb2 = new srgui::srRadioButton();
	srgui::srRadioButton* rb3 = new srgui::srRadioButton();
	srgui::srGroupBox* grpb = new srgui::srGroupBox("Group box", {40, 70, 150, 150});
	
	srgui::srLabel* lbl = new srgui::srLabel("Labels! <b>Yay!</b>", {50, 42} );
	win2->addChild(lbl);
	win2->addChild(grpb);
	grpb->addChild(rb1);
	grpb->addChild(rb2); 
	grpb->addChild(rb3);
	srgui::srSpinner* spin2 = new srgui::srSpinner({ 5, 80, 50, 50 });
	grpb->addChild(spin2);

	//lbl->setArea({ 50, 42, 100, 20});
	//lbl->setText();
	rb1->setArea({ 5, 20, 100, 20});
	rb1->setText("radio b1!");
	rb2->setArea({ 5, 40, 100, 20});
	rb2->setText("radio b2!");
	rb3->setArea({ 5, 60, 100, 20});
	rb3->setText("radio b3!");
	unique_ptr<srgui::srRadioGroup> rg(new srgui::srRadioGroup{ rb1, rb2, rb3  });

	unique_ptr<srgui::srWindow> win3(srgui::CreateWindow({ 100,500,350,350 }, nullptr, "Banana 3"));
	srgui::srSpinner* spin = new srgui::srSpinner({ 20, 50, 50, 50 });
	srgui::srTextField* fld = new srgui::srTextField({ 20, 120, 230, 30 });
	srgui::srProgressBar* pb = new srgui::srProgressBar({ 20, 190, 200, 30 });
	srgui::srHScrollBar *scl = new srgui::srHScrollBar({ 20, 150, 230, 20 });
	scl->setScrollInfo(200, 30);
	
	fld->setText("and ");
	pb->setProgress(66);

	win3->addChild(fld);
	win3->addChild(spin);
	win3->addChild(pb);
	win3->addChild(scl);

	while( MainRunning )
	{
		SDL_Event event;
		while( SDL_PollEvent(&event) ) 
		{
			switch( event.type )
			{
			case SDL_TEXTINPUT:
				srgui::SendEvent(srgui::SR_EVENT_KEY_PRESS, 0, event.text.text[0], 0, 0);
				break;
			case SDL_KEYDOWN:{
				int m = 0;
				bool k = false;
				if( event.key.keysym.scancode == SDL_SCANCODE_DELETE )
				{
					m |= (1<<16);
					k = true;
				} else if( event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE ) {
					m |= (1<<17);
					k = true;
				}
				if( k ) srgui::SendEvent(srgui::SR_EVENT_KEY_PRESS, m, 0, 0,0);
				}break;
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

		for(uint32_t i = 0; i < tasks.size(); ++i)
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

	SDL_Quit();

	return 0;
}

