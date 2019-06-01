#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <cairo.h>
#include <pango/pangocairo.h>

namespace srgui {

struct srRect
{
	int x, y;
	uint32_t width, height;
};

struct srPoint
{
	int x, y;
	srPoint operator+(const srPoint& p)
	{
		return { x + p.x, y + p.y };
	}
	srPoint operator-(const srPoint& p)
	{
		return { x - p.x, y - p.y };
	}
};


enum srControlType { SR_CT_WINDOW = 1, SR_CT_BUTTON, SR_CT_CHECKBOX, SR_CT_RADIOBUTTON, SR_CT_LABEL,
			SR_CT_SPINNER, SR_CT_TEXTFIELD, SR_CT_PROGRESSBAR
	 };
enum srDrawInfoFlags { SR_DIF_FOCUS = 1, SR_DIF_MOUSE_OVER = 2, 
		       SR_DIF_MOUSE_LEFT = 4, SR_DIF_MOUSE_MIDDLE = 8,
		       SR_DIF_MOUSE_RIGHT = 16 };
enum srEventType { SR_EVENT_MOUSE_MOVE = 1, SR_EVENT_MOUSE_DOWN, SR_EVENT_MOUSE_UP, SR_EVENT_KEY_PRESS };

class srDrawSurface;

struct srDrawInfo
{
	srDrawSurface* surface;
	uint32_t flags;
	srPoint mouse_rel;
	srPoint parent_offset;
};

/* srControlFlags: for the moment, the top 32bits is reserved for the system and currently used only
 * 	for a control to event repainting and some layout information.
 *      The lowest 32bits is available to the widget implementation as general storage.
 */

const uint64_t SR_CF_REPAINT_ON_HOVER = (1ULL<<63);
const uint64_t SR_CF_REPAINT_ON_LBUTTON_STATE = (1ULL<<62); 
const uint64_t SR_CF_REPAINT_ON_RBUTTON_STATE = (1ULL<<61);
const uint64_t SR_CF_REPAINT_ON_MBUTTON_STATE = (1ULL<<60);
const uint64_t SR_CF_REPAINT_ON_KEY_STATE = (1ULL<<59);
const uint64_t SR_CF_REPAINT_WHILE_FOCUS = (1ULL<<58);
const uint64_t SR_CF_REPAINT_ON_LEFT_CLICK = (1ULL<<57);
const uint64_t SR_CF_REPAINT_ON_MID_CLICK = (1ULL<<56);
const uint64_t SR_CF_REPAINT_ON_RIGHT_CLICK = (1ULL<<55);
const uint64_t SR_CF_REPAINT_ON_KEY_PRESS = (1ULL<<54);
const uint64_t SR_CF_LAYOUT_FIXED_WIDTH = (1ULL<<49);
const uint64_t SR_CF_LAYOUT_FIXED_HEIGHT = (1ULL<<48);
const uint64_t SR_CF_LAYOUT_FIXED_Y_OFFSET = (1ULL<<47);
const uint64_t SR_CF_LAYOUT_FIXED_X_OFFSET = (1ULL<<46);

class srWindow;
class srControl;

struct srInputTracker
{
	srWindow* window;
	srControl* child;
	operator bool() const { return (window != nullptr); }

	bool operator==(const srInputTracker& b)
	{
		return (window!=nullptr) && b && (window == b.window) && (child == b.child);
	}

	void clear() { window = nullptr; child = nullptr; }
};

	/* srStyleFlags top 16bits are unique to the control being styled.
	 * low 16bits are intended to be relevant to any(ish) widget.
	 */
const uint32_t SR_SF_OUTLINE = 1;
const uint32_t SR_SF_WINDOW_DECOR_BUTTON_HOVER_COLOR = (1<<16);
const uint32_t SR_SF_WINDOW_DECOR_BUTTON_ROUND = (1<<17);
const uint32_t SR_SF_WINDOW_ROUND_BOTTOM = (1<<19);
const uint32_t SR_SF_BUTTON_USE_WINDOW_BGCOLOR = (1<<16);

struct srLookAndFeel
{
	uint32_t windowStyle;
	uint32_t windowBackground;
	uint32_t windowCaptionColor1, windowCaptionColor2;
	uint32_t windowCaptionHeight;
	uint32_t windowCaptionRounding;
	srPoint windowCaptionOffset;
	uint32_t windowCaptionTextColor;
	uint32_t windowOutline;
	uint32_t windowDecorButtonOutline;
	uint32_t windowDecorButtonMinColor;
	uint32_t windowDecorButtonMaxColor;
	uint32_t windowDecorButtonCloseColor;

	uint32_t buttonStyle;
	uint32_t buttonBackground;
	uint32_t buttonRounding;
	uint32_t buttonOutline;

	uint32_t sliderSideColor;

	uint32_t itemSelectionColor;

	uint32_t textColor;
	uint32_t textBackground;
};

struct srRenderTask;

bool point_in_rect(const srRect& r, const srPoint& p);
void initialize();
void setDefaultStyle();
void generateDrawList(std::vector<srRenderTask>&);
srWindow* CreateWindow(const srRect& extents, void* userData, const std::string& title);
/* the most raw way to send an event. pass zero for unused data parameters */
void SendEvent(srEventType event, int data0, int data1, int data2, int data3);


struct system_data 
{
	std::vector<srWindow*> windows;
	srInputTracker mouse_over;
	srInputTracker mouse_l_down;
	srInputTracker mouse_r_down;
	srInputTracker mouse_m_down;
	srInputTracker mouse_l_up;
	srInputTracker mouse_r_up;
	srInputTracker mouse_m_up;

	bool caption_move;

	srPoint mouse_pos;
	srLookAndFeel UIStyle;
};

}; //end of srgui namespace



