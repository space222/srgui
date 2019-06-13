#pragma once
#include <vector>
#include <string>
#include <functional>
#include "srgui.h"
#include "srControl.h"
#include "srWindow.h"

namespace srgui {

struct srMenuItem
{
	std::string text;
	std::function<void()> onClick;

	srMenuItem(const std::string& t, const std::function<void()>& fn) : text(t), onClick(fn) {}
};

class srDrawSurface;

class srMenu : public srControl, public srIOverlay, public srIEvent
{
public:
	srMenu() : text(), dirty(true), surface(nullptr), text_layout(nullptr) { }
	virtual ~srMenu() { delete surface; delete text_layout; }

	virtual srDrawSurface* getSurface() override { return surface; }
	virtual void setDirty() override { dirty = true; return; }
	virtual bool getDirty() override { return dirty; }
	virtual void clearDirty() override { dirty = false; return; }

	void setText(const std::string& s)
	{
		text = s;
		dirty = true;
		return;
	}

	std::string getText() { return text; }

	int size()
	{
		return items.size(); 
	}

	srMenuItem& item(int index)
	{
		dirty = true;        // Have to assume modification.
		return items[index]; // Yes, this can throw. Maybe don't mess up the indices
				 // until I come up with something better. :P
				 // Hopefully this kind of grabbing and mutating an
				 // individual srMenuItem struct won't be a commonly
				 // necessary thing. (all the mutators in srMenu instead?)
	}

	int useItems(std::initializer_list<srMenuItem> itms)
	{
		int retval = items.size();
		items = std::vector<srMenuItem>(itms);
		dirty = true;
		resize();
		return retval;
	}

	int addItem(const std::string& s, const std::function<void()>& fn)
	{
		int retval = items.size();
		items.emplace_back(s, fn);
		dirty = true;
		resize();
		return retval;
	}

	void removeItem(int index)
	{
		if( index >= items.size() ) return;
		items.erase(begin(items)+index);
		dirty = true;
		resize();
		return;
	}

	virtual void draw(const srDrawInfo& info) override;
	
	friend void generateDrawList(std::vector<srRenderTask>& tasks);
	friend class srMenuBar;
protected:
	void resize();

	bool dirty;
	std::string text;
	std::vector<srMenuItem> items;
	srDrawSurface* surface;
	srPangoTextLayout* text_layout;
};

class srMenuBar : public srControl, public srIEvent
{
public:
	srMenuBar() 
	{
		flags = SR_CF_REPAINT_ON_HOVER | SR_CF_REPAINT_ON_LBUTTON_STATE;
		text_layout = new srPangoTextLayout;
		return;
	}

	virtual void raiseClickEvent(const srEventInfo&) override;
	virtual void draw(const srDrawInfo& info) override;

	virtual void add(srMenu* menu)
	{
		menus.push_back(menu);
		srWindow* w = dynamic_cast<srWindow*>(getToplevelParent());
		if( w ) w->setDirty();
		return;
	}

protected:
	std::vector<srMenu*> menus;
	std::vector<int> menu_offsets;
	srPangoTextLayout* text_layout;
};

} //end of srgui namespace









