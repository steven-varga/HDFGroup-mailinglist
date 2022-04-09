/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <math.h>
#include <variant>
#include <algorithm>

#ifndef  PLOT_ATTRIBUTE_HPP
#define  PLOT_ATTRIBUTE_HPP
// initializer for  must be brace-enclosed
namespace plot::attribute {
	enum class hue_t : unsigned {
		aliceblue=0xF0F8FF, blueviolet=0x8a2be2,
	};
	enum class align_t : char { // how objects behave within a bounding box
		left=0, center, right, top, bottom };

	enum class weight_t : char { // font property
		bold=0x0, normal=0x1, italic=0x2 };

	struct degree_t { // in degrees, specify rotation/angle 
		using value_type = tag::degree_t;
		float value;
	};
	struct ccw_t : public degree_t { // counter clock wise on degree
		using value_type = tag::ccw_t;
	};
	struct cw_t : public degree_t { // clock wise in degree
		using value_type = tag::cw_t;
	};
	struct radian_t : public degree_t { // from  radian to degree
		using value_type = tag::radian_t;
	};
	struct layout_t { // layout type
		using value_type = tag::layout_t;
		int value;
	};
	struct margin_t {
		using value_type = tag::margin_t;
		std::array<float,4> value;
	};


	struct color_t {
		using value_type = tag::color_t;
		color_t(unsigned value ) : color_t({value}){
		}
		color_t( const std::initializer_list<unsigned int> list  )
			: length(list.size()), ptr(new unsigned[length]) {
			std::copy(list.begin(),list.end(), ptr.get());
		}
		operator unsigned() const {
			return *ptr.get();
		}
		const unsigned& operator[]( std::ptrdiff_t i) const{
			return ptr.get()[ i % length ];
		}
		size_t length;
		std::shared_ptr<unsigned> ptr;
	};

	struct font_t {
		using value_type = tag::font_t;

		font_t() : family("Arial, Helvetica, sans-serif"),
	   		weight("normal"), size(9) {
		}
		font_t(const std::string& name, const std::string& weight, unsigned size)
		: family(name), weight(weight), size(size){
		}
		font_t(const std::string& name, unsigned size)
		: family(name), weight("normal"), size(size){
		}
		font_t(unsigned size)
		: family("Arial, Helvetica, sans-serif"), weight("normal"), size(size){
		}

		std::string family;
		unsigned size;
		std::string weight;
		std::optional<std::string> style;
	};

	struct stroke_t {
		using value_type = tag::stroke_t;
		float opacity;
		float width;

		//std::optional<>dasharray, dash_offset,
		std::optional<unsigned > linecap;
	   	std::optional<unsigned>	linejoin;
		std::optional<unsigned> miterlimit;
	};

	struct opacity_t {
		using value_type = tag::opacity_t;
		float value;
	};
	struct id_t {
		using value_type = tag::id_t;
		std::string value;
	};

	struct position_t {
		using value_type = tag::position_t;

		std::variant<size_t, align_t> x;
		std::variant<size_t, align_t> y;
	};
	struct marker_t {
		using value_type = tag::marker_t;
		int value;
	};


	template <class... Ts>
	std::optional<degree_t> get_rotate(Ts... args){
		using degree_t   = typename arg::tpos<tag::degree_t, Ts...>;
		using cw_t   = typename arg::tpos<tag::cw_t, Ts...>;
		using ccw_t   = typename arg::tpos<tag::ccw_t, Ts...>;
		using horizontal_t   = typename arg::tpos<tag::horizontal_t, Ts...>;
		using vertical_t   = typename arg::tpos<tag::vertical_t, Ts...>;

		auto tuple = std::forward_as_tuple(args...);
		// rotation can be defined differently
		if constexpr( degree_t::present )
			return std::get<degree_t::position>( tuple );
		else if constexpr( cw_t::present )
			return std::get<cw_t::position>( tuple );
		else if constexpr( ccw_t::present ){
			return attribute::degree_t{ 360.0f - std::get<ccw_t::position>( tuple ).value};
		}
		return std::nullopt;
	}

	struct element_t {
		template <class... Ts> element_t(Ts... args ){
			auto tuple = std::forward_as_tuple(args...);

			this->color = arg::get<color_t>(args...);
			this->opacity = arg::get<opacity_t>(args...);
			this->position = arg::get<position_t>(args...);
			this->stroke = arg::get<stroke_t>(args...);
			this->id = arg::get<id_t>(args...);
			this->rotate = get_rotate(args...);

			using align_tt = typename arg::tpos<plot::attribute::align_t, Ts...>;
			if constexpr( align_tt::present )
				this->align = std::get<align_tt::value>( tuple );
			this->layout = arg::get<layout_t>(args...);
			this->font = arg::get<font_t>(args...);
		}

		std::optional<color_t> color;
		std::optional<opacity_t> opacity;
		std::optional<position_t> position;
		std::optional<id_t> id;
		std::optional<stroke_t> stroke;
		std::optional<color_t> fill;
		std::optional<degree_t> rotate;
		std::optional<align_t> align;
		std::optional<layout_t> layout;
		std::optional<font_t> font;
		std::optional<std::string> label;
		std::optional<std::string> href;
	};
}

namespace plot {
	using position = attribute::position_t;
	using font = attribute::font_t;
	using hue = attribute::hue_t;
	using align = attribute::align_t;
	using weight = attribute::weight_t;
	using margin = attribute::margin_t;
	namespace layout {
		using layout_t = attribute::layout_t;
		static constexpr layout_t horizontal = attribute::layout_t{1};
		static constexpr layout_t vertical = attribute::layout_t{0};
	}
	namespace marker {
		using marker_t = attribute::marker_t;
		static constexpr marker_t rect = attribute::marker_t{1};
		static constexpr marker_t circle = attribute::marker_t{2};
		static constexpr marker_t hex = attribute::marker_t{3};
		static constexpr marker_t star = attribute::marker_t{4};
		static constexpr marker_t point = attribute::marker_t{5};
		static constexpr marker_t line = attribute::marker_t{6};
	}
	namespace rotate {
		using ccw = attribute::ccw_t;
		using cw = attribute::cw_t;
		using degree = attribute::degree_t;
		using radian = attribute::radian_t;
		static constexpr degree horizontal = attribute::degree_t{0.0f};
		static constexpr degree no = attribute::degree_t{0.0f};
		static constexpr degree vertical = attribute::degree_t{270.0f};
	}

	namespace color {
		using fg = attribute::color_t;
		using bg = attribute::color_t;
	}

	struct width {
		using value_type = tag::width_t;
		size_t value;
	};
	struct height {
		using value_type = tag::height_t;
		size_t value;
	};
}
#endif
