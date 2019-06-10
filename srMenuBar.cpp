#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srMenuBar.h"
#include "srWindow.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srMenuBar::raiseClickEvent(const srEventInfo& event)
{
	int mx = event.mouse.x;
	if( menu_offsets.size() == 0 || mx > menu_offsets[menu_offsets.size()-1] ) return;
	
	for(uint32_t i = 0; i < menu_offsets.size()-1; ++i)
	{
		if( mx < menu_offsets[i+1] )
		{
			//todo: set menu i as overlay control for parent window
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


