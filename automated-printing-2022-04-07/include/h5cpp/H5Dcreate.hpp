/*
 * Copyright (c) 2018 - 2021 vargaconsulting, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */

#ifndef  H5CPP_DCREATE_HPP 
#define  H5CPP_DCREATE_HPP

#include <hdf5.h>
#include "H5config.hpp"
#include "H5Eall.hpp"
#include "H5Iall.hpp"
#include "H5meta.hpp"
#include "H5Sall.hpp"
#include "H5Tall.hpp"
#include "H5Tmeta.hpp"
#include "H5capi.hpp"
#include "H5Fopen.hpp"
#include "H5Dopen.hpp"
#include <type_traits>
#include <string>
#include <stdexcept>
#include <utility>
#include <limits>

//TODO: if constexpr(..){} >= c++17
namespace h5 {
 /** @ingroup io-create
 	*  @brief creates a dataset within  an already opened HDF5 container
	* By default the HDF5 dataset size, the file space, is derived from the passed object properties, or may be explicitly specified
	* with optional properties such as h5::count, h5::current_dims h5::max_dims, h5::stride, h5::block 
	* @param fd valid h5::fd_t descriptor
	* @param dataset_path where the dataset is, or will be created within the HDF5 file
    * @param args[, ...] comma separated list of arguments in arbitrary order, only `T object` | `const T*` is required 
	* @return h5::ds_t  a valid, RAII enabled handle, binary compatible with HDF5 CAP `hid_t` 
	* 
	* @tparam T C++ type of dataset being written into HDF5 container
	*
	* <br/>The following arguments are context sensitive, may be passed in arbitrary order and with the exception
	* of `const ref&` or `const T*` object being saved/memory region pointed to, the arguments are optional. By default the arguments are set to sensible values,
	* and in most cases the function call will deliver good performance. With that in mind, the options below provide an easy to use high level fine
	* tuning mechanism to get the best experience without calling HDF5 CAPI functions directly. 
    *
	* @param h5::current_dims_t optionaly  defines the size of the dataset, applicable only to datasets which has to be created
	* When omitted, the system computes the default value as follows  `h5::block{..}` and `h5::stride{..}` given as:
	* 		`current_dims[i] = count[i] (stride[i] - block[i] + 1) + offset[i]` and when only `h5::count` is available 
	*       `current_dims[i] = count[i] + offset[i]`
	* @param h5::max_dims_t  optional maximum size of dataset, applicable only to datasets which has to be created `max_dims[i] >= current_dims[i]`
	* or `H5S_UNLIMITED` along the dimension intended to be extendable
	* @param h5::dcpl_t data creation property list, used only if dataset needs to be created
	* @param h5::lcpl_t link control property list, controls how path is created when applicabl
	* <br/><b>example:</b>
	* @code
	* auto ds = h5::create<short>("file.h5","path/to/dataset", 
	*	h5::current_dims{10,20}, h5::max_dims{10,H5S_UNLIMITED},
	*	h5::create_path | h5::utf8, // optional lcpl with this default settings**
	*	h5::chunk{2,3} | h5::fill_value<short>{42} | h5::fletcher32 | h5::shuffle | h5::nbit | h5::gzip{9})
	* @endcode 
 	*/
    template<class T, class L, class... args_t>
    inline typename std::enable_if< impl::is_location<L>::value,
    h5::ds_t>::type create( const L& loc, const std::string& dataset_path, args_t&&... args ) try {
        // compile time check of property lists: 
        using tcurrent_dims = typename arg::tpos<const h5::current_dims_t&,const args_t&...>;
        using tmax_dims     = typename arg::tpos<const h5::max_dims_t&,const args_t&...>;
        using tlcpl         = typename arg::tpos<const h5::lcpl_t&,const args_t&...>;
        using tdcpl         = typename arg::tpos<const h5::dcpl_t&,const args_t&...>;
        using tdapl         = typename arg::tpos<const h5::dapl_t&,const args_t&...>;
        using ttype         = typename arg::tpos<const h5::dt_t<T>&,const args_t&...>;

        //TODO: make copy of default dcpl
        h5::dcpl_t default_dcpl{ H5Pcreate(H5P_DATASET_CREATE) };
        // get references to property lists or system default values 
        const h5::lcpl_t& lcpl = arg::get(h5::default_lcpl, args...);
        h5::dcpl_t dcpl = arg::get(default_dcpl, args...);
        const h5::dapl_t& dapl = arg::get(h5::default_dapl, args...);

        H5CPP_CHECK_PROP( lcpl, h5::error::property_list::misc, "invalid list control property" );
        H5CPP_CHECK_PROP( dcpl, h5::error::property_list::misc, "invalid data control property" );
        H5CPP_CHECK_PROP( dapl, h5::error::property_list::misc, "invalid data access property" );
        // and dimensions
        h5::current_dims_t current_dims_default{0}; // if no current dims_present 
        // this mutable value will be referenced
        const h5::current_dims_t& current_dims = arg::get(current_dims_default, args...);
        const h5::max_dims_t& max_dims = arg::get(h5::max_dims_t{0}, args... );
        h5::sp_t space_id{H5I_UNINIT}; // set to invalid state 
        h5::ds_t ds{H5I_UNINIT};

        // accounting
        bool is_equal_dims = true, is_unlimited = false,
            is_extendable = true, is_filtering_on = false;
        hsize_t total_space = sizeof(typename meta::decay<T>::type);

        if constexpr( tmax_dims::present ) {
            // check if `current_dims` present as well, if not, copy `max_dims` into `current_dims`
            // such ranks marked as `H5S_UNLIMITED` will have `current_dims` marked as `0` 
            size_t rank = max_dims.size();
            if constexpr( !tcurrent_dims::present ) {
                // set current dimensions to given one or zero if H5S_UNLIMITED mimics matlab(tm) behavior while allowing extendable matrices
                for(hsize_t i=0; i<rank; i++){
                    current_dims_default[i] = max_dims[i] != H5S_UNLIMITED
                        ? max_dims[i] :  static_cast<hsize_t>(0);
                    // taint compact | contiguous layout if unlimited
                    if( !is_unlimited && max_dims[i] == H5S_UNLIMITED ) is_unlimited = true;
                }
                current_dims_default.rank = rank;
            } else { // both dimensions are provided, check if match, unlimited 
                for(hsize_t i=0; i<rank; i++) {
                    if( is_equal_dims && max_dims[i] != current_dims[i] ) is_equal_dims = false;
                    if( !is_unlimited && max_dims[i] == H5S_UNLIMITED ) is_unlimited = true;
                    total_space *= max_dims[i];
                }
            }
            // at this point we have valid `current_dims` and `max_dims` to describe file space
            space_id = std::move( h5::create_simple( current_dims, max_dims ) );
        } else if (tcurrent_dims::present) { // previously we checked max_dims + current_dims, therefore 
            // only `current_dims` is present, dataset will have same `max_dims` and is not extendable
            for(hsize_t i=0; i<current_dims.rank; i++)
                total_space *= current_dims[i];
            space_id = std::move( h5::create_simple( current_dims ) );
        } else  // no dimensions are provided, we are dealing with a scalar
            space_id = std::move(h5::sp_t{H5Screate(H5S_SCALAR)});

        //LAYOUT: by default we use `contiguous` layout, then alter it if needed
        is_extendable = is_unlimited || !is_equal_dims;
        if constexpr ( tdcpl::present ) is_filtering_on = H5Pget_nfilters(dcpl) > 0;

        // at this point we have all the information to decide if compact dataset
        if( !is_filtering_on && !is_extendable && total_space < H5CPP_COMPACT_PAYLOAD_MAX_SIZE )
            set_compact_layout(default_dcpl);
        if( !tdcpl::present && ( is_unlimited || (tcurrent_dims::present && tmax_dims::present)) ) {
            chunk_t chunk{0}; chunk.rank = current_dims.rank;
            for(hsize_t i=0; i<current_dims.rank; i++)
                chunk[i] = current_dims[i] ? current_dims[i] : 1;
            h5::set_chunk(default_dcpl, chunk );
        }
        // if type is specified, let's use it, otherwise execute callback: h5::create
        using element_t = typename meta::decay<T>::type;
        h5::dt_t<T> type; ;
        if constexpr( ttype::present )
            type = std::move(arg::get(dt_t<T>(), args...));
        else
            type = create<T>();
        return h5::createds(loc, dataset_path, type, space_id, lcpl, dcpl, dapl);

    } catch( const std::runtime_error& err ) {
            throw h5::error::io::dataset::create( err.what() );
    }

  /** @ingroup io-create
 	*  @brief creates a dataset within  an HDF5 container opened with flag `H5F_ACC_RDWR`
	* By default the HDF5 dataset size, the file space, is derived from the passed object properties, or may be explicitly specified
	* with optional properties such as h5::count, h5::current_dims h5::max_dims, h5::stride, h5::block 
	* @param file_path path the the HDF5 file
	* @param dataset_path where the dataset is, or will be created within the HDF5 file
    * @param args[, ...] comma separated list of arguments in arbitrary order, only `T object` | `const T*` is required 
	* @return h5::ds_t  a valid, RAII enabled handle, binary compatible with HDF5 CAP `hid_t` 
	* 
	* @tparam T C++ type of dataset being written into HDF5 container
	*
	* <br/>The following arguments are context sensitive, may be passed in arbitrary order and with the exception
	* of `const ref&` or `const T*` object being saved/memory region pointed to, the arguments are optional. By default the arguments are set to sensible values,
	* and in most cases the function call will deliver good performance. With that in mind, the options below provide an easy to use high level fine
	* tuning mechanism to get the best experience without calling HDF5 CAPI functions directly. 
    *
	* @param h5::current_dims_t optionaly  defines the size of the dataset, applicable only to datasets which has to be created
	* When omitted, the system computes the default value as follows  `h5::block{..}` and `h5::stride{..}` given as:
	* 		`current_dims[i] = count[i] (stride[i] - block[i] + 1) + offset[i]` and when only `h5::count` is available 
	*       `current_dims[i] = count[i] + offset[i]`
	* @param h5::max_dims_t  optional maximum size of dataset, applicable only to datasets which has to be created `max_dims[i] >= current_dims[i]`
	* or `H5S_UNLIMITED` along the dimension intended to be extendable
	* @param h5::dcpl_t data creation property list, used only if dataset needs to be created
	* @param h5::lcpl_t link control property list, controls how path is created when applicabl
	* <br/><b>example:</b>
	* @code
	* auto ds = h5::create<short>("file.h5","path/to/dataset", 
	*	h5::current_dims{10,20}, h5::max_dims{10,H5S_UNLIMITED},
	*	h5::create_path | h5::utf8, // optional lcpl with this default settings**
	*	h5::chunk{2,3} | h5::fill_value<short>{42} | h5::fletcher32 | h5::shuffle | h5::nbit | h5::gzip{9})
	* @endcode 
 	*/
    template<class T, class... args_t>
    inline h5::ds_t create( const std::string& file_path, const std::string& dataset_path, args_t&&... args ){
        h5::fd_t fd = h5::open(file_path, H5F_ACC_RDWR, h5::default_fapl);
        return h5::create<T>(fd, dataset_path, args...);
    }
}

namespace h5::impl {
   /** @ingroup internals
    * @brief opens dataset if exists, otherwise create a new one
    * considering template paramter `file_t` and `mem_t` when a dataset doesn't exist, this routine either directly delegates 
    * to `h5::create<file_t>(...)` or derives the file type and dataspace from the object reference and the optional 
    * arguments such as `h5::current_dims{..}`, `h5::count{}`, `h5::max_dims{}`, `h5::offset{}` and strinding.  
    * @return h5::ds_t dataset valid descriptor 
    */ 
    template <class file_t, class mem_t, class... args_t>
	inline h5::ds_t open_or_create( const h5::fd_t& fd, const std::string& dataset_path, const mem_t& ref,  args_t&&... args  ){
		using tcount = typename arg::tpos<const h5::count_t&, const args_t&...>;
		using tcurrent_dims = typename arg::tpos<const h5::current_dims_t&, const args_t&...>;
		using tmax_dims  = typename arg::tpos<const h5::max_dims_t&, const args_t&...>;
		using toffset = typename arg::tpos<const h5::offset_t&, const args_t&...>;

		// find out if we have to create the dataset
		h5::mute(); 
			// Returns a negative value when the function fails and may return a negative value if the link does not exist.
			// - name is not local to the group specified by loc_id or, if loc_id is something other than a group identifier, 
			//        name is not local to the root group
			// - Any element of the relative path or absolute path in name, except the target link, does not exist.
			bool is_dataset_present = H5Lexists(fd, dataset_path.c_str(), h5::default_lapl) > 0;
		h5::unmute(); // <- make sure not to mute error handling longer than needed
		if (is_dataset_present)  // we have dataset, open and return a valid descriptor 
			return h5::open(fd, dataset_path, h5::arg::get(h5::default_dapl, args...));

		h5::ds_t ds; // initialized to H5I_UNINIT

        // syntax check: 
        static_assert( std::is_array<file_t>::value || !std::is_pointer<mem_t>::value || tcount::present, 
            "h5::count_t{ ... } must be provided to describe T* memory region" );
        //static_assert( !tcurrent_dims::present || toffset::present,
        //    "when h5::current_dims{..} specified, you must provide h5::offset{..} as well!!! ");
        if (!meta::is_undefined<file_t>::value)
            return h5::create<file_t>(fd, dataset_path, args...);

        if constexpr(tcurrent_dims::present || meta::is_array<mem_t>::value || std::is_pod<mem_t>::value) // user
            ds = h5::create<mem_t>(fd, dataset_path, args...);
        else if constexpr (meta::is_linalg<mem_t>::value) // all linear algebra libraries
            ds = h5::create<mem_t>(fd, dataset_path, get_current_dims(h5::count_t{meta::size(ref)}, args...), args...);
        else if constexpr (meta::is_stl<mem_t>::value) { // the object has:  data(), begin(), end(), ::value_type
            static_assert( meta::has_size<mem_t>::value || tcount::present, 
                "std::forward_list<T> and alike has no ::size() method, must provide h5::count{..}!!!");
            if constexpr (meta::has_size<mem_t>::value)
                ds = h5::create<mem_t>(fd, dataset_path, get_current_dims(h5::count_t{ref.size()}, args...), args...);
                //ds = h5::create<mem_t>(fd, dataset_path, h5::current_dims{ref.size()}, args...);
            else {
                h5::current_dims_t current_dims = static_cast<h5::current_dims_t>(
                    std::get<tcount::value>(std::forward_as_tuple(args...))); 
                ds = h5::create<mem_t>(fd, dataset_path, current_dims, args...);
            }
        } else if constexpr( std::is_pointer<mem_t>::value ){ // scalar, arrays, pointers 
            std::cout <<"pointer!!!\n\n";
        } 
		return ds;
	}
}
#endif
