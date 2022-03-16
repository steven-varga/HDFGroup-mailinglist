#ifndef H5CPP_UTILS_LOCAL
#define H5CPP_UTILS_LOCAL

#include <random>

namespace h5::utils {
    std::string get_random_string(size_t min, size_t max){
		static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz"
										"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::random_device rd;
		std::default_random_engine rng(rd());
		std::uniform_int_distribution<> dist(0,sizeof(alphabet)/sizeof(*alphabet)-2);
		std::uniform_int_distribution<> string_length(min, max);

        std::string str;
        size_t N = string_length(rng);
        str.reserve(N);
        std::generate_n(std::back_inserter(str), N, [&]() {
                        return alphabet[dist(rng)];
                    });
        return str;
    }
}
#endif
