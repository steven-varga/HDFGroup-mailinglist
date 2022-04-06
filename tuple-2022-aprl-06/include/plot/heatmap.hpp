/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#include <string>
#include <vector>
#include <tuple>
#include <experimental/type_traits>
#include <armadillo>

#ifndef  PLOT_HEATMAP_HPP
#define  PLOT_HEATMAP_HPP

namespace plot::data {
	template <class T>
	struct point_t {
		size_t x,y;
		T value;
		std::string label,href;
	};
}

namespace plot::impl {
	template <typename T>
	typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>
	::type heatmap(impl::canvas_t& canvas, float x, float y,
			std::vector<float>& dx, std::vector<float>& dy, float width, float height,
            const arma::Mat<T>& data, const plot::attribute::color_t& palette ) {

		using attribute_t = plot::attribute::element_t;
		attribute_t heatmap_attr, gr_attr;
		// find unique elements
		std::vector M(data.begin(), data.end());
		std::sort(M.begin(), M.end());
		auto last = std::unique(M.begin(), M.end());
	   	M.erase(last, M.end());
        // elements are grouped and their attributes are handled collectively
		canvas.group(x, y, heatmap_attr, [&]() -> void {
			for( auto value : M ) {
				gr_attr.color = palette[value];
				canvas.group(0, 0, gr_attr, [&]() -> void {
					for(size_t j = 0; j < data.n_cols; j++) for( size_t i = 0; i < data.n_rows; i++)
						if( data(i,j) == value )
							canvas.rect( dx[j], dy[i], width, height, 1.5, 1.5, heatmap_attr);
				});
			}
		});
	}
}
namespace plot::impl {
	template <typename T>
	typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>
	::type heatmap(impl::canvas_t& canvas, float x, float y,
			std::vector<float>& dx, std::vector<float>& dy, float width, float height,
			const std::vector<plot::data::point_t<T>>& data,   const plot::attribute::color_t& palette ) {
		using attribute_t = plot::attribute::element_t;
		using element_t = plot::data::point_t<T>;
		attribute_t rect_attr, gr_attr;
		gr_attr.color.reset();
		canvas.group(x,y, gr_attr, [&]() -> void {
			for(auto v : data){
				rect_attr.href = v.href; rect_attr.label = v.label; rect_attr.color = palette[v.value];
				canvas.rect(dx[v.x], dy[v.y], width, height, 1.5, 1.5, rect_attr);
			}
		});
	}
}


namespace plot {
	template <class T, class... arg_t>
	void heatmap(const std::string& filename, const T& data, arg_t... args ) {
		auto rotate = attribute::get_rotate(args...);
		// xyx::type -- original type
		using title_t    = typename arg::tpos<tag::title_t, arg_t...>;
		using footnote_t = typename arg::tpos<tag::footnote_t, arg_t...>;
		using legend_t   = typename arg::tpos<tag::legend_t, arg_t...>;
		using axisx_t    = typename arg::tpos<tag::axis::x_t, arg_t...>;
		using axisy_t    = typename arg::tpos<tag::axis::y_t, arg_t...>;
		using width_t    = typename arg::tpos<tag::width_t, arg_t...>;
		using height_t   = typename arg::tpos<tag::height_t, arg_t...>;
		using margin_t   = typename arg::tpos<tag::margin_t, arg_t...>;
		using font_t   = typename arg::tpos<tag::font_t, arg_t...>;

		static_assert( axisx_t::present, "x axis must be specified..." );
		static_assert( axisy_t::present, "y axis must be specified..." );

		auto tuple = std::forward_as_tuple(args...);

		std::array<float,4> margin{5,5,5,5};
		if constexpr( margin_t::present )
			margin = std::get<margin_t::position>( tuple ).value;

		auto x_axis = std::get<axisx_t::position>( tuple );
		auto y_axis = std::get<axisy_t::position>( tuple );

		float offset_x = std::get<size_t>(x_axis.position->x) - .5 * x_axis.grid;
		float offset_y = std::get<size_t>(x_axis.position->y) + .5 * y_axis.grid;
		// when y axis label position is not preset, compute it from 
		float pos_x = x_axis.dx.back() + 1.5 * x_axis.grid + offset_x;
		if( !y_axis.position )
				y_axis.position = position{pos_x, offset_y + .5 * y_axis.grid };
		// compute size when not set
		size_t width, height;
	   	if constexpr( width_t::present )
			width = std::get<width_t::position>( tuple ).value;
		else
			width = y_axis.get_x() + margin[2];
	   	if constexpr( height_t::present )
			height = std::get<height_t::position>( tuple ).value;
		else
			height = y_axis.get_y() + offset_y + margin[3];

		auto canvas = plot::impl::canvas_t(filename, width, height + 10, margin);
		canvas << x_axis; canvas << y_axis;

		if constexpr (title_t::present)    canvas << std::get<title_t::position>( tuple );
		if constexpr (footnote_t::present) canvas << std::get<footnote_t::position>( tuple );
		if constexpr (legend_t::present)   canvas << std::get<legend_t::position>( tuple );
		auto legend = std::get<legend_t::position>( tuple );

		impl::heatmap(canvas,
				offset_x, offset_y, x_axis.dx, y_axis.dy,
				.9 * x_axis.grid, .9 * y_axis.grid, data, legend.color.value() );
	};
}
#endif
