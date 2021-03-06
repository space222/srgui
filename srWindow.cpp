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
	focus = nullptr;
	overlay = nullptr;
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
		caption_layout->setFont("Sans 13");
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
					area.width - srgui_data.UIStyle.windowCaptionOffset.x, (int)srgui_data.UIStyle.windowCaptionHeight},
						srgui_data.UIStyle.windowCaptionRounding);
		
		if( ! caption_layout )
		{
			setTitle(title);
		}
		surface->setColor(srgui_data.UIStyle.windowCaptionTextColor);
		int a = srgui_data.UIStyle.windowCaptionRounding;
		srRect r1;
		caption_layout->getExtents(r1);
		int b = srgui_data.UIStyle.windowCaptionHeight/2 - r1.height/2;
		surface->drawTextLayout(srgui_data.UIStyle.windowCaptionOffset + srPoint{a+3,b}, caption_layout);
	}

	for(uint32_t i = 0; i < children.size(); ++i)
	{
		srControl* c = children[i];
		if( ! c->visible ) continue;

		srDrawInfo di;
		di.surface = surface;
		di.flags = (int)0;
		di.parent_offset = {0,0};
		di.mouse_rel = srgui_data.mouse_pos - srPoint{area.x, area.y};
		if( !isPointInOverlay( srgui_data.mouse_pos ) )
		{
			if( (srgui_data.mouse_over && srgui_data.mouse_over.child == c) 
				|| point_in_child(i, srgui_data.mouse_pos) ) { di.flags |= SR_DIF_MOUSE_OVER; }
			if( srgui_data.mouse_l_down && 
			(srgui_data.mouse_l_down.child == c || point_in_child(i, srgui_data.mouse_pos)) ) di.flags |= SR_DIF_MOUSE_LEFT;
			if( srgui_data.mouse_m_down && 
			(srgui_data.mouse_m_down.child == c || point_in_child(i, srgui_data.mouse_pos)) ) di.flags |= SR_DIF_MOUSE_MIDDLE;
			if( srgui_data.mouse_r_down && 
			(srgui_data.mouse_r_down.child == c || point_in_child(i, srgui_data.mouse_pos)) ) di.flags |= SR_DIF_MOUSE_RIGHT;
			if( c == focus ) di.flags |= SR_DIF_FOCUS;
		}
		srRect r;
		c->getArea(r);
		surface->clip({r.x -2, r.y -2, r.width +3, r.height +3});
		c->draw(di);

		srContainer* contr = dynamic_cast<srContainer*>(c);
		if( contr )
		{
			draw_container(this, c, di);
		}
		
		surface->reset_clip();
	}

	isdirty = false;
	return;
}

void srWindow::draw_container(srWindow* win, srControl* contr, const srDrawInfo& info)
{
	srContainer* tainer = dynamic_cast<srContainer*>(contr);

	for(uint32_t i = 0; i < tainer->getNumChildren(); ++i)
	{
		srControl* c = tainer->getChild(i);
		if( ! c->visible ) continue;

		srRect area;
		contr->getArea(area);
		
		srDrawInfo di;
		di.surface = info.surface;
		di.flags = (int)0;
		di.parent_offset = srPoint{area.x, area.y} + info.parent_offset;
		di.mouse_rel = di.mouse_rel - srPoint{area.x, area.y};

		if( !isPointInOverlay( srgui_data.mouse_pos ) )
		{
			if( (srgui_data.mouse_over && srgui_data.mouse_over.child == c) 
				|| tainer->point_in_child(i, srgui_data.mouse_pos) ) { di.flags |= SR_DIF_MOUSE_OVER; }

			if( srgui_data.mouse_l_down && 
			(srgui_data.mouse_l_down.child == c || tainer->point_in_child(i, srgui_data.mouse_pos)) ) 
				di.flags |= SR_DIF_MOUSE_LEFT;

			if( srgui_data.mouse_m_down && 
			(srgui_data.mouse_m_down.child == c || tainer->point_in_child(i, srgui_data.mouse_pos)) ) 
				di.flags |= SR_DIF_MOUSE_MIDDLE;

			if( srgui_data.mouse_r_down && 
			(srgui_data.mouse_r_down.child == c || tainer->point_in_child(i, srgui_data.mouse_pos)) ) 
				di.flags |= SR_DIF_MOUSE_RIGHT;

			if( c == win->getChildFocus() )
				 di.flags |= SR_DIF_FOCUS;
		}
		
		srRect r;

		c->getArea(r);
		surface->clip({di.parent_offset.x + r.x - 2, di.parent_offset.y + r.y - 2, r.width + 3, r.height + 3});
		c->draw(di);

		srContainer* contr2 = dynamic_cast<srContainer*>(c);
		if( contr2 )
		{
			draw_container(win, c, di);
		}
		
		surface->reset_clip();
	}

	return;
}

srIOverlay* srWindow::isPointInOverlay(const srPoint& p)
{
	if( ! overlay ) return nullptr;
	srIOverlay* ovl = dynamic_cast<srIOverlay*>(overlay);
	
	do {
		if( point_in_rect( ovl->getArea(), p ) ) return ovl;
		ovl = ovl->getSubOverlay();
	} while( ovl );

	return nullptr;
}

srWindow::~srWindow()
{
	delete caption_layout;
	delete surface;	

	for(srControl* c : children)
	{
		delete c;
	}

	return;
}


} //end of srgui namespace


