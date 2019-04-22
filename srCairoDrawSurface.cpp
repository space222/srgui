#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <cairo.h>
#include "srgui.h"
#include "srDrawSurface.h"

namespace srgui {

srPangoTextLayout::srPangoTextLayout()
{
	PangoFontMap* font_map = pango_cairo_font_map_get_default(); // owned by pango system, no deleting
	context = pango_font_map_create_context(font_map);
	layout = pango_layout_new(context);
	return;
}

srPangoTextLayout::~srPangoTextLayout()
{
	g_object_unref(layout);
	g_object_unref(context);
	return;
}

void srPangoTextLayout::getExtents(srRect& r)
{
	int w, h;
	pango_layout_get_pixel_size(layout, &w, &h);
	r.width = w;
	r.height = h;
	return;
}

void srPangoTextLayout::setText(const std::string& text)
{
	pango_layout_set_text(layout, text.c_str(), -1);
	return;
}

void srPangoTextLayout::setFont(const std::string& fdsc)
{
	PangoFontDescription* font_desc = pango_font_description_from_string(fdsc.c_str());
	pango_layout_set_font_description(layout, nullptr);   // pango will silently not make a copy if 
							      // the new fontdesc matches the old one
							      // whether by pointer or deep compare.
							      // this makes sure the old one is deleted
							      // in favor of a new copy
	pango_layout_set_font_description(layout, font_desc);
	//pango_font_map_load_font(font_map, pc, font_desc);  // most examples do this, but does not appear
							      // to actually be necessary.

	pango_font_description_free(font_desc); 	      // no longer needed, pango makes a copy
	
	return;
}

void srCairoDrawSurface::drawRectangle(const srRect& r) 
{
	cairo_rectangle(ct, r.x, r.y, r.width, r.height);
	cairo_fill(ct);
	return;
}

void srCairoDrawSurface::drawTextLayout(const srPoint& p, srTextLayout* ptl)
{
	cairo_move_to(ct, p.x, p.y);
	pango_cairo_show_layout(ct, ((srPangoTextLayout*) ptl)->getPangoLayout());
	return;
}

void srCairoDrawSurface::drawRoundRectangle(const srRect& r, int rounding)
{
	if( rounding == 0 )
	{
		drawRectangle(r);
		return;
	}

	double degrees = 3.14596 / 180.0;

	cairo_new_sub_path(ct);
	//cairo_move_to(ct, r.x, r.y);
	cairo_arc(ct, r.x + r.width - rounding, r.y + rounding, rounding, -90 * degrees, 0 * degrees);
	cairo_arc(ct, r.x + r.width - rounding, r.y + r.height - rounding, rounding, 0 * degrees, 90 * degrees);
	cairo_arc(ct, r.x + rounding, r.y + r.height - rounding, rounding, 90 * degrees, 180 * degrees);
	cairo_arc(ct, r.x + rounding, r.y + rounding, rounding, 180 * degrees, 270 * degrees);
	cairo_close_path(ct);
	cairo_fill(ct);
	return;
}

void srCairoDrawSurface::outlineRectangle(const srRect& r, float linewidth)
{
	cairo_rectangle(ct, r.x, r.y, r.width, r.height);
	cairo_set_line_width(ct, linewidth);
	cairo_stroke(ct);
	return;
}

void srCairoDrawSurface::drawTopRoundedRect(const srRect& r, int rounding)
{
	if( rounding == 0 )
	{
		drawRectangle(r);
		return;
	}

	double degrees = 3.14596 / 180.0;

	cairo_new_sub_path(ct);
	//cairo_move_to(ct, r.x, r.y);
	cairo_arc(ct, r.x + rounding, r.y + rounding, rounding, 180 * degrees, 270 * degrees);
	cairo_arc(ct, r.x + r.width - rounding, r.y + rounding, rounding, -90 * degrees, 0 * degrees);
	cairo_line_to(ct, r.x+r.width, r.y+r.height);
	cairo_line_to(ct, r.x, r.y+r.height);
	cairo_close_path(ct);

	cairo_fill(ct);

	return;
}

void srCairoDrawSurface::outlineTopRoundedRect(const srRect& r, int rounding, float linewidth)
{
	if( rounding == 0 )
	{
		outlineRectangle(r, linewidth);
		return;
	}

	double degrees = 3.14596 / 180.0;

	cairo_new_sub_path(ct);
	cairo_arc(ct, r.x + rounding, r.y + rounding, rounding, 180 * degrees, 270 * degrees);
	cairo_arc(ct, r.x + r.width - rounding, r.y + rounding, rounding, -90 * degrees, 0 * degrees);
	cairo_line_to(ct, r.x+r.width, r.y+r.height);
	cairo_line_to(ct, r.x, r.y+r.height);
	cairo_close_path(ct);

	cairo_set_line_width(ct, linewidth);
	cairo_stroke(ct);

	return;
}

void srCairoDrawSurface::outlineRoundRectangle(const srRect& r, int rounding, float linewidth)
{
	if( rounding == 0 )
	{
		outlineRectangle(r, linewidth);
		return;
	}

	double degrees = 3.14596 / 180.0;

	cairo_new_sub_path(ct);
	cairo_arc(ct, r.x + r.width - rounding, r.y + rounding, rounding, -90 * degrees, 0 * degrees);
	cairo_arc(ct, r.x + r.width - rounding, r.y + r.height - rounding, rounding, 0 * degrees, 90 * degrees);
	cairo_arc(ct, r.x + rounding, r.y + r.height - rounding, rounding, 90 * degrees, 180 * degrees);
	cairo_arc(ct, r.x + rounding, r.y + rounding, rounding, 180 * degrees, 270 * degrees);
	cairo_close_path(ct);
	cairo_set_line_width(ct, linewidth);
	cairo_stroke(ct);
	return;
}

void srCairoDrawSurface::setLinearGradient(uint32_t color1, uint32_t color2)
{
	cairo_pattern_t* pat1 = cairo_pattern_create_linear(0, 0, 1.0f, 0);
	float alpha = (color1>>24)/255.0f;
	float r = ((color1>>16)&0xFF)/255.0f;
	float g = ((color1>>8)&0xFF)/255.0f;
	float b = (color1&0xFF) / 255.0f;
	cairo_pattern_add_color_stop_rgba(pat1, 0.0f, r, g, b, 1.0f-alpha);
	alpha = (color2>>24)/255.0f;
	r = ((color2>>16)&0xFF)/255.0f;
	g = ((color2>>8)&0xFF)/255.0f;
	b = (color2&0xFF) / 255.0f;
	cairo_pattern_add_color_stop_rgba(pat1, 1.0f, r, g, b, 1.0f-alpha);

	if( gradient ) cairo_pattern_destroy(gradient);
	gradient = pat1;

	setLinearGradientWidth(width);
	useLinearGradient();

	return;
}

void srCairoDrawSurface::useLinearGradient()
{
	cairo_set_source(ct, gradient);
	return;
}

void srCairoDrawSurface::setLinearGradientWidth(float gw)
{
	cairo_matrix_t mat1;
	cairo_matrix_init_scale(&mat1, 1.0f/(gw+0.01f), 1);
	cairo_pattern_set_matrix(gradient, &mat1);
	return;
}

void srCairoDrawSurface::setColor(float r, float g, float b, float a)
{
	cairo_set_source_rgba(ct, r, g, b, a);
	return;
}

void srCairoDrawSurface::setColor(uint32_t rgba)
{
	float alpha = (rgba>>24)/255.0f;
	float r = ((rgba>>16)&0xFF)/255.0f;
	float g = ((rgba>>8)&0xFF)/255.0f;
	float b = (rgba&0xFF) / 255.0f;
	cairo_set_source_rgba(ct, r, g, b, 1.0f-alpha);
	return;
}

void srCairoDrawSurface::drawLine(float x1, float y1, float x2, float y2, float linewidth)
{
	cairo_move_to(ct, x1, y1);
	cairo_line_to(ct, x2, y2);
	cairo_set_line_width(ct, linewidth);
	cairo_stroke(ct);
	return;
}

void srCairoDrawSurface::setSize(int Width, int Height)
{
	this->width = Width;
	this->height = Height;
	this->stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, Width);
	
	if( buffer ) delete[] buffer;
	buffer = new uint8_t[stride*Height*4];

	if( ct ) cairo_destroy(ct);
	cairo_surface_t* sur = cairo_image_surface_create_for_data(buffer, CAIRO_FORMAT_ARGB32, width, height, stride);
	ct = cairo_create(sur);
	cairo_surface_destroy(sur);

	return;
}

srCairoDrawSurface::srCairoDrawSurface(int wid, int ht)
{
	gradient = nullptr;
	ct = nullptr;
	buffer = nullptr;

	setSize(wid, ht);
	return;
}

void srCairoDrawSurface::write_to_png(const std::string& filename)
{
	cairo_surface_t* surfy = cairo_get_target(ct);
	cairo_surface_flush(surfy);
	cairo_surface_write_to_png(surfy, filename.c_str());
	return;
}

	

}; //end of srgui namespace



