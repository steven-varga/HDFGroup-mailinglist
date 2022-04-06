/*
 * Copyright (c) 2017 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 *
 */

#ifndef  H5CPP_TEST_SVGIMPL_HPP
#define  H5CPP_TEST_SVGIMPL_HPP

template <class T>
heatmap_t<T>::heatmap_t(const std::string& filename, const std::tuple<Ts...>& tuple ) :
		width(800), height(400), font_size(14), font_family("courier"), color("blue") {
};


template <class T>
void heatmap_t<T>::print(  ){
/*
	svg.open(this->file_name + ".svg", std::ios::out | std::ios::trunc);

	size_t x=0, y=0; // find dimensions
	for( auto &line : *ptr ) // insert the two axes into a map
		insert_x(x, std::get<0>(line)), insert_y(y, std::get<1>(line));
	// order axes by insertion index
	std::vector<std::string> x_values = values_by_idx(x_axis);
	std::vector<std::string> y_values = values_by_idx(y_axis);
	// find the longest strings in each directions
	size_t max_x = size_of_max_value(x_axis);
	size_t max_y = size_of_max_value(y_axis);

	width  = x * 1 / cos_90 * font_size + max_y * font_size * cos_90;
	height = sin_90 * sin_90 * max_x * font_size + (y + 1) * font_size;
	print_doc_header("H5CPP Test Header", font_size, sin_90 * sin_90 * max_x * font_size);
		print_x_axis(x_values, font_size, 0);
		print_y_axis(y_values, x * 1 / cos_90 * font_size + 0.8 * font_size, 0.8 * font_size);
		print_cells(0, 0);
	print_doc_footer();
	svg.close();
	*/
}


template <class T>
void heatmap_t<T>::print_doc_header(const std::string& title, size_t x, size_t y) {
	svg << "<svg width=\"" << width << "\" height=\"" << height << "\">\n";
	svg <<"\t<text x=\"" << font_size << "\""
		" y=\"" << 3 * font_size <<"\" font-size=\"" << 2 * font_size << "\"  class=\"heavy\">"
		<< title << "</text>\n";
	svg << "\t<g id=\"chart\" font-size=\""<< font_size << "\" font-family=\"" << font_family
				<< "\" color=\"" << color <<"\" fill=\"currentColor\" "
				"transform=\"translate(" << x << " " << y << ")\">\n";
 }

template <class T>
void heatmap_t<T>::print_doc_footer(){
	svg <<"\t</g>\n</svg>\n";
}

template <class T>
void heatmap_t<T>::print_x_axis( const std::vector<std::string>& values, size_t x, size_t y ){
	size_t n = values.size();

	svg << "\t\t<g id=\"top\" transform=\"rotate(-45) translate(" << x << " " << y << ")\" text-anchor=\"start\">\n";
	for( size_t i=0; i < n; i++){
		int X = i * font_size;
		int Y = i * font_size; 
		svg << "\t\t\t<text x=\"" << X << "\" y=\"" << Y << "\">" << values[i] << "</text>\n";
	}
	svg << "\t\t</g>\n";
}

template <class T>
void heatmap_t<T>::print_y_axis( const std::vector<std::string>& values, size_t x, size_t y ){
	size_t n = values.size();

	svg << "\t\t<g id=\"side\" text-anchor=\"start\"  transform=\"translate(" << x << " " << y << ")\" >\n";
	for( size_t i=0; i < n; i++){
		int X = 0;
		int Y = i * font_size;
		svg << "\t\t\t<text x=\"" << X << "\" y=\"" << Y << "\">" << values[i] << "</text>\n";
	}
	svg << "\t\t</g>\n";
}

template <>
void heatmap_t<bool>::print_cells(size_t x, size_t y ){
/*
	svg << "\t\t<g id=\"cells\" text-anchor=\"start\"  transform=\"translate(" << x << " " << y << ")\" >\n";
	for( auto &line : *ptr ){ // insert the two axes into a map
		size_t x = x_axis[ std::get<0>(line) ];
		size_t y = y_axis[ std::get<1>(line) ];
		bool value  = std::get<2>(line);

		std::string color = value ? "#BBC42A" : "#FF4500";

		svg << "\t\t\t<rect rx=\"2\" ry=\"2\" width=\"" << 0.9 * 1.0 / cos_90 * font_size <<"\" height=\"" << 0.9 * font_size << "\" x=\""
			<< x * 1.0 / cos_90 * font_size << "\" y=\"" <<  y * font_size << "\""
			" fill=\"" << color << "\""
		   <<" />\n";
	}
	svg << "\t\t</g>\n";
	*/
}
template <>
void heatmap_t<result_t>::print_cells(size_t x, size_t y ){
/*	svg << "\t\t<g id=\"cells\" text-anchor=\"start\"  transform=\"translate(" << x << " " << y << ")\" >\n";
	for( auto &line : *ptr ){ // insert the two axes into a map
		size_t x = x_axis[ std::get<0>(line) ];
		size_t y = y_axis[ std::get<1>(line) ];
		result_t value  = std::get<2>(line);

		std::string color;

		switch ( value ){
			case result_t::failure: color = "#FF4500"; break;
			case result_t::success: color = "#BBC42A"; break;
			case result_t::not_executed: ;break;
			default:
				;
		}
		if( value != result_t::not_executed ) {
			svg << "\t\t\t<rect rx=\"2\" ry=\"2\" width=\"" << 0.9 * 1.0 / cos_90 * font_size <<"\" height=\"" << 0.9 * font_size << "\" x=\""
				<< x * 1.0 / cos_90 * font_size << "\" y=\"" <<  y * font_size << "\""
				" fill=\"" << color << "\""
			   <<" />\n";
		}
	}
	svg << "\t\t</g>\n";
	*/
}

template <class T>
void heatmap_t<T>::print_cells(size_t x, size_t y ){

}


template <class T>
size_t heatmap_t<T>::insert(size_t& idx, map_t& map,  const std::string& label ){
	auto search = map.find( label );
	if(search == map.end())
		map.emplace(label, idx++);
	return idx;
}

template <class T>
size_t heatmap_t<T>::insert_x(size_t& idx, const std::string& label){
	return insert(idx, x_axis, label);
}

template <class T>
size_t heatmap_t<T>::insert_y(size_t& idx, const std::string& label){
	return insert(idx, y_axis, label);
}

template <class T>
std::string heatmap_t<T>::html_escape( const std::string& data ) {
	std::string buffer;
	buffer.reserve(data.size());
	for(size_t pos = 0; pos != data.size(); ++pos) {
		switch(data[pos]) {
			case '&':  buffer.append("&amp;");       break;
			case '\"': buffer.append("&quot;");      break;
			case '\'': buffer.append("&apos;");      break;
			case '<':  buffer.append("&lt;");        break;
			case '>':  buffer.append("&gt;");        break;
			default:   buffer.append(&data[pos], 1); break;
		}
	}
	return buffer;
}

template <class T>
std::vector<std::string> heatmap_t<T>::values_by_idx( const map_t& src ){
	std::vector<std::string> dst(src.size());
	for (const auto &it : src)
		dst[it.second] = html_escape( it.first );
	return dst;
}

template <class T>
size_t heatmap_t<T>::size_of_max_value( const map_t& map ){
	auto it = std::max_element( std::begin(map), std::end(map),
			[](const auto& a, const auto& b) -> bool {
				return a.first.size() < b.first.size();
	});
	return it->first.size();
}
#endif
