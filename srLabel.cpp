#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srLabel.h"

namespace srgui {

extern srgui::system_data srgui_data;

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


} //end of srgui namespace


