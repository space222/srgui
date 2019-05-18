#include <vector>
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srgui.h"
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"

namespace srgui {

extern srgui::system_data srgui_data;

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

void srWindow::setTitle(const std::string& t)
{
	title = t;
	if( ! caption_layout )
	{
		caption_layout = new srPangoTextLayout;
		caption_layout->setText(title);
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
		
		if( ! caption_layout )
		{
			caption_layout = new srPangoTextLayout;
			caption_layout->setText(title);
		}
		surface->setColor(srgui_data.UIStyle.windowCaptionTextColor);
		int a = srgui_data.UIStyle.windowCaptionRounding;
		srRect r1;
		caption_layout->getExtents(r1);
		int b = srgui_data.UIStyle.windowCaptionHeight/2 - r1.height/2;
		surface->drawTextLayout(srgui_data.UIStyle.windowCaptionOffset + srPoint{a+3,b}, caption_layout);
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


} //end of srgui namespace


