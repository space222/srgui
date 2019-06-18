#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srMenuBar.h"
#include "srWindow.h"

namespace srgui {

extern srgui::system_data srgui_data;

srMenu::srMenu(const std::string& name, std::initializer_list<srMenuItem> initlist) 
		: text(name), items(initlist), dirty(true), surface(nullptr), text_layout(nullptr)
{
	flags = SR_CF_REPAINT_ON_HOVER | SR_CF_REPAINT_ON_LBUTTON_STATE;
	resize();
	return;
}

void srMenu::raiseClickEvent(const srEventInfo& event)
{
	int i = event.mouse.y / 20;
	if( items[i].text.size() && items[i].onClick )
	{
		items[i].onClick();
		dirty = true;
		//srgui_data.windows[0]->overlay = nullptr;
	}
	return;
}

void srMenu::draw(const srDrawInfo& info)
{
	if( !surface ) resize();
	
	surface->setColor(srgui_data.UIStyle.windowBackground);
	surface->drawRectangle({ 0, 0, area.width, area.height });

	for(uint32_t i = 0; i < items.size(); ++i)
	{
		bool selectd = false;
		if( items[i].text.size() && (info.flags & SR_DIF_MOUSE_OVER) 
					 && info.mouse_rel.y > (i*20) && info.mouse_rel.y < ((i+1)*20) )
		{
			selectd = true;
			surface->setColor(srgui_data.UIStyle.itemSelectionColor);
		} else {
			surface->setColor(srgui_data.UIStyle.windowBackground);
		}

		surface->drawRectangle({ 0, (int) i*20, area.width, 20 });

		if( items[i].text.size() )
		{
			text_layout->setText(items[i].text);
			surface->setColor( (selectd ? 0xffffff : 0) );
			surface->drawTextLayout({ 5, (int) i*20 }, text_layout);
		} else { // separator will be indicated by a text-less item
			surface->setColor(0x808080);
			surface->drawLine( 5, (int) (i*20 + 10), 5 + (area.width - 10), (int) (i*20+10), 1);
		}
	}

	//todo: like anything else, make it look nicer once functionality... functions.

	return;
}

void srMenu::resize()
{
	if( ! text_layout ) text_layout = new srPangoTextLayout;

	int longest = 0;
	int longest_index = 0;
	for(uint32_t i = 0; i < items.size(); ++i)
	{
		srMenuItem& mi = items[i];
		int a = mi.text.size();		// this should use text-extents, but for now good enough
		if( a > longest ) 
		{
			longest = a;
			longest_index = i;
		}
	}

	text_layout->setText(items[longest_index].text);
	srRect tr;
	text_layout->getExtents(tr);

	int width = tr.width + 10;
	int height = tr.height * items.size();
	area.width = width;
	area.height = height;

	if( !surface )
	{
		surface =(srDrawSurface*) new srCairoDrawSurface(width, height);
	} else {
		surface->setSize(width, height);
	}

	return;
}

void srMenuBar::raiseClickEvent(const srEventInfo& event)
{
	int mx = event.mouse.x;
	if( menu_offsets.size() == 0 || mx > menu_offsets[menu_offsets.size()-1] ) return;
	
	for(uint32_t i = 0; i < menu_offsets.size(); ++i)
	{
		if( mx < menu_offsets[i] )
		{
			srRect winr;
			srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());
			win->getArea(winr);
			win->overlay = dynamic_cast<srControl*>(menus[i]);
			menus[i]->area.x = winr.x + ((i == 0) ? 0 : menu_offsets[i-1]);
			menus[i]->area.y = winr.y + area.y + area.height;
			break;
		}
	}

	return;
}

void srMenuBar::setArea(const srRect& r)
{
	srRect a = r;
	a.y += srgui_data.UIStyle.windowCaptionHeight;
	area = a;
	return;
}

void srMenuBar::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };
	int menu_offs = 0;

	surf->setColor(srgui_data.UIStyle.windowBackground);
	surf->drawRectangle(newarea);

	menu_offsets.clear();

	srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());

	for(srMenu* M : menus)
	{
		const std::string& txt = M->getText();
		text_layout->setText(txt);
		srRect txtsize;
		text_layout->getExtents(txtsize);

		if( (info.flags & SR_DIF_MOUSE_OVER) && info.mouse_rel.x > menu_offs && info.mouse_rel.x < (menu_offs + txtsize.width + 20) )
		{
			if( win->overlay )
			{
				// not great to do this here, but not feeling
				// like adding a hover event just yet.
				srIOverlay* ovl = dynamic_cast<srIOverlay*>(win->overlay);
				ovl->setDirty();
				win->overlay = M;
				M->setDirty();
				srRect wr;
				win->getArea(wr);
				M->area.x = wr.x + menu_offs;
				M->area.y = wr.y + area.y + area.height;
			}
		} 

		if( win->overlay == M )
		{
			surf->setColor(0xffffff);
			surf->drawLine( menu_offs, origin.y+area.height, menu_offs+txtsize.width+20, origin.y+area.height, 1);
			surf->drawLine( menu_offs+txtsize.width+20, origin.y, menu_offs+txtsize.width+20, origin.y+area.height, 1);
			surf->setColor(0);
			surf->drawLine( menu_offs, origin.y, menu_offs+txtsize.width+20, origin.y, 1 );
			surf->drawLine( menu_offs, origin.y, menu_offs, origin.y+area.height, 1 );
			surf->drawTextLayout({ menu_offs + 12, origin.y + 2 }, text_layout);
		} else {
			surf->setColor(0);
			surf->drawTextLayout({ menu_offs + 10, origin.y }, text_layout);
		}
		
		//surf->drawRectangle({ menu_offs, origin.y, txtsize.width + 20, area.height });
		//surf->setColor(0);
		
		menu_offs += txtsize.width + 20;
		menu_offsets.push_back(menu_offs);
	}

	surf->setColor(0);
	surf->drawLine( origin.x, area.height-1, origin.x+area.width, area.height-1 );

	return;
}

srMenuBar::~srMenuBar()
{
	delete text_layout;
	for(srMenu* M : menus)
	{
		delete M;
	}
	return;
}


} //end of srgui namespace


