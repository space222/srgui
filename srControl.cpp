#include <cstdint>
#include "srgui.h"
#include "srWindow.h"
#include "srControl.h"

namespace srgui {

srControl* srControl::getToplevelParent()
{
	if( ! parent ) return nullptr;
	srControl* c = parent;
	while( c->getParent() ) c = c->getParent();
	return c;
}

void srControl::setVisible(bool b)
{
	if( visible == b ) return;
	visible = b;
	srWindow* W = dynamic_cast<srWindow*>(getToplevelParent());
	if( W ) W->setDirty();
	return;
}

void srControl::setEnabled(bool b)
{
	if( enabled == b ) return;
	enabled = b;
	srWindow* W = dynamic_cast<srWindow*>(getToplevelParent());
	if( W ) W->setDirty();
	return;
}

void srControl::setArea(const srRect& r) 
{
	area = r;
	srWindow* w = dynamic_cast<srWindow*>(getToplevelParent());
	if(w) w->setDirty();
	return;
}

void srControl::setSize(int width, int height)
{
	area.width = width;
	area.height = height;
	srWindow* w = dynamic_cast<srWindow*>(getToplevelParent());
	if(w) w->setDirty();
	return;
}

} //end of srgui namespace


