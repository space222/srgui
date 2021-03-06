#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srgui.h"
#include "srControl.h"
#include "srWindow.h"
#include "srDrawSurface.h"
#include "srButton.h"
#include "srCheckbox.h"
#include "srLabel.h"

namespace srgui {

system_data srgui_data;
srControl* findControlUnderPoint(srWindow* W, const srPoint& p);
srWindow* findWindowUnderPoint(const srPoint& p);

void initialize()
{
	setDefaultStyle();
	srgui_data.caption_move = false;
	return;
}

void setDefaultStyle()
{
	srLookAndFeel& laf = srgui_data.UIStyle;

	laf.windowStyle |= (int)SR_SF_WINDOW_ROUND_BOTTOM;
	laf.windowBackground = 0xc0c0be;
	laf.windowCaptionColor1 = 0x000079;
	laf.windowCaptionColor2 = 0x3c82a4;
	laf.windowCaptionTextColor = 0xF0F0F0;
	laf.windowCaptionHeight = 40;
	laf.windowCaptionRounding = 15;
	laf.windowCaptionOffset = {0,0};
	laf.buttonBackground = 0xc0c0be;
	laf.buttonRounding = 0;
	laf.buttonOutline = 0x3030A0;
	laf.itemSelectionColor = 0x2a448a; //0x7070FF;
	laf.textBackground = 0xF0F0F0;
	return;
}

bool point_in_rect(const srRect& r, const srPoint& p)
{
	return (p.x >= r.x && p.x < (r.x+r.width) &&
		p.y >= r.y && p.y < (r.y+r.height) );
}

srPoint get_control_relative_point(srWindow* win, srControl* c, const srPoint& p)
{
	srRect win_area, c_area;
	win->getArea(win_area);
	c->getArea(c_area);

	if( win->overlay == c )
	{
		return { p.x - c_area.x, p.y - c_area.y };
	}
	
	srPoint res{ (p.x - win_area.x), (p.y - win_area.y) };
	
	srControl* par = c->getParent();
	while( par && par != win )
	{
		srRect parea;
		par->getArea(parea);
		res.x -= parea.x;
		res.y -= parea.y;
		par = par->getParent();
	}

	res.x -= c_area.x;
	res.y -= c_area.y;
	return res;
}

srControl* find_in_container(srContainer* con, const srPoint& p)
{
	srControl* C = nullptr;

	for(uint32_t i = 0; i < con->getNumChildren(); ++i)
	{
		if( ! con->point_in_child(i, p) ) 
		{
			continue;
		}
		C = con->getChild(i);
		srContainer* recurse = dynamic_cast<srContainer*>(C);
		if( recurse )
		{
			srRect r;
			C->getArea(r);
			srControl* temp = find_in_container(recurse, {p.x - r.x, p.y - r.y});
			if( temp ) C = temp;
		}
		break;
	}

	return C;
}

void SendEvent(srEventType event, int data0, int data1, int data2, int data3)
{
	if( event == SR_EVENT_KEY_PRESS )
	{
		int mods = data0;
		int key = data1;

		if( ! srgui_data.windows[0] )
		{
			return;
		}

		srIEvent* ev = dynamic_cast<srIEvent*>( srgui_data.windows[0]->getChildFocus() );

		if( ! ev )
		{
			return;
		}

		ev->raiseKeyPressEvent({{0,0}, (uint32_t)mods, (uint32_t)key});
		
		return;
	}
	if( event == SR_EVENT_MOUSE_DOWN )
	{
		int button = data0;
		int x = data1;
		int y = data2;
		srgui_data.mouse_pos = {x,y};

		// if we're not even over a window, we can ditch early
		if( ! srgui_data.mouse_over ) 
		{
			if( srgui_data.windows[0] )
			{
				srgui_data.windows[0]->closeOverlay();
			}
			return;
		}

		// if we were over a window that isn't already top
		// find it and bring it up
		if( srgui_data.mouse_over && srgui_data.mouse_over.window != srgui_data.windows[0] )
		{
			// remove focus from the control on the old window[0] that had it
			srControl* fc = srgui_data.windows[0]->getChildFocus();
			if( fc && (fc->flags & SR_CF_REPAINT_WHILE_FOCUS) )
			{
				srgui_data.windows[0]->setDirty();
			}
			srIEvent* ev = dynamic_cast<srIEvent*>(fc);
			if( ev ) ev->raiseLoseFocusEvent({});
			srgui_data.windows[0]->setChildFocus(nullptr);

			// old window gets overlay controls (just menus for now) closed
			srgui_data.windows[0]->closeOverlay();
			
			// find and pull up the new window
			for(uint32_t i = 0; i < srgui_data.windows.size(); ++i)
			{
				if( srgui_data.mouse_over.window == srgui_data.windows[i] )
				{
					for(int j = i; j > 0; j--) srgui_data.windows[j] = srgui_data.windows[j-1];
					srgui_data.windows[0] = srgui_data.mouse_over.window;
					break;
				}
			}
		}

		if( button == 1 && !srgui_data.mouse_over.child )
		{    // we can only end up clicking on the top level window
		     // if we're not clicking a control, see if its time to
		     // drag by the caption
			srgui_data.mouse_l_down.window = srgui_data.windows[0];
			srRect r, c;
			srgui_data.windows[0]->getArea(r);
			c = r;
			c.height = srgui_data.UIStyle.windowCaptionHeight;
			if( (srgui_data.caption_move = point_in_rect( c, {x,y} )) ) 
			{
				srgui_data.windows[0]->closeOverlay(); // window drag closes menus
				return;
			}
		}

		switch( button )
		{
		case 1:	srgui_data.mouse_l_down = srgui_data.mouse_over; break;
		case 2:	srgui_data.mouse_m_down = srgui_data.mouse_over; break;
		case 3: srgui_data.mouse_r_down = srgui_data.mouse_over; break;
		}

		if(srControl *c = srgui_data.mouse_over.child; c )
		{
			srIEvent* ev = dynamic_cast<srIEvent*>(c);
			if( ev ) ev->raiseMouseDownEvent({get_control_relative_point(srgui_data.windows[0], c, {x,y}), 0,0,button});
		}

		srControl* fc = srgui_data.windows[0]->getChildFocus();
		if( fc && (fc->flags & SR_CF_REPAINT_WHILE_FOCUS) )
		{
			srgui_data.windows[0]->setDirty();
		}

		if( srgui_data.mouse_over && srgui_data.mouse_over.child )
		{
			if( srgui_data.mouse_over.child != fc )
			{
				srIEvent* ev = dynamic_cast<srIEvent*>(fc);
				if( ev ) ev->raiseLoseFocusEvent({});
				ev = dynamic_cast<srIEvent*>(srgui_data.mouse_over.child);
				if( ev ) ev->raiseGainFocusEvent({});
			}
			srgui_data.windows[0]->setChildFocus(srgui_data.mouse_over.child);
		} else {
			srIEvent* ev = dynamic_cast<srIEvent*>(fc);
			if( ev ) ev->raiseLoseFocusEvent({});
			srgui_data.windows[0]->setChildFocus(nullptr);
		}
	
		if( srgui_data.windows[0]->overlay 
			&& !dynamic_cast<srControl*>(srgui_data.windows[0]->isPointInOverlay({x,y})) )
		{
			// if the mouse is down on something that isn't the current overlay, close the overlay.
			// this used to do a != of mouse_over.child with isPointInOverlay but would fail on the
			// *second* click of a menu separator item resulting in odd state of any control on the
			// window underneath. This appears to work now with better logic, but there still must
			// be a logic error in detecting a mouse hover or click on a menu.
			srgui_data.windows[0]->closeOverlay();
		}

		if( srgui_data.mouse_l_down && srgui_data.mouse_l_down.child 
				&& (srgui_data.mouse_l_down.child->getFlags() & SR_CF_REPAINT_ON_LBUTTON_STATE) )
		{
			if( srgui_data.mouse_l_down.child != srgui_data.windows[0]->overlay )
				srgui_data.mouse_over.window->setDirty();
		}

		return;
	}

	if( event == SR_EVENT_MOUSE_UP )
	{
		int button = data0;
		int x = data1;
		int y = data2;
		srgui_data.mouse_pos = {x,y};
		srgui_data.caption_move = false;

		switch( button )
		{
		case 1:
			if( srgui_data.mouse_l_down.child && srgui_data.mouse_l_down == srgui_data.mouse_over )
			{
				srIEvent* c = dynamic_cast<srIEvent*>(srgui_data.mouse_l_down.child);
				if( c )
				{
					srWindow* win = srgui_data.mouse_l_down.window;
					if( win->overlay && srgui_data.mouse_l_down.child 
								== dynamic_cast<srControl*>(win->isPointInOverlay({x,y})) )
					{
						srRect &r = srgui_data.mouse_l_down.child->area;
						c->raiseClickEvent({{x - r.x, y - r.y}, 0,0});
						
						if( srWindow* W; !srgui_data.windows[0]->overlay 
								&& (W = srgui_data.mouse_over.window = findWindowUnderPoint({x,y})) )
						{  // need to refill what's under the mouse
							srgui_data.mouse_over.child = findControlUnderPoint(W, {x,y});
						} else {
							srgui_data.mouse_over.child = nullptr;
						}
					} else {
						c->raiseClickEvent({get_control_relative_point(win, 
								srgui_data.mouse_l_down.child, {x,y}), 0,0});
					}
				}
				else printf("It wasn't clickable! %i\n", srgui_data.mouse_l_down.child->type());
			}
			if( srgui_data.mouse_l_down && srgui_data.mouse_l_down.child 
				&& (srgui_data.mouse_l_down.child->getFlags() & (SR_CF_REPAINT_ON_LEFT_CLICK|SR_CF_REPAINT_ON_LBUTTON_STATE)) )
			{
				srgui_data.mouse_l_down.window->setDirty();
			}
			break;
		case 2:
			srgui_data.mouse_m_down.clear();
			break;
		case 3:
			srgui_data.mouse_r_down.clear();
			break;
		}

		if( srControl* c = srgui_data.mouse_l_down.child; c )
		{
			srIEvent* ev = dynamic_cast<srIEvent*>(c);	
			if( ev ) ev->raiseMouseUpEvent({get_control_relative_point(srgui_data.mouse_l_down.window, c, {x,y}), 0,0,1});
		}

		srgui_data.mouse_l_down.clear();
		return;
	} //end of mouse up

	if( event == SR_EVENT_MOUSE_MOVE )
	{
		int x = data0;
		int y = data1;
		int relx = data2;
		int rely = data3;
		srgui_data.mouse_pos = {x,y};
		srInputTracker old = srgui_data.mouse_over;
		srgui_data.mouse_over.child = nullptr;

		if( srgui_data.caption_move )
		{
			if( ! srgui_data.windows[0] )
			{
				std::puts("how did this happen!?\nsrgui.cpp: line 140");
				std::exit(1);
			}
			srRect r;
			srgui_data.windows[0]->getArea(r);
			r.x += relx;
			r.y += rely;
			srgui_data.windows[0]->setArea(r);
			return;
		}

		if(srIOverlay* tmp; srgui_data.windows[0] && srgui_data.windows[0]->overlay
				 && (tmp = srgui_data.windows[0]->isPointInOverlay({x,y})) )
		{
			// Mouse pointer is over the top window's overlay
			// can set window and child to window and overlay control.
			// Can't return early, something else might require redraw
			// because of no longer hovering.
			srgui_data.mouse_over.window = srgui_data.windows[0];
			srgui_data.mouse_over.child = dynamic_cast<srControl*>(tmp);
			if( srgui_data.mouse_over.child->flags & SR_CF_REPAINT_ON_HOVER )
			{
				tmp->setDirty();
				//srIOverlay* ov = dynamic_cast<srIOverlay*>(old.child);
				//if( ov ) ov->setDirty();
			}
		} else {
			srWindow* W = findWindowUnderPoint({x,y});
			
			if( W ) srgui_data.mouse_over.child = findControlUnderPoint(W, {x,y});
			
			srgui_data.mouse_over.window = W;
		}

		if( old && old.child && (old.child->getFlags() & SR_CF_REPAINT_ON_HOVER) )
		{
			if( old.child == old.window->overlay )
			{    //TODO: ^ need a isPartOfOverlay function, this will only find first overlay(menu).
				srIOverlay* iov = dynamic_cast<srIOverlay*>(old.child);
				iov->setDirty();
			} else {
				old.window->setDirty();
			}
		}

		if( srControl* c = srgui_data.mouse_l_down.child;  c )
		{
			if( c->flags & SR_CF_REPAINT_ON_MOUSE_MOVE ) srgui_data.mouse_l_down.window->setDirty();
			
			srIEvent* ev = dynamic_cast<srIEvent*>(c);	
			if( ev ) ev->raiseMouseMoveEvent({{relx,rely}, 0,0,1});
		}
	} //end of mouse move



	return;
}

srWindow* findWindowUnderPoint(const srPoint& p)
{
	srWindow* retval = nullptr;

	for(uint32_t i = 0; i < srgui_data.windows.size(); ++i)
	{
		srWindow* temp = srgui_data.windows[i];
		srRect r;
		temp->getArea(r);
		if( ! point_in_rect(r, {p.x,p.y}) ) continue;

		retval = temp;
		break;
	}

	return retval;
}

srControl* findControlUnderPoint(srWindow* W, const srPoint& p)
{
	if( ! W ) return nullptr;

	srControl* retval = nullptr;

	for(uint32_t i = 0; i < W->getNumChildren(); ++i)
	{
		if( ! W->point_in_child(i, p) ) continue;
	
		srControl* C1 = W->getChild(i);

		if( !C1->getVisible() || !C1->getEnabled() ) continue;

		retval = C1;

		srContainer* con = dynamic_cast<srContainer*>(retval);
		if( con )
		{
			srRect r, wr;
			C1->getArea(r);
			W->getArea(wr);
			srControl* temp = find_in_container(con, { (p.x - wr.x) - r.x, (p.y - wr.y) - r.y });
			if( temp && temp->getVisible() && temp->getEnabled() ) retval = temp;
		}

		if( retval->getFlags() & SR_CF_REPAINT_ON_HOVER )
		{
			W->setDirty();
		}
		break;
	}

	return retval;
}

void generateDrawList(std::vector<srRenderTask>& tasks)
{
	tasks.reserve(srgui_data.windows.size());

	for(auto i = srgui_data.windows.rbegin(); i != srgui_data.windows.rend(); ++i)
	{
		srWindow* win = *i;

		srControl* fc = win->getChildFocus();
		
		if( win->getDirty() || (fc && (fc->flags & SR_CF_REPAINT_WHILE_FOCUS)) )
		{
			win->draw(srDrawInfo{win->surface, (srDrawInfoFlags) 0, 
				{srgui_data.mouse_pos.x-win->area.x, srgui_data.mouse_pos.y-win->area.y}, {0,0}});
		}

		tasks.emplace_back(win->surface, win->area, win->getDirty());
		win->isdirty = false;

		srIOverlay* ovly = dynamic_cast<srIOverlay*>(win->overlay);
		if( !ovly ) continue;
		
		srRect oca;
		while( ovly )
		{		
			oca = ovly->getArea();

			if( ovly->getDirty() )
			{
				uint32_t f = 0;
				if( point_in_rect( oca, srgui_data.mouse_pos ) ) f |= SR_DIF_MOUSE_OVER;
				dynamic_cast<srControl*>(ovly)->draw(srDrawInfo{ovly->getSurface(), (srDrawInfoFlags) f, 
						{srgui_data.mouse_pos.x-oca.x, srgui_data.mouse_pos.y-oca.y}, {0,0}});
				ovly->clearDirty();
			}
			tasks.emplace_back(ovly->getSurface(), oca, ovly->getDirty());
			ovly = ovly->getSubOverlay();
		}
	}

	return;
}

void setTextFocusCallback(const std::function<void(bool)>& cb)
{
	srgui_data.text_focus_callback = cb;
	return;
}


}; //end of namespace srgui


