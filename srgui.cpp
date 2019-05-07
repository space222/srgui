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

struct {
	std::vector<srWindow*> windows;
	srInputTracker mouse_over;
	srInputTracker mouse_l_down;
	srInputTracker mouse_r_down;
	srInputTracker mouse_m_down;
	srInputTracker mouse_l_up;
	srInputTracker mouse_r_up;
	srInputTracker mouse_m_up;

	bool caption_move;

	srPoint mouse_pos;
	srLookAndFeel UIStyle;
} srgui_data;

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

	laf.buttonBackground = 0xc0c0be;
	laf.buttonRounding = 0;
	laf.buttonOutline = 0x3030A0;

	laf.textBackground = 0xF0F0F0;
	return;
}

bool point_in_rect(const srRect& r, const srPoint& p)
{
	return (p.x >= r.x && p.x < (r.x+r.width) &&
		p.y >= r.y && p.y < (r.y+r.height) );
}

void SendEvent(srEventType event, int data0, int data1, int data2, int data3)
{
	if( event == SR_EVENT_MOUSE_DOWN )
	{
		int button = data0;
		int x = data1;
		int y = data2;

		// if we're not even over a window, we can ditch early
		if( ! srgui_data.mouse_over ) return;

		// if we were over a window that isn't already top
		// find it and bring it up
		if( srgui_data.mouse_over && srgui_data.mouse_over.window != srgui_data.windows[0] )
		{
			for(int i = 0; i < srgui_data.windows.size(); ++i)
			{
				if( srgui_data.mouse_over.window == srgui_data.windows[i] )
				{
					for(int j = i; j > 0; j--) srgui_data.windows[j] = srgui_data.windows[j-1];
					srgui_data.windows[0] = srgui_data.mouse_over.window;
					break;
				}
			}
		}

		if( button == 1 )
		{    // we can only end up clicking on the top level window
			srgui_data.mouse_l_down.window = srgui_data.windows[0];
			srRect r, c;
			srgui_data.windows[0]->getArea(r);
			c = r;
			c.height = srgui_data.UIStyle.windowCaptionHeight;
			if( srgui_data.caption_move = point_in_rect( c, {x,y} ) ) return;
		}

		switch( button )
		{
		case 1: srgui_data.mouse_l_down = srgui_data.mouse_over; break;
		case 2: srgui_data.mouse_m_down = srgui_data.mouse_over; break;
		case 3: srgui_data.mouse_r_down = srgui_data.mouse_over; break;
		}

		if( srgui_data.mouse_l_down && srgui_data.mouse_l_down.child 
				&& (srgui_data.mouse_l_down.child->getFlags() & SR_CF_REPAINT_ON_LBUTTON_STATE) )
		{
			srgui_data.mouse_over.window->setDirty();
		}

		return;
	}

	if( event == SR_EVENT_MOUSE_MOVE )
	{
		int x = data0;
		int y = data1;
		int relx = data2;
		int rely = data3;

		if( srgui_data.caption_move )
		{
			srRect r;
			srgui_data.windows[0]->getArea(r);
			r.x += relx;
			r.y += rely;
			srgui_data.windows[0]->setArea(r);
			return;
		}

		srWindow* W = nullptr;
		for(int i = 0; i < srgui_data.windows.size(); ++i)
		{
			srWindow* temp = srgui_data.windows[i];
			srRect r;
			temp->getArea(r);
			if( ! point_in_rect(r, {x,y}) ) continue;

			W = temp;
			break;
		}

		srInputTracker old = srgui_data.mouse_over;
		srgui_data.mouse_over.child = nullptr;

		if( W )
		for(int i = 0; i < W->getNumChildren(); ++i)
		{
			if( ! W->point_in_child(i, {x,y} ) ) continue;
		
			srgui_data.mouse_over.child = W->getChild(i);
			if( srgui_data.mouse_over.child->getFlags() & SR_CF_REPAINT_ON_HOVER )
			{
				W->setDirty();
			}
			break;
		}

		srgui_data.mouse_over.window = W;

		if( old && old.child && (old.child->getFlags() & SR_CF_REPAINT_ON_HOVER) )
		{
			old.window->setDirty();
		}
	} //end of mouse move

	if( event == SR_EVENT_MOUSE_UP )
	{
		int button = data0;
		int x = data1;
		int y = data2;
		srgui_data.caption_move = false;

		switch( button )
		{
		case 1:
			if( srgui_data.mouse_l_down.child && srgui_data.mouse_l_down == srgui_data.mouse_over )
			{
				srIEvent* c = dynamic_cast<srIEvent*>(srgui_data.mouse_l_down.child);
				if( c ) c->raiseClickEvent();
				//else puts("It wasn't clickable!");
			}
			if( srgui_data.mouse_l_down && srgui_data.mouse_l_down.child 
				&& (srgui_data.mouse_l_down.child->getFlags() & (SR_CF_REPAINT_ON_LEFT_CLICK|SR_CF_REPAINT_ON_LBUTTON_STATE)) )
			{
				srgui_data.mouse_l_down.window->setDirty();
			}
			srgui_data.mouse_l_down.clear();
			break;
		case 2:
			srgui_data.mouse_m_down.clear();
			break;
		case 3:
			srgui_data.mouse_r_down.clear();
			break;
		}
	}

	return;
}

srWindow* CreateWindow(const srRect& extents, void* userData, const std::string& title)
{
	srWindow* win = new srWindow(extents.width, extents.height, new srCairoDrawSurface(extents.width, extents.height));
	win->setArea(extents);
	win->surface->setUserData(userData);
	win->title = title;
	srgui_data.windows.push_back(win);
	return win;
}

srWindow::srWindow(int w, int h, srDrawSurface* draw)
{
	area.x = area.y = 0;
	area.width = w;
	area.height = h;
	parent = nullptr;
	surface = draw;
	caption_layout = nullptr;
}

void srButton::setArea(const srRect& r)
{
	area = r;
	srControl* c = parent;
	if( c )
	{
		while( c->getParent() ) c = c->getParent();
		((srWindow*)c)->setDirty();
	}
	return;
}

void srButton::setText(const std::string& s)
{
	text = s;
	if( ! text_layout ) 
	{
		text_layout = new srPangoTextLayout;
		//text_layout->setFont("Sans 15");
	}
	text_layout->setText(text);
	return;
}

void srButton::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;

	surf->setColor(srgui_data.UIStyle.buttonBackground);
	surf->drawRoundRectangle(area, srgui_data.UIStyle.buttonRounding);

	uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
					srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	if( srgui_data.UIStyle.buttonRounding )
	{
		if( info.flags & SR_DIF_MOUSE_LEFT ) {
			surf->setColor(outlineColor + 0x222222);
		} else {
			surf->setColor(outlineColor);
		}
		surf->outlineRoundRectangle(area, srgui_data.UIStyle.buttonRounding);
	} else {
		if( info.flags & SR_DIF_MOUSE_LEFT ) {
			surf->setColor(outlineColor - 0x222222);
			surf->drawLine(area.x, area.y, area.x + area.width, area.y);
			surf->drawLine(area.x, area.y, area.x, area.y+area.height);
			surf->setColor(outlineColor + 0x222222);
			surf->drawLine(area.x, area.y+area.height, area.x+area.width, area.y+area.height);
			surf->drawLine(area.x+area.width, area.y, area.x+area.width, area.y+area.height);
		} else if( info.flags & SR_DIF_MOUSE_OVER ) {
			surf->setColor(outlineColor + 0x222222);
			surf->drawLine(area.x, area.y, area.x + area.width, area.y);
			surf->drawLine(area.x, area.y, area.x, area.y+area.height);
			surf->setColor(outlineColor - 0x222222);
			surf->drawLine(area.x, area.y+area.height, area.x+area.width, area.y+area.height);
			surf->drawLine(area.x+area.width, area.y, area.x+area.width, area.y+area.height);
		} 
	}

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(area.x+r.width/2), (int)(area.y)}, text_layout);

	return;
}

void srCheckbox::setArea(const srRect& r)
{
	area = r;
	srControl* c = parent;
	if( c )
	{
		while( c->getParent() ) c = c->getParent();
		((srWindow*)c)->setDirty();
	}
	return;
}

void srCheckbox::setText(const std::string& s)
{
	text = s;
	if( ! text_layout ) 
	{
		text_layout = new srPangoTextLayout;
		//text_layout->setFont("Sans 15");
	}
	text_layout->setText(text);
	return;
}

void srCheckbox::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;

	uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
					srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	surf->setColor(srgui_data.UIStyle.textBackground);
	surf->drawRectangle({area.x, area.y, 15, 15});

	if( isChecked )
	{
		surf->setColor(0);
		surf->drawLine(area.x+11, area.y+2, area.x+8, area.y+13);
		surf->drawLine(area.x+8, area.y+13, area.x+3, area.y+7);
	}

	surf->setColor(srgui_data.UIStyle.buttonBackground);

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(area.x+20), (int)(area.y)}, text_layout);

	return;
}

void srLabel::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(srgui_data.UIStyle.textColor);
	surf->drawTextLayout({(int)(area.x+20), (int)(area.y)}, text_layout);

	return;
}

void srLabel::setText(const std::string& s)
{
	text = s;
	if( ! text_layout ) 
	{
		text_layout = new srPangoTextLayout;
		//text_layout->setFont("Sans 15");
	}
	text_layout->setText(text);
	return;
}

void srLabel::setArea(const srRect& r)
{
	area = r;
	srControl* c = parent;
	if( c )
	{
		while( c->getParent() ) c = c->getParent();
		((srWindow*)c)->setDirty();
	}
	return;
}

void srWindow::draw(const srDrawInfo& info)
{
	surface->setColor(srgui_data.UIStyle.windowBackground);

	if( srgui_data.UIStyle.windowStyle & SR_SF_WINDOW_ROUND_BOTTOM )
	{
		surface->drawRoundRectangle({0, 0, area.width, area.height}, srgui_data.UIStyle.windowCaptionRounding);
	} else {
		surface->drawTopRoundedRect({0, 0, area.width, area.height}, srgui_data.UIStyle.windowCaptionRounding);
	}

	if( title.size() > 0 )
	{
		if( srgui_data.UIStyle.windowCaptionColor1 != srgui_data.UIStyle.windowCaptionColor2 )
		{
			surface->setLinearGradient(srgui_data.UIStyle.windowCaptionColor1, srgui_data.UIStyle.windowCaptionColor2);
		} else {
			surface->setColor(srgui_data.UIStyle.windowCaptionColor1);
		}
	
		surface->drawTopRoundedRect({srgui_data.UIStyle.windowCaptionOffset.x, srgui_data.UIStyle.windowCaptionOffset.y,
					area.width - srgui_data.UIStyle.windowCaptionOffset.x, srgui_data.UIStyle.windowCaptionHeight},
						srgui_data.UIStyle.windowCaptionRounding);
		
		if( ! caption_layout ) caption_layout = new srPangoTextLayout;
		caption_layout->setText(title);
		surface->setColor(srgui_data.UIStyle.windowCaptionTextColor);
		int a = srgui_data.UIStyle.windowCaptionRounding;
		surface->drawTextLayout(srgui_data.UIStyle.windowCaptionOffset + srPoint{a,0} , caption_layout);
	}

	
	for(int i = 0; i < children.size(); ++i)
	{
		srControl* c = children[i];
		srDrawInfo di;
		di.surface = surface;
		di.flags = (int)0;
		if( srgui_data.mouse_over && srgui_data.mouse_over.child == c ) { di.flags |= SR_DIF_MOUSE_OVER; }
		if( srgui_data.mouse_l_down && srgui_data.mouse_l_down.child == c ) di.flags |= SR_DIF_MOUSE_LEFT;
		if( srgui_data.mouse_m_down && srgui_data.mouse_m_down.child == c ) di.flags |= SR_DIF_MOUSE_MIDDLE;
		if( srgui_data.mouse_r_down && srgui_data.mouse_r_down.child == c ) di.flags |= SR_DIF_MOUSE_RIGHT;
		c->draw(di);
	}

	isdirty = false;
	return;
}

void generateDrawList(std::vector<srRenderTask>& tasks)
{
	tasks.reserve(srgui_data.windows.size());

	for(auto i = srgui_data.windows.rbegin(); i != srgui_data.windows.rend(); ++i)
	{
		srWindow* win = *i;

		/* TODO: open menus, comboboxes, etc */

		if( win->getDirty() )
		{
			win->draw(srDrawInfo{win->surface, (srDrawInfoFlags) 0, 
				{srgui_data.mouse_pos.x-win->area.x, srgui_data.mouse_pos.y-win->area.y}});
		}

		tasks.emplace_back(win->surface, win->area, win->getDirty());
		win->isdirty = false;
	}

	return;
}


}; //end of namespace srgui


