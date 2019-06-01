#pragma once
#include <string>
#include <cairo.h>
#include <pango/pangocairo.h>
#include "srControl.h"
#include "srDrawSurface.h"

namespace srgui {

class srProgressBar : public srControl
{
public:
	srProgressBar() : text_layout(nullptr) { flags = 0; }
	virtual void draw(const srDrawInfo&) override;
	virtual srControlType type() override { return SR_CT_PROGRESSBAR; }

	virtual void setText(const std::string& str);

	virtual void setProgress(int percent);

protected:
	int progress;
	std::string text;
	srPangoTextLayout* text_layout;
};


} //end of srgui namespace
