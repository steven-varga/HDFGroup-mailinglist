/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#include <iterator>

#ifndef  PLOT_TEXT_HPP
#define  PLOT_TEXT_HPP
namespace plot {
	struct text : public attribute::element_t, public impl::io_t<text> {
		using value_type = tag::text_t;
		text( std::initializer_list<std::string> il) {
			std::copy(il.begin(), il.end(), std::back_inserter(values));
		}
		template <class... Ts>
		text( const std::string& value , Ts... args )
			: attribute::element_t(args...) {
				values.push_back(value);
		}

		void ostream( impl::canvas_t& os ) const {
		}

		std::vector<std::string> values;
	};

	template <class... Ts>
	struct title : public attribute::element_t,
					public impl::io_t<title<Ts...>> {
		using value_type = tag::title_t;
		using attribute::element_t::position;
		using attribute::element_t::color;
		using font_t = attribute::font_t;

		title(const std::string& txt, Ts... args)
			: txt(txt), attribute::element_t(args...) {
			this->font = arg::get<font_t>(args...);
		}

		void ostream( impl::canvas_t& os ) const {
			os.text(txt, position->x, position->y, *this );
		}
		const std::string txt;
	};

	template <class... Ts>
	struct footnote : public attribute::element_t, public impl::io_t<footnote<Ts...>> {
		using value_type = tag::footnote_t;
		footnote(const std::string& txt, Ts... args) {
		}
		void ostream( impl::canvas_t& os ) const {
			std::cout << "<footnote base>\n";
		}
	};
	template <class... Ts>
	struct legend :  public attribute::element_t, public impl::io_t<legend<Ts...>> {
		using value_type = tag::legend_t;
		using attribute::element_t::position;
		using attribute::element_t::color;
		using font_t = attribute::font_t;
		using attribute_t = plot::attribute::element_t;
		using align_t = plot::attribute::align_t;
		using color_t = plot::attribute::color_t;

		legend(Ts... args) : attribute::element_t(args...) {
			if( !font ){
			   	this->font = font_t();
				this->font->size *= 0.7;
			}
			this->values =  arg::get<text>(args...)->values;
		}

		void ostream( impl::canvas_t& os ) const {
			size_t font_width = .8 * font->size;
			attribute_t text_attr, marker_attr, group_attr;
			group_attr.font = this->font; group_attr.position = this->position;
			color_t palette = color.value();
			float width = os.grid_x ? os.grid_x.value() : font_width;
			float height = os.grid_y ? os.grid_y.value() : font_width;
			os.group(position->x, position->y, group_attr,
			[&]() -> void {
				for(size_t i=0, j=0; i<values.size(); j += 6 * width, i++){
					marker_attr.color = palette[i];
					os.text(values[i], j + 1.25 * width , 0, text_attr);
					os.rect(j, - height, .9*width, .9*height, 1.5, 1.5,   marker_attr);
				}
			});
		}

		std::vector<std::string> values;
	};
	struct style : text {
		using value_type = tag::style_t;
		style(const std::string& txt) : text( txt ) {
		}
	};
}
#endif
