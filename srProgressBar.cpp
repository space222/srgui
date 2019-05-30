#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"
#include "srWindow.h"
#include "srProgressBar.h"

namespace srgui {

extern srgui::system_data srgui_data;

void srProgressBar::setProgress(int p)
{
	progress = p;

	if( progress > 100 ) progress = 100;
	else if( progress < 0 ) progress = 0;

	//setParentDirty();

	return;
}

void srProgressBar::draw(const srDrawInfo& info)
{
	srDrawSurface* surf = info.surface;
	srPoint origin = srPoint{area.x,area.y} + info.parent_offset;
	srRect na{ origin.x, origin.y, area.width, area.height };

	surf->setColor(srgui_data.UIStyle.windowBackground);
	surf->drawRectangle(na);	
	surf->setColor(0);
	surf->drawLine(origin.x, origin.y, origin.x+na.width, origin.y, 1);
	surf->drawLine(origin.x, origin.y, origin.x, origin.y+na.height, 1);
	surf->setColor(0xffffff);
	surf->drawLine(origin.x, origin.y+na.height, origin.x+na.width, origin.y+na.height, 1);
	surf->drawLine(origin.x+na.width, origin.y, origin.x+na.width, origin.y+na.height, 1);

	surf->setColor(0xA8); //srgui_data.UIStyle.textSelection
	surf->drawRectangle({origin.x+3, origin.y+3,(uint32_t) ((area.width*(progress/100.0f))-3), area.height-6});

	//srRect r;
	//text_layout->getExtents(r);
	//surf->setColor(srgui_data.UIStyle.textColor);
	//surf->drawTextLayout(origin, text_layout);

	return;
}

void srProgressBar::setText(const std::string& s)
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

} //end of srgui namespace


