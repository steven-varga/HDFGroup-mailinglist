/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */


#ifndef  PLOT_TAGS_HPP
#define  PLOT_TAGS_HPP

namespace plot::tag {
	struct width_t{};
	struct height_t{};
	struct position_t{};
	struct font_t{};
	struct color_t{};
	struct layout_t{};
	struct stroke_t{};
	struct opacity_t{};
	struct marker_t{};
	namespace axis {
		struct x_t{};
		struct y_t{};
	}
	struct text_t{};
	struct title_t{};
	struct footnote_t{};
	struct legend_t{};
	struct id_t{};
	struct style_t{};
	struct degree_t{};
	struct radian_t{};
	struct cw_t{};
	struct ccw_t{};
	struct horizontal_t{};
	struct vertical_t{};
	struct margin_t{};
}
#endif

