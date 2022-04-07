/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <math.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include "utils.hpp"
#include <armadillo>
#include <stdexcept>

#ifndef  PLOT_CANVAS_HPP
#define  PLOT_CANVAS_HPP

namespace plot::impl {
	struct canvas_t {
		using color_t = plot::attribute::color_t;
		using font_t = plot::attribute::font_t;
		using stroke_t = plot::attribute::stroke_t;
		using opacity_t = plot::attribute::opacity_t;
		using id_t = plot::attribute::id_t;
		using position_t = plot::attribute::position_t;
		using attribute_t = plot::attribute::element_t;
		using align_t = plot::attribute::align_t;

		canvas_t(const std::string& filename, size_t x, size_t y, const std::array<float,4>& margin);
		~canvas_t();
		void group (std::variant<size_t, align_t> x, std::variant<size_t, align_t> y, const attribute_t& attr, std::function<void()> const& call );
		void rect(float x, float y, float width, float height, float rx, float ry, const attribute_t& attr );
		void circle(size_t x, size_t y, size_t radius, const attribute_t& attr ){};
		void elipse(size_t cx, size_t cy, size_t rx, size_t ry, const attribute_t& attr ){};
		void line(size_t x1, size_t y1, size_t x2 , size_t y2, unsigned stroke, size_t width, const attribute_t& attr ){};
		void poly_line(const std::vector<size_t>&x, const std::vector<size_t>&y, unsigned stroke, size_t width, const attribute_t& attr ){};
		static std::pair<size_t,size_t>
			bounding_box( 	const std::vector<std::string>& x_axis, double angle_x,
							const std::vector<std::string>& y_axis, double angle_y, size_t font_size );
		void text( const std::string& txt,
				std::variant<size_t, align_t> x, std::variant<size_t, align_t> y, const attribute_t& attr );

		std::optional<float> grid_x, grid_y;

		private:
		void align( std::optional<align_t> alignment );
		float align_x( std::variant<size_t, align_t> x );
		float align_y( std::variant<size_t, align_t> y, const std::optional<font_t>& font);

		size_t width, height;
		float font_size;
		std::array<float,4> margin;
		static constexpr double rad  = 0.017453292519943295;
		std::ofstream os;

		const std::string gr_begin_ = "<g {}>\n";
		const std::string gr_end_ = "</g>\n";
		const std::string group_ = gr_begin_ + "{}" + gr_end_;
		const std::string rotate_ = " rotate({} {} {})";
		const std::string translate_ = "translate({} {})";
		const std::string transform_ = " transform=\"{}\"";

		const std::string svg_start_  = "<svg viewBox=\"{} {} {} {}\"  xmlns=\"http://www.w3.org/2000/svg\">\n";
		const std::string svg_end_ = "</svg>\n";
		const std::string href_begin_ = "<a href=\"{}\">";
		const std::string href_end_   = "</a>\n";

		const std::string text_anchor_ = " text-anchor=\"{}\"";
		const std::string font_ = " font-size=\"{}\" font-family=\"{}\" font-weight=\"{}\"";
		const std::string fill_ = " fill=\"#{:06X}\"";
		const std::array<std::string, 5> align_horizontal = {"start", "middle", "end", "", ""};

		const std::string rect_ = "<rect x=\"{:.2f}\" y=\"{:.2f}\" width=\"{:.2f}\" height=\"{:.2f}\" rx=\"{:.2f}\" ry=\"{:.2f}\" {}>{}</rect>\n";
		const std::string text_ = "<text x=\"{:.2f}\" y=\"{:.2f}\"{}>{}</text>\n";
		const std::string title_ = "<title>{}</title>";
	};
}

namespace plot::impl {
	template <class Derived >
	struct io_t {
		void ostream( impl::canvas_t& os ) const {
		}

		friend impl::canvas_t& operator<<(impl::canvas_t& cs, const io_t<Derived>& attr){
				const Derived& derived = static_cast<const Derived&>(attr);
				derived.ostream( cs );
				return cs;
			};
	};
}

plot::impl::canvas_t::canvas_t( const std::string& filename, size_t width, size_t height, const std::array<float,4>& margin )
	: width(width), height(height), margin(margin) {

	os.open(filename, std::ios::out | std::ios::trunc);
	os << fmt::format( svg_start_, 0,0, width,height);
};

plot::impl::canvas_t::~canvas_t(){
	os << svg_end_;
};


void plot::impl::canvas_t::group (std::variant<size_t, align_t> x, std::variant<size_t, align_t> y, const attribute_t& attr,
		std::function<void()> const& call ){
	float _x = align_x( x ); float _y = align_y( y, attr.font );
	// FIXME: clang++ fails s
	int i = std::holds_alternative<size_t>( x ) ? 0 : static_cast<int>( std::get<align_t>(x) );

	std::string _attr = fmt::format(transform_, fmt::format(translate_, _x, _y));
	if( attr.font )   _attr += fmt::format(font_, attr.font->size, attr.font->family, attr.font->weight);
	if( attr.color )  _attr += fmt::format(fill_, attr.color.value() );

	os << fmt::format(gr_begin_, _attr); // group has properties set
		call();
	os << gr_end_;
};

std::pair<size_t,size_t>
plot::impl::canvas_t::bounding_box( const std::vector<std::string>& x_axis, double angle_x,
	   	const std::vector<std::string>& y_axis, double angle_y,  size_t font_size ){

	return std::make_pair(0,0);
}

void plot::impl::canvas_t::rect(float x, float y, float width, float height, float rx, float ry,
		const attribute_t& attr){
	std::string _attr, _lbl;
	if( attr.color )  _attr += fmt::format(fill_, attr.color.value() );
	if( attr.label )  _lbl = fmt::format(title_, attr.label.value());

	if( attr.href ) os << fmt::format(href_begin_, attr.href.value());
		os << fmt::format(rect_, x, y, width, height, rx, ry, _attr, _lbl );
	if( attr.href ) os << fmt::format(href_end_);
}

void plot::impl::canvas_t::text( const std::string& txt,
		std::variant<size_t, align_t> x, std::variant<size_t, align_t> y, const attribute_t& attr ){
	float _x = align_x( x ); float _y = align_y( y, attr.font );
	int i = std::holds_alternative<size_t>( x ) ? 0 : static_cast<int>(std::get<align_t>(x));

	std::string _attr;
	if( attr.font )   _attr += fmt::format(font_, attr.font->size, attr.font->family, attr.font->weight);
	if( attr.color )  _attr += fmt::format(fill_, attr.color.value() );
	if( attr.rotate ) _attr += fmt::format(transform_, fmt::format(rotate_, attr.rotate->value, _x, _y));
	_attr += fmt::format(text_anchor_, align_horizontal[i]);

	os << fmt::format(text_, _x, _y, _attr, util::html_escape(txt) );
}

void plot::impl::canvas_t::align( std::optional<align_t> x ) {
	if( !x ) return;
		os << fmt::format(text_anchor_, align_horizontal[ static_cast<int>(x.value()) ]);
}

float plot::impl::canvas_t::align_x( std::variant<size_t, align_t> x ) {
	if( std::holds_alternative<size_t>( x ) )
		return std::get<size_t>( x );
	switch( std::get<align_t>(x) ){
		case align_t::left: return std::get<0>(margin);
		case align_t::right: return width - std::get<2>(margin);
		case align_t::center: return width / 2 - std::get<0>(margin);
		default: throw std::runtime_error("fixme: #169");
	}
	return std::get<0>(margin);
}

float plot::impl::canvas_t::align_y( std::variant<size_t, align_t> y, const std::optional<font_t>& font ) {
	if( std::holds_alternative<size_t>( y ) )
		return std::get<size_t>( y );
	switch( std::get<align_t>(y) ){
		case align_t::top: return std::get<1>(margin) + (font ? font->size : 0);
		case align_t::bottom: return height - std::get<3>(margin);
		case align_t::center: return height / 2 - std::get<1>(margin);
		default:
			throw std::runtime_error("fixme: 182");
			break;
	}
	return std::get<1>(margin);
}
#endif
