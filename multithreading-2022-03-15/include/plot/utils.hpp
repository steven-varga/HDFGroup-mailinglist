/*
 * Copyright (c) 2019 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#ifndef  PLOT_UTILS_HPP
#define  PLOT_UTILS_HPP
#include <string>

namespace plot::util {
	inline std::string html_escape( const std::string& data ){
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
	};
}
namespace plot::util::details {
    template <typename Base> std::true_type is_base_of_test_func(Base*);
    template <typename Base> std::false_type is_base_of_test_func(void*);
    template <typename Base, typename Derived>
    using pre_is_base_of = decltype(is_base_of_test_func<Base>(std::declval<Derived*>()));
 
    // with <experimental/type_traits>:
    // template <typename Base, typename Derived>
    // using pre_is_base_of2 = std::experimental::detected_or_t<std::true_type, pre_is_base_of, Base, Derived>;
    template <typename Base, typename Derived, typename = void>
    struct pre_is_base_of2 : public std::true_type { };
    // note std::void_t is a C++17 feature
    template <typename Base, typename Derived>
    struct pre_is_base_of2<Base, Derived, std::void_t<pre_is_base_of<Base, Derived>>> :
        public pre_is_base_of<Base, Derived> { };
}

namespace plot::util {
template <typename Base, typename Derived>
struct is_base_of :
    public std::conditional_t<
        std::is_class<Base>::value && std::is_class<Derived>::value,
        details::pre_is_base_of2<Base, Derived>,
        std::false_type
    > { };
}

namespace plot::utils {
	size_t size_of_max_value( const std::vector<std::string>& lbl ){
		auto it = std::max_element( std::begin(lbl), std::end(lbl),
				[](const auto& a, const auto& b) -> bool {
					return a.size() < b.size();
		});
		return it->size();
	}
}
#endif
