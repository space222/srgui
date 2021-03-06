#pragma once
#include <vector>
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srgui.h"
#include "srControl.h"

namespace srgui {

class srRadioGroup;

class srRadioButton : public srControl, public srIEvent
{
public:
	srRadioButton() : text_layout(nullptr), isChecked(false), r_group(nullptr) { flags = SR_CF_REPAINT_ON_LEFT_CLICK; }
	srRadioButton(const std::string&, const srPoint&);

	virtual ~srRadioButton();

	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_RADIOBUTTON; }

	// the setter is mainly for use of srRadioGroup.
	// use getGroup and mutate on that.
	virtual void setGroup(srRadioGroup* rg);

	virtual srRadioGroup* getGroup() { return r_group; }

	virtual void setText(const std::string& str);

	virtual void raiseClickEvent(const srEventInfo& ei) override;

	virtual bool checked() { return isChecked; }
	virtual void setChecked(bool v);

protected:
	std::string text;
	srPangoTextLayout* text_layout;
	bool isChecked;
	srRadioGroup* r_group;
};

class srRadioGroup
{
public:
	srRadioGroup() { }
	virtual ~srRadioGroup() { }

	srRadioGroup(std::initializer_list<srRadioButton*> i) : buttons(i) 
	{
		for(auto i = begin(buttons); i != end(buttons); ++i) (*i)->setGroup(this);
		return;
	}
	void add(srRadioButton* b);
	void remove(srRadioButton* b);
	void activateButton(srRadioButton* b);
	int size() { return buttons.size(); }
protected:
	std::vector<srRadioButton*> buttons;
};

} //end of namespace srgui


