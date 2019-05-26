#pragma once
#include <cairo.h>
#include <pango/pangocairo.h>

namespace srgui {

class srTextLayout
{
public:
	virtual void setFont(const std::string&) =0;

	virtual void getPosFromIndex(int index, srPoint& p) =0;
	virtual bool getIndexFromPos(const srPoint& p, int &index) =0; 
	virtual void getCursorPos(int index, srRect& r) =0;

	// getExtents must only set width and height.
	virtual void getExtents(srRect&) =0;
	virtual void setText(const std::string&) =0;

	virtual ~srTextLayout() {}
};

class srDrawSurface
{
	/* srDrawSurface is the abstract class that provides a drawing API
 	 * for drawing to a surface. It will be more or less limited to 
	 * what windows and widgets need in order to draw. Currently the
	 * only implementation is a thin wrapper around a Cairo 
	 * Image(memory) surface using Pango, but maybe eventually HarfBuzz and
	 * Cairo for text (to enable static linking without copyleft).
	 */
public:
	virtual void setSize(int width, int height) =0;

	/* for debugging only */
	virtual void write_to_png(const std::string&) =0;

	virtual void outlineCircle(const srPoint& p, float radius, float linewidth = 2.0f) =0;
	virtual void drawCircle(const srPoint& p, float radius) =0;

	virtual void drawRectangle(const srRect& r) =0;
	virtual void drawRoundRectangle(const srRect& r, int rounding) =0;
	virtual void drawTopRoundedRect(const srRect& r, int rounding) =0;
	virtual void outlineRectangle(const srRect& r, float linewidth = 2.0f) =0;
	virtual void outlineRoundRectangle(const srRect& r, int rounding, float linewidth = 2.0f) =0;
	virtual void outlineTopRoundedRect(const srRect& r, int rounding, float linewidth = 2.0f) =0;
	virtual void setLinearGradient(uint32_t color1, uint32_t color2) =0;
	virtual void setLinearGradientWidth(float) =0;
	virtual void useLinearGradient() =0;
	virtual void setColor(float r, float g, float b, float a = 1.0f) =0;
	virtual void setColor(uint32_t rgba) =0;

	virtual void drawLine(float x1, float y1, float x2, float y2, float linewidth = 2.0f) =0;

	virtual void drawTextLayout(const srPoint& p, srTextLayout*) =0;

	/* this is where the client will stuff a pointer to a struct that is the
	 * client-surface info (whether their engine's OpenGL texture object, SDL_Surface, etc)
	 */
	virtual void setUserData(void* ud) { userData = ud; }
	virtual void* getUserData() { return userData; }
protected:
	void* userData;
};

class srPangoTextLayout : public srTextLayout
{
friend class srDrawSurface;
public:
	srPangoTextLayout();
	virtual ~srPangoTextLayout();
	virtual void setFont(const std::string&) override;
	virtual void getExtents(srRect&) override;
	virtual void setText(const std::string&) override;
	virtual bool getIndexFromPos(const srPoint& p, int &index) override;
	virtual void getPosFromIndex(int index, srPoint& p) override;

	virtual void getCursorPos(int index, srRect& r) override;
	PangoLayout* getPangoLayout() { return layout; }
protected:
	PangoLayout* layout;
	PangoContext* context;
};

class srCairoDrawSurface : public srDrawSurface
{
public:
	srCairoDrawSurface(int Width, int Height);

	virtual void write_to_png(const std::string&) override;

	virtual void setSize(int Width, int Height) override;

	virtual void outlineCircle(const srPoint& p, float radius, float linewidth = 2.0f);
	virtual void drawCircle(const srPoint& p, float radius);
	virtual void drawRectangle(const srRect& r);
	virtual void drawRoundRectangle(const srRect& r, int rounding);
	virtual void drawTopRoundedRect(const srRect& r, int rounding);
	virtual void outlineRectangle(const srRect& r, float linewidth = 2.0f);
	virtual void outlineRoundRectangle(const srRect& r, int rounding, float linewidth = 2.0f);
	virtual void outlineTopRoundedRect(const srRect& r, int rounding, float linewidth = 2.0f);
	virtual void setLinearGradient(uint32_t color1, uint32_t color2);
	virtual void setLinearGradientWidth(float);
	virtual void useLinearGradient();
	virtual void setColor(float r, float g, float b, float a = 1.0f);
	virtual void setColor(uint32_t rgba);

	virtual void drawTextLayout(const srPoint& p, srTextLayout*) override;

	virtual void drawLine(float x1, float y1, float x2, float y2, float width = 2.0f) override;

	virtual uint8_t* getBuffer() { return buffer; }
	virtual int getStride() { return stride; }
protected:
	int width, height, stride;
	uint8_t* buffer;
	cairo_t* ct;
	cairo_pattern_t* gradient;
};

struct srRenderTask
{
	srDrawSurface* surface;
	srRect extents;
	bool dirty;
	srRenderTask(srDrawSurface* s, const srRect& r, bool d) : surface(s), extents(r), dirty(d) { }
};

} //end of srgui namespace


