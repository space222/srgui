#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srMenuBar.h"
#include "srWindow.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srMenu::draw(const srDrawInfo& info)
{
	if( !surface ) resize();
	
	surface->setColor(srgui_data.UIStyle.windowBackground);
	surface->drawRectangle({ 0, 0, area.width, area.height });

	for(uint32_t i = 0; i < items.size(); ++i)
	{
		if( items[i].text.size() && (info.flags & SR_DIF_MOUSE_OVER) 
					 && info.mouse_rel.y > (i*20) && info.mouse_rel.y < ((i+1)*20) )
		{
			surface->setColor(srgui_data.UIStyle.itemSelectionColor);
		} else {
			surface->setColor(srgui_data.UIStyle.windowBackground);
		}

		surface->drawRectangle({ 0, (int) i*20, area.width, 20 });

		if( items[i].text.size() )
		{
			text_layout->setText(items[i].text);
			surface->setColor(0);
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
		int a = mi.text.size();
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
	
	for(uint32_t i = 0; i < menu_offsets.size()-1; ++i)
	{
		if( mx < menu_offsets[i+1] )
		{
			srWindow* win = dynamic_cast<srWindow*>(getToplevelParent());
			win->overlay = dynamic_cast<srControl*>(menus[i]);
			break;
		}
	}

	return;
}

void srMenuBar::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };
	int menu_offs = 0;

	menu_offsets.clear();

	for(srMenu* M : menus)
	{
		const std::string& txt = M->getText();
		text_layout->setText(txt);
		srRect txtsize;
		text_layout->getExtents(txtsize);

		if( (info.flags & SR_DIF_MOUSE_OVER) && info.mouse_rel.x > menu_offs && info.mouse_rel.x < (menu_offs + txtsize.width) )
		{
			surf->setColor( srgui_data.UIStyle.itemSelectionColor );
		} else {
			surf->setColor( srgui_data.UIStyle.windowBackground );
		}

		surf->drawRectangle({ menu_offs, origin.y, txtsize.width, area.height });
		surf->setColor(0);
		surf->drawTextLayout({ menu_offs, origin.y }, text_layout);

		menu_offsets.push_back(menu_offs);
		menu_offs += txtsize.width;
	}

	surf->setColor(0);
	surf->drawLine( origin.x, area.height-1, origin.x+area.width, area.height-1 );

	return;
}

} //end of srgui namespace


