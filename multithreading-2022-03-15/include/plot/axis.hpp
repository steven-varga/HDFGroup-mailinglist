/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef  PLOT_AXIS2_HPP
#define  PLOT_AXIS2_HPP
#include <cmath>

namespace plot::axis {
	constexpr float rad = 3.14159265358979f / 180.0f;

	template <class T, class... Ts>
	struct x : public attribute::element_t, public impl::io_t< x<T,Ts...>> {
		using value_type = tag::axis::x_t;
		using attribute::element_t::color;
		using attribute::element_t::position;
		using font_t = attribute::font_t;
		using layout_t = attribute::layout_t;
		using degree_t = attribute::degree_t;
		using position_t = attribute::position_t;
		using attribute_t = plot::attribute::element_t;

		x(const std::vector<T>& labels, Ts... args)
			: labels(labels), attribute::element_t(args...) {
			if( !rotate ) rotate = degree_t{-90.0} ;
			dx.resize(labels.size()); dy.resize(labels.size());
			set_font( arg::get<font_t>(args...) );
		}
		void ostream( impl::canvas_t& os ) const {
			os.grid_x = grid;
			attribute_t text_attr;
			text_attr.rotate = this->rotate;
			os.group(position->x, position->y, *this,
			[&]() -> void {
				for(size_t i=0; i<labels.size(); i++)
					os.text(labels[i], dx[i], dy[i], text_attr);
			});
		}
		void set_font( const std::optional<font_t>& font_ ){
			this->font = font_ ? font_ : font_t{"Ubuntu Mono", "bold", 7};
			this->grid = 1.6 * font->size;
			for (size_t i=0; i < labels.size(); i++)
			  	dy[i] = 0.0, dx[i] = i * grid;
			float angle = rotate->value * rad;
			float max = utils::size_of_max_value( labels ) * 0.6 * font->size * std::abs(std::sin(angle));
			if( !position )
		   		position = position_t{2 * grid, max};
		}
		float get_x(){
			return static_cast<float>(
					std::get<size_t>(position->x)) - 0.5 * grid;
		}
		const std::vector<T>& labels;
		std::vector<float> dx,dy;
		float grid, max_x;
	};

	template <class T, class... Ts>
	struct y : public  attribute::element_t, impl::io_t< y<T, Ts...>> {
		using value_type = tag::axis::y_t;
		using attribute::element_t::position;
		using attribute::element_t::color;
		using font_t = attribute::font_t;
		using layout_t = attribute::layout_t;
		using degree_t = attribute::degree_t;
		using attribute_t = plot::attribute::element_t;

		y(const std::vector<T>& labels, Ts... args)
			: labels(labels), attribute::element_t(args...) {
			if( !rotate ) rotate = degree_t{0.0} ;
			this->layout = plot::layout::vertical;
			this->grid = 1.2 * font->size;
			dx.resize(labels.size()); dy.resize(labels.size());
			set_font( arg::get<font_t>(args...) );
		}

		void ostream( impl::canvas_t& os ) const {
			os.grid_y = grid;

			attribute_t text_attr;
			os.group(position->x, position->y, *this,
			[&]() -> void {
				for(size_t i=0; i<labels.size(); i++)
					os.text(labels[i], dx[i], dy[i] + .25 * grid, text_attr);
			});
		}

		void set_font( const std::optional<font_t>& font_ ){
			this->font = font_ ? font_ : font_t{"Ubuntu Mono", "bold", 7};
			this->grid = 1.2 * font->size;
			for (size_t i=0; i < labels.size(); i++)
				dx[i] = 0.0, dy[i] = i * grid;
		}

		float get_y(){
			return dy.back() + grid;
		}
		float get_x(){
			float angle = rotate->value * rad;
			float max = utils::size_of_max_value( labels ) * 0.6 * font->size * std::abs(std::cos(angle));

			return static_cast<float>(
					std::get<size_t>(position->x)) + max;
		}

		const std::vector<T>& labels;
		std::vector<float> dx,dy;
		float grid, max_x, max_y;
	};
}
#endif
