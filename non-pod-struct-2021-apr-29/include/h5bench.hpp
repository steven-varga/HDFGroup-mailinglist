

#define FMT_HEADER_ONLY

#include <hdf5.h>
#include <h5cpp/all>
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
#include "struct.hpp"
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/color.h>


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

    template <class T>
    std::pair<T,T> stats(const std::vector<T>& data){
        T mean = std::accumulate(data.begin(), data.end(), 0) / (double)data.size();
        T var  = std::accumulate(data.begin(), data.end(), 0, [&](const T&s, const T&v ) -> T {
            T m = (v-mean);
            return s + m*m;
        }) / (double)data.size();
        T std = sqrt(var);
        return std::pair<T,T>{mean, std};
    }
}

namespace h5::bench {
    constexpr size_t max_dims = 32;
    namespace arg = h5::meta::arg;
    static void init(h5::fd_t fd, std::string report);

    using name = impl::value_t<std::string, impl::tag::name_t>;
    using warmup = impl::value_t<uint16_t, impl::tag::warmup_t>;
    using sample = impl::value_t<uint16_t, impl::tag::sample_t>;
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
        
        void push(const test_t& data) {
           this->list.push_back(data); 
        }
        template<class V, class T>
        void push(const std::string& name, uint64_t x, V warmup, V iteration, const std::pair<T,T>& time, const std::pair<T,T>& tp ) {
            test_t item {
                .warmup = warmup,
                .iteration = iteration,
                .x = x,
                .mean_runtime = time.first,
                .std_runtime = time.second,
                .mean_throughput = tp.first,
                .std_throughput = tp.second 
            };
            strncpy(item.name, name.data(), test_t::max_lenght::value);
            list.push_back(item);
        }
        friend  void init(std::string, std::string);
        private:
            store_t() : file_name("benchmark.h5"), dataset_name("/report"){
            }
            ~store_t(){
                fmt::print("[name                                              ][total events][Mi events/s] [ms runtime / stddev] [    MiB/s / stddev ]\n");

                for(auto i: list)
                    fmt::print("{:<52} {:>12} {:>12.4f}  {:>11.2f}  {:>8.3f} {:>10.2f}   {:>7.1f}\n",
                    i.name, i.x, i.x / i.mean_runtime, i.mean_runtime/1000.0, i.std_runtime/1000.0, i.mean_throughput, i.std_throughput);
            }
            std::string file_name, dataset_name;
            std::vector<test_t> list;
            unsigned warmup, iteration;
    };

    void init(std::string file_name, std::string dataset_name){
        store_t& store = store_t::get();
        store.file_name = file_name;
        store.dataset_name = dataset_name;
    }

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
        using pt_t = typename arg::tpos<pt_t, args_t...>;

        static_assert( name_t::present, "benchmark name must be specified..." );

        auto tuple = std::forward_as_tuple(args...);
        store_t& store = store_t::get();
        
        warmup wu{3}; sample su{20}; name name = std::get<name_t::position>(tuple);
        arg_x x; arg_y y; arg_z z;
        if constexpr(x_t::present) x = std::get<x_t::position>(tuple);
        if constexpr(y_t::present) y = std::get<y_t::position>(tuple);
        if constexpr(z_t::present) z = std::get<z_t::position>(tuple);
        if constexpr(warmup_t::present) wu = std::get<warmup_t::position>(tuple);
        if constexpr(sample_t::present) su = std::get<sample_t::position>(tuple);
        std::vector<double> time, throughput;

        if constexpr( callback_f::present ){
            callback_t fn = std::get<callback_f::position>(tuple);
            // warm up phase
            for(size_t j=0; j<x.rank; j++){
                for(size_t i=0; i<wu.value; i++) auto ignore = fn( j, x[j] );
                time.clear(); throughput.clear();
                for(size_t i=0; i<su.value; i++) {
                    namespace cl = std::chrono;
                    if constexpr (pt_t::present){
                        h5::pt_t pt = std::get<pt_t::position>(tuple);
                        h5::flush(pt);
                    }
                    const auto start = cl::steady_clock::now();
                        double transfer_size = fn(j, x[j]);
                        if constexpr (pt_t::present){
                            h5::pt_t pt = std::get<pt_t::position>(tuple);
                            h5::flush(pt);
                        }
                    const auto end = cl::steady_clock::now();
                    // time is in microsec, transfer_size in bytes -> thoughput in MiB/sec
                    const auto delta = static_cast<double>(
                        cl::duration_cast<cl::nanoseconds>(end - start).count()) / 1'000.0d;
                    time.push_back(delta);
                    // time is in nano seconds,  
                    throughput.push_back(transfer_size /delta); 
                }
                auto tp = impl::stats(throughput);
                auto rt = impl::stats(time);
                store.push(name.value, x[j], wu.value, su.value, rt, tp);
            }
        } 
    }

}

