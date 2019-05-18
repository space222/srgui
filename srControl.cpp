#include <cstdint>
#include "srgui.h"
#include "srWindow.h"
#include "srControl.h"

namespace srgui {

void srControl::setArea(const srRect& r) 
{
	area = r;
	srControl* c = parent;
	if( c )
	{
		while( c->getParent() ) c = c->getParent();
		srWindow* w = dynamic_cast<srWindow*>(c);
		if(w) w->setDirty();
	}
	return;
}

} //end of srgui namespace


