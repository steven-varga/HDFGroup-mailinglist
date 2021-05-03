#include <hdf5.h>
#include <h5cpp/core>
#include <vector>
#include <string>
#include <functional>
#include <tuple>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <stdint.h>
#include "meta.hpp"

namespace h5::bench {
    struct test_t {
        using max_lenght = std::integral_constant<size_t,40>;
        uint16_t warmup;
        uint16_t iteration;

        uint64_t min_runtime;
        uint64_t avg_runtime;
        uint64_t max_runtime;

        double min_throughput;
        double avg_throughput;
        double max_throughput;

        char name[max_lenght::value];
    };
}

namespace h5 {
    //template specialization of shim::pod_t to create HDF5 COMPOUND type
    template<> hid_t inline register_struct<h5::bench::test_t>(){
        hid_t name_t = H5Tcopy(H5T_C_S1);
        H5Tset_size(name_t, H5T_VARIABLE);

        hid_t ct_00 = H5Tcreate(H5T_COMPOUND, sizeof (shim::pod_t));
        H5Tinsert(ct_00, "warmup",	    HOFFSET(h5::bench::test_t,warmup), H5T_NATIVE_USHORT);
        H5Tinsert(ct_00, "iteration",	HOFFSET(h5::bench::test_t,iteration), H5T_NATIVE_USHORT);
        
        H5Tinsert(ct_00, "min_runtime",	HOFFSET(h5::bench::test_t,min_runtime), H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "avg_runtime",	HOFFSET(h5::bench::test_t,avg_runtime), H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "max_runtime",	HOFFSET(h5::bench::test_t,max_runtime), H5T_NATIVE_DOUBLE);

        H5Tinsert(ct_00, "min_throughput",	HOFFSET(h5::bench::test_t,min_throughput), H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "avg_throughput",	HOFFSET(h5::bench::test_t,avg_throughput), H5T_NATIVE_DOUBLE);
        H5Tinsert(ct_00, "max_throughput",	HOFFSET(h5::bench::test_t,max_throughput), H5T_NATIVE_DOUBLE);

        H5Tinsert(ct_00, "name", HOFFSET(shim::pod_t,name), name_t);

        H5Tclose(name_t); 
        return ct_00;
    };
}
H5CPP_REGISTER_STRUCT(h5::bench::test_t);

namespace h5::bench::impl::tag {
    struct name_t {};
    struct warmup_t {};
    struct sample_t {};
    struct x_t {};
    struct y_t {};
    struct z_t {};

}

namespace h5::bench::impl {

    template <class T, class tag_t>
    struct value_t {
        using value_type = tag_t;
        T value;
    };

    template <typename T, int N>
    struct array_t {
        using value_type = T;
        array_t(const std::initializer_list<size_t>& list ) : rank(0){
            for(size_t v: list) value[rank] = v, rank++;
        }
		array_t() : rank(0){};
		array_t( array_t&& arg ) = default;
		array_t( array_t& arg ) = default;
		array_t& operator=( array_t&& arg ) = default;
		array_t& operator=( array_t& arg ) = default;
		
		size_t& operator[](size_t i){ return *(value + i); }
		const size_t& operator[](size_t i) const { return *(value + i); }
		size_t* operator*() { return value; }
		const size_t* operator*() const { return value; }

		size_t size() const { return rank; }
		const size_t* begin()const { return value; }
		size_t* begin() { return value; }
        size_t* end() { return value+rank; }


        size_t value[N];
        size_t rank;
    };
}

namespace h5::bench {
    constexpr size_t max_dims = 32;
    namespace arg = h5::meta::arg;

    using name = impl::value_t<std::string, impl::tag::name_t>;
    using warmup = impl::value_t<unsigned, impl::tag::warmup_t>;
    using sample = impl::value_t<unsigned, impl::tag::sample_t>;
    using arg_x = impl::array_t<impl::tag::x_t, max_dims>;
    using arg_y = impl::array_t<impl::tag::y_t ,max_dims>;
    using arg_z = impl::array_t<impl::tag::z_t, max_dims>;

    struct store_t {

        store_t(store_t const&)   = delete;
        void operator=(store_t const&)  = delete;
        static store_t& get() {
            static store_t instance;
            return instance;
        }
        
        void push(const std::string& name, long double, long double, long double) {

        }

        private:
            store_t() {}
            ~store_t(){
                std::cout << "terminating ... \n";
            }
            std::vector<std::tuple<std::string, long double, long double, long double>> list;
            unsigned warmup, iteration;
    };


    template <class... args_t>
    static void throughput(args_t... args) {

        using callback_t = std::function<long double(size_t, size_t)>;

        using name_t = typename arg::tpos<impl::tag::name_t, args_t...>;
        using x_t = typename arg::tpos<impl::tag::x_t, args_t...>;
        using y_t = typename arg::tpos<impl::tag::y_t, args_t...>;
        using z_t = typename arg::tpos<impl::tag::z_t, args_t...>;
        using warmup_t = typename arg::tpos<impl::tag::warmup_t, args_t...>;
        using sample_t = typename arg::tpos<impl::tag::sample_t, args_t...>;
        using callback_f = typename arg::tpos<callback_t, args_t...>;
        
        static_assert( name_t::present, "benchmark name must be specified..." );

        auto tuple = std::forward_as_tuple(args...);
        store_t& store = store_t::get();
        
        warmup wu{3}; sample su{20};
        arg_x x; arg_y y; arg_z z;
        if constexpr(x_t::present) x = std::get<x_t::position>(tuple);
        if constexpr(y_t::present) y = std::get<y_t::position>(tuple);
        if constexpr(z_t::present) z = std::get<z_t::position>(tuple);
        if constexpr(warmup_t::present) wu = std::get<warmup_t::position>(tuple);
        if constexpr(sample_t::present) su = std::get<sample_t::position>(tuple);
        std::vector<long double> time, throughput;

        if constexpr( callback_f::present ){
            callback_t fn = std::get<callback_f::position>(tuple);
            // warm up phase
            for(size_t j=0; j<x.rank; j++){
                for(size_t i=0; i<wu.value; i++) auto ignore = fn( j, x[j] );
                time.clear(); throughput.clear();
                for(size_t i=0; i<su.value; i++) {
                    namespace cl = std::chrono;
                    const auto start = cl::steady_clock::now();
                        long double transfer_size = fn(j, x[j]);
                    const auto end = cl::steady_clock::now();
                    const auto delta = static_cast<long double>(
                        cl::duration_cast<cl::nanoseconds>(end - start).count());
                    time.push_back(delta);
                    // time is in nano seconds,  
                    throughput.push_back(transfer_size /(delta /1'000.0)); 
                }
                auto avg_tp = std::accumulate(throughput.begin(), throughput.end(), 0) / throughput.size();
                std::cout << " count: " << throughput.size() << " " << avg_tp   << " MiB/s\n";
            }


            store.push("hello", 0.0, 0.0, 0.0);
        } 
    }

}

