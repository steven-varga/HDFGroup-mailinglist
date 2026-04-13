/* This file is part of the H5CPP project and is licensed under the MIT License.
 * 
 * Copyright © 2018–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
 * Contact: info@vargaconsulting.ca */

#pragma once
#include <hdf5.h>
#include <armadillo>
#include <cstdlib>
#include <iterator> 
#include <numeric>
#include <ostream>
#include <ranges>
#include <memory>
#include <stdexcept>
#include "H5Dappend.hpp"
#include "H5Eall.hpp"
#include "H5Sall.hpp"
#include "H5Zpipeline.hpp"
#include "H5capi.hpp"
#include "H5misc.hpp"

namespace h5 {
    namespace impl {
        template<typename T> struct iterator_t;
    }
    template<typename T>
    std::ranges::subrange<impl::iterator_t<T>, impl::iterator_t<T>> view(h5::ds_t ds);
}

namespace h5::impl {
    [[nodiscard]] inline hsize_t get_total_elements(h5::ds_t ds){
		hsize_t current_dims[H5CPP_MAX_RANK];
		h5::sp_t file_space = h5::get_space( ds );
		hsize_t rank = h5::get_simple_extent_dims(file_space, current_dims, nullptr );
        hsize_t total = std::accumulate(current_dims, current_dims + rank, 1, std::multiplies<hsize_t>());
        if(rank != 1) THROW_RUNTIME_ERROR("Only rank 1 objects are supported...");
        return total;
    }

    template<typename T>
    struct iterator_t {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using pointer = const T*;
        using reference = const T&;
        using pipeline_t = impl::pipeline_t<impl::basic_pipeline_t>;

        iterator_t() : ds{H5I_UNINIT}, offset{0}, total{0},
            current_dims{0}, chunk_dims{0}, filter{H5Z_FILTER_ERROR}{}
            
        reference operator*() const { return cache[offset % chunk_dims]; }
        pointer operator->() const { return &cache[offset % chunk_dims]; }

        iterator_t& operator++() {
            ++offset;
            if(offset % chunk_dims == 0 )
                read_chunk();
            return *this;
        }

        iterator_t operator++(int) {
            iterator_t tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator_t& a, const iterator_t& b) {
            return a.offset == b.offset;
        }

        friend bool operator!=(const iterator_t& a, const iterator_t& b) {
            return !(a == b);
        }

        template<typename U>
        friend std::ranges::subrange<impl::iterator_t<U>, impl::iterator_t<U>> h5::view(h5::ds_t);

    private:
        iterator_t(h5::ds_t ds, size_t offset)
            : ds{ds}, offset{offset}, total{ get_total_elements(ds)},filter(H5Z_FILTER_NONE) {
            if(offset >= total) return;
            h5::sp_t file_space = h5::get_space( ds );
            int rank = H5Sget_simple_extent_ndims(file_space);
            if(rank != 1) THROW_RUNTIME_ERROR("Only rank 1 streams are supported...");
            H5Sget_simple_extent_dims(file_space, &current_dims, nullptr);
            h5::dcpl_t dcpl = h5::get_dcpl( ds );
            H5D_layout_t layout = H5Pget_layout(dcpl);
            if(layout != H5D_CHUNKED) THROW_RUNTIME_ERROR("only chunked dataset are supported...");
            H5Pget_chunk(dcpl, 1, &chunk_dims);
            cache.resize(chunk_dims);

            unsigned filter_config, flags, N = H5Pget_nfilters( dcpl );
            size_t cd_nelmts = 1;
            if(N == 1){ H5CPP_CHECK_NZ(
                filter = H5Pget_filter2(dcpl, 0, &flags, &cd_nelmts, cd_values, 0, nullptr, &filter_config), std::runtime_error, "could not read filter");
                TRACE << "filter: " << filter << " config: " << filter_config << " n elems:" << cd_nelmts << std::endl;
                filter = H5Z_FILTER_DEFLATE;
            } else if (N > 1) THROW_RUNTIME_ERROR("for performance reasons only a single filter is allowed...");
            if(filter != H5Z_FILTER_DEFLATE && filter != H5Z_FILTER_NONE)
                THROW_RUNTIME_ERROR("unsupported filter: only gzip-compressed or uncompressed data is supported...");
            else chunk.resize(chunk_dims);
            TRACE << "offset: " << offset << " chunk: " << chunk_dims << " size: " << total << " N:" << N << " " << filter << std::endl;
            if(offset < total && offset % chunk_dims == 0)
                read_chunk();  // preload first chunk or any aligned offset
        }        
        void read_chunk() {
            uint32_t filter_mask;
            size_t nbytes = chunk_dims * sizeof(T), compressed_chunk_size;
            switch(filter){
                case H5Z_FILTER_NONE: H5CPP_CHECK_NZ(
                    H5Dread_chunk(ds, H5P_DEFAULT, &offset, &filter_mask, cache.data()), std::runtime_error, h5::error::msg::read_dataset);
                break;
                case H5Z_FILTER_DEFLATE: H5CPP_CHECK_NZ(
                        H5Dget_chunk_storage_size(ds, &offset, &compressed_chunk_size), std::runtime_error, "failed to get compressed chunk size");
                    H5CPP_CHECK_NZ(
                        H5Dread_chunk(ds, H5P_DEFAULT, &offset, &filter_mask, chunk.data()), std::runtime_error, h5::error::msg::read_dataset);
                    if(uncompress(reinterpret_cast<Bytef*>(cache.data()), &nbytes, reinterpret_cast<const Bytef*>(chunk.data()), compressed_chunk_size) != Z_OK)
                        THROW_RUNTIME_ERROR("decompression has failed");

                break;
                default: THROW_RUNTIME_ERROR("not supported filter chain, contact vendor");
            }
        }

    private:
        h5::ds_t ds;
        hsize_t offset, total,
            current_dims, chunk_dims;
        unsigned cd_values[16];
        std::vector<T> cache, chunk;
        H5Z_filter_t filter;
    };
} // namespace h5::impl

namespace h5 {
    template<typename T>
    [[nodiscard]] std::ranges::subrange<impl::iterator_t<T>, impl::iterator_t<T>> view(h5::ds_t ds) {
        hsize_t total = get_total_elements(ds);
        using iterator = h5::impl::iterator_t<T>;
        return std::ranges::subrange<iterator, iterator> {
            iterator(ds, 0), iterator(ds, total)};
    }
}

