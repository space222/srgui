#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srgui.h"
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srButton.h"


namespace srgui {

extern srgui::system_data srgui_data;

srButton::srButton(const std::string& s, const srPoint& p, int label_margin) : srButton()
{
	srRect te;

	setText(s);

	text_layout->getExtents(te);

	if( label_margin < 0 )
	{
		// todo: part of UIStyle
		label_margin = 25;
	}

	area = srRect{ p.x, p.y, te.width+label_margin, te.height+label_margin };

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
	srPoint origin = srPoint{area.x, area.y} + info.parent_offset;
	srRect newarea{ origin.x, origin.y, area.width, area.height };

	surf->setColor(srgui_data.UIStyle.buttonBackground);
	surf->drawRoundRectangle(newarea, srgui_data.UIStyle.buttonRounding);

	uint32_t outlineColor = (srgui_data.UIStyle.buttonStyle & SR_SF_OUTLINE) ? 
					srgui_data.UIStyle.buttonOutline : srgui_data.UIStyle.buttonBackground;

	if( srgui_data.UIStyle.buttonRounding )
	{
		if( info.flags & SR_DIF_MOUSE_LEFT ) {
			surf->setColor(outlineColor + 0x222222);
		} else {
			surf->setColor(outlineColor);
		}
		surf->outlineRoundRectangle(newarea, srgui_data.UIStyle.buttonRounding);
	} else {
		if( info.flags & SR_DIF_MOUSE_LEFT ) {
			surf->setColor(outlineColor - 0x222222);
			surf->drawLine(newarea.x, newarea.y, newarea.x + newarea.width, newarea.y);
			surf->drawLine(newarea.x, newarea.y, newarea.x, newarea.y+newarea.height);
			surf->setColor(outlineColor + 0x222222);
			surf->drawLine(newarea.x, newarea.y+newarea.height, newarea.x+newarea.width, newarea.y+newarea.height);
			surf->drawLine(newarea.x+newarea.width, newarea.y, newarea.x+newarea.width, newarea.y+newarea.height);
		} else /* if( info.flags & SR_DIF_MOUSE_OVER ) */ {
			surf->setColor(outlineColor + 0x222222);
			surf->drawLine(newarea.x, newarea.y, newarea.x + newarea.width, newarea.y);
			surf->drawLine(newarea.x, newarea.y, newarea.x, newarea.y+newarea.height);
			surf->setColor(outlineColor - 0x222222);
			surf->drawLine(newarea.x, newarea.y+newarea.height, newarea.x+newarea.width, newarea.y+newarea.height);
			surf->drawLine(newarea.x+newarea.width, newarea.y, newarea.x+newarea.width, newarea.y+newarea.height);
		} 
	}

	srRect r;
	text_layout->getExtents(r);
	surf->setColor(0);
	surf->drawTextLayout({(int)(newarea.x+newarea.width/2-r.width/2), (int)(newarea.y+newarea.height/2-r.height/2)}, text_layout);

	return;
}

} //end of srgui namespace


