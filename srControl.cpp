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

void srControl::setArea(const srRect& r) 
{
	area = r;
	srWindow* w = dynamic_cast<srWindow*>(getToplevelParent());
	if(w) w->setDirty();
	return;
}

} //end of srgui namespace


