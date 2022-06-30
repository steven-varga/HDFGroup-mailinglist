/*
 * Copyright (c) 2018-2020 Steven Varga, Toronto,ON Canada
 * Author: Varga, Steven <steven@vargaconsulting.ca>
 */
#ifndef H5CPP_DWRITE_HPP
#define H5CPP_DWRITE_HPP

#include <hdf5.h>
#include "H5config.hpp"
#include "H5Eall.hpp"
#include "H5Iall.hpp"
#include "H5meta.hpp"
#include "H5Sall.hpp"
#include "H5Tall.hpp"
#include "H5Tmeta.hpp"
#include "H5Tconversion.hpp"
#include "H5Pall.hpp"
#include "H5Zpipeline.hpp"
#include "H5Zpipeline_basic.hpp"
#include "H5Pdapl.hpp"
#include "H5capi.hpp"
#include "H5Fopen.hpp"
#include "H5Dopen.hpp"
//#include "H5Dgather.hpp"
#include <tuple>
#include <type_traits>
#include <string>
#include <stdexcept>
#include <utility>
#include <string.h>

// defines an runtime_exception thrown to track template matching
// for various types;  see tests/H5Dwrite 
#ifndef H5CPP_WRITE_DISPATCH_TEST_
    #define H5CPP_WRITE_DISPATCH_TEST_(type, msg)
#endif
/*
/usr/bin/ld: H5Dwrite.o: in function
T := char, std::char_traits<char>, std::allocator<char> 


 `h5::impl::detail::hid_t<h5::impl::ds_t, &H5Dclose, true, true, 4> 
 h5::write<std::vector<std::__cxx11::basic_string<char, T >,
std::allocator<std::__cxx11::basic_string<T> > >>

(h5::impl::detail::hid_t<h5::impl::ds_t, &H5Dclose, true, true, 4> const&, 
std::vector<std::__cxx11::basic_string<T>, std::allocator<std::__cxx11::basic_string<T> > > const&)':
H5Dwrite.cpp:(
	.text._ZN2h55writeISt6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS7_EEJEEENS_4impl6detail5hid_tINSA_4ds_tEXadL_Z8H5DcloseEELb1ELb1ELi4EEERKSE_RKT_DpOT0_[_ZN2h55writeISt6vectorINSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESaIS7_EEJEEENS_4impl6detail5hid_tINSA_4ds_tEXadL_Z8H5DcloseEELb1ELb1ELi4EEERKSE_RKT_DpOT0_]+0xbe): undefined reference to 
	`h5::impl::detail::hid_t<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, &H5Tclose, true, true, 2> 
	h5::copy<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > >()'

*/
namespace h5 {
  /** @ingroup io-write
	* @brief write the memory content of `const T* ptr` with given `mem_space`  into `file_space` of the dataset
	* @param ds valid h5::ds_t descriptor
	* @param mem_space the dimensions of the memory region being transferred
	* @param file_space the dimensions of the target dataset region `nelems(mem_space) == nelems(file_space)`
	* @param dxpl transfer property list to fine tune IO ops
	* @param ptr `const T*` typed pointer to supported objects or elementary types
	* @tparam T memory_t (as opposed to file_t -- which may be differnt) C++ fundamental numeric or pod types, but not strings 
 	*/ 
	template <class mem_t>
	inline void write( const h5::ds_t& ds, const h5::dt_t<mem_t>& dt, const h5::sp_t& mem_space, const h5::sp_t& file_space, const h5::dxpl_t& dxpl, const mem_t* ptr){
		H5CPP_CHECK_PROP( dxpl, h5::error::io::dataset::write, "invalid data transfer property" );
		H5CPP_CHECK_NZ(
			H5Dwrite( static_cast<hid_t>( ds ), dt, mem_space, file_space, static_cast<hid_t>(dxpl), ptr),
				h5::error::io::dataset::write, h5::error::msg::write_dataset);
	}

   /** @ingroup io-write
 	* @brief writes data, from contiguous memory region  into an existing, opened dataset specified by`h5::ds_t` descriptor 
	* Lower level template with generative programming paradigm constructs an optimal function respec to specified arguments
	* @param ds valid h5::ds_t descriptor
	* @param ptr `const T*` typed pointer to supported objects or elementary types
    * @param args[, ...] comma separated list of arguments in arbitrary order, only `T object` is required 
	* @return h5::ds_t  returns the same `ds` dataset descriptor as the first param 
	* 
	* @tparam T element_t type, the type information of the memory region; which may be different from the dataset type 
	*
	* <br/>The following arguments are context sensitive, may be passed in arbitrary order and with the exception
	* of `const T*` pointing to the memory region being saved, the arguments are optional. By default the arguments are set to sensible values,
	* and in most cases the function call will deliver good performance. With that in mind, the options below provide an easy high level fine
	* tuning mechanism to get the best experience without calling HDF5 CAPI functions directly. 
    *
	*
	* @param h5::count number of element of `element_t`, compiler time error if not specified when `T*` is a pointer. Not the same as HDF5  `count`
	* in hyperslab selection; instead from this value the correct `slab_count` is derived considering the block size whenever applicable with a normalisation step:
    * `slab_count[i] = h5::count[i] / h5::block[i]`
	* @param h5::stride_t determins how many elements to move in each dimension from current position, `stride[i] >= block[i] && stride[i] != 0` 
	* @param h5::block_t only used when `stride` is specified, as `h5::count` otherwise the `slab_block` is derived from `h5::count` or number of elements 
	* @param h5::offset_t writes `T object` starting from this coordinates, considers this value into `h5::current_dims` when applicable
	* @param h5::dcpl_t data control property list
	* @param h5::dxpl_t data transfer property list
	* 
	* <br/><b>example:</b>
	* @code
	* std::vector<int> data = ...
	* //creates an extendable, chunked dataset, with initial size matching of the vector
	* h5::fd_t fd = h5::open("example.h5", H5F_ACC_RDWR);
	* h5::ds_t ds = h5::write(fd, "path/chunked layout dataset", data,
	* 		h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9});
	* //creates a contiguous layout dataset, with size matching `data` 
	* h5::ds_t ds = h5::write(fd, "path/contiguous layout dataset", data);
	* //interop with HDF5 CAPI: will flush dataset `ds` by directly calling libhdf5 function
	* H5Dflush(ds); 
	* @endcode 
 	*/ 
 	namespace impl {
		template <class mem_t, class file_t = undefined_t, class... args_t>
		inline h5::ds_t write( const h5::ds_t& ds, const h5::dt_t<mem_t>& mem_type, const mem_t* ptr,  args_t&&... args) try {
			// element types: pod | [signed|unsigned](int8 | int16 | int32 | int64) | float | double
			using tcount = typename arg::tpos<const h5::count_t&,const args_t&...>;
			using toffset = typename arg::tpos<const h5::offset_t&, const args_t&...>;
			using tstride = typename arg::tpos<const h5::stride_t&, const args_t&...>;
			using tblock = typename arg::tpos<const h5::block_t&, const args_t&...>;
			static_assert( tcount::present,"h5::count_t{ ... } must be provided to describe T* memory region" );
			static_assert( (tstride::present && tblock::present) || (!tblock::present && !tstride::present),
				"stride and block must be both present for strided access pattern..." );
			//static_assert( utils::is_supported<T>, "error: " H5CPP_supported_elementary_types );
			auto tuple = std::forward_as_tuple(args...);
			h5::count_t count = std::get<tcount::value>( tuple );  //we make a copy of it
			const h5::dxpl_t& dxpl = arg::get(h5::default_dxpl, args...); // gets reference to default value if property not present
			h5::sp_t file_space{H5Dget_space( static_cast<::hid_t>(ds) )};
			int rank = h5::get_simple_extent_ndims( file_space );
			::hid_t dapl = h5::get_access_plist( ds );
			::herr_t err = 0; // course error handling does well


			h5::dt_t<file_t> file_type{H5Dget_type(ds)};
			//impl::convert_t<mem_t, file_t> mem(mem_type, file_type, count, dxpl, ptr);

			// we want to optimize for best hyper block selection, in order to do that
			// let's find out what level of control is needed
			if constexpr (toffset::present || tstride::present || tblock::present){
				// HYPERBLOCK selection: we either have the argument in `args...` or using default values
				const h5::block_t& block = arg::get( h5::default_block, args...);
				const h5::offset_t& offset = arg::get( h5::default_offset, args...);
				const h5::stride_t& stride = arg::get( h5::default_stride, args...);
				if constexpr( tblock::present ){ // we have to normalise `count` such that `size[i] = count[i] * block[i]` holds
					for(hsize_t i=0; i < rank; i++) count[i] /= block[i];
					err = H5Sselect_hyperslab(file_space, H5S_SELECT_SET, *offset, *stride, *count, *block);
				} else { // we have to convert h5::count_t{..} to h5::block{..} and initiate a single block transfer
					h5::block_t block_ = static_cast<h5::block_t>(count);
					block_.rank = rank; // memory space may have different rank, be sure to use the rank of file_space
					err = H5Sselect_hyperslab(file_space, H5S_SELECT_SET, *offset, *stride, *h5::default_count, *block_);
				}
			} else // SELECT_ALL this is the fastest approach, mem_space and file_space must match
				err = H5Sselect_all(file_space);
			// throw an exception if any selection error
			H5CPP_CHECK_NZ(err, h5::error::io::dataset::write,	h5::error::msg::select_hyperslab);
			h5::sp_t mem_space = h5::create_simple( count );
			h5::select_all( mem_space );

			H5CPP_CHECK_NZ(
				H5Dwrite(ds, mem_type, mem_space, file_space, dxpl, ptr),
				h5::error::io::dataset::write, h5::error::msg::write_dataset);
			return ds;
		} catch ( const std::exception& err ){
			throw h5::error::io::dataset::write( err.what() );
		}
	}
	
   /** @ingroup io-write
 	*  @brief writes data  within an HDF5 container specified with `h5::fd_t` descriptor 
	* By default the HDF5 dataset size, the file space, is derived from the passed object properties, or may be explicitly specified
	* with optional properties such as h5::count, h5::current_dims h5::max_dims, h5::stride, h5::block <br/>
	* This template specialization acts as a switchboard between objects with <b>contiguous</b> content, such as `std::vector<int>` and
	* objects where the actual content maybe scattered in memory. In the altter case with the help of `h5::gather` operator, and O(n) complexity 
	* this template builds a vector of pointers to actual content, and delegates it to	`h5::write<element_t*>(.., ptr**)` call.
	*
	* @param ds valid h5::ds_t descriptor
	* @param ref `T` typed reference to supported objects or elementary types
    * @param args[, ...] comma separated list of arguments in arbitrary order, only `T object` is required 
	* @return h5::ds_t  a valid, RAII enabled handle, binary compatible with HDF5 CAP `hid_t` 
	* 
	* @tparam T C++ memory type of dataset being written into HDF5 container
	*
	* <br/>The following arguments are context sensitive, may be passed in arbitrary order and with the exception
	* of `const ref&` object being saved, the arguments are optional. By default the arguments are set to sensible values,
	* and in most cases the function call will deliver good performance. With that in mind, the options below provide an easy high level fine
	* tuning mechanism to get the best experience without calling HDF5 CAPI functions directly. 
    *
	*
	* @param h5::current_dims_t optionaly  defines the size of the dataset, applicable only to datasets which has to be created
	* When omitted, the system computes the default value as follows  `h5::block{..}` and `h5::stride{..}` given as:
	* 		`current_dims[i] = count[i] (stride[i] - block[i] + 1) + offset[i]` and when only `h5::count` is available 
	*       `current_dims[i] = count[i] + offset[i]`
	* @param h5::max_dims_t  optional maximum size of dataset, applicable only to datasets which has to be created `max_dims[i] >= current_dims[i]`
	* or `H5S_UNLIMITED` along the dimension intended to be extendable
	* @param h5::count number of element of `element_t`, compiler time error if not specified when `T*` is a pointer. Not the same as HDF5  `count`
	* in hyperslab selection; instead from this value the correct `slab_count` is derived considering the block size whenever applicable with a normalisation step:
    * `slab_count[i] = h5::count[i] / h5::block[i]`
	* @param h5::stride_t determins how many elements to move in each dimension from current position, `stride[i] >= block[i] && stride[i] != 0` 
	* @param h5::block_t only used when `stride` is specified, as `h5::count` otherwise the `slab_block` is derived from `h5::count` or number of elements 
	* @param h5::offset_t writes `T object` starting from this coordinates, considers this value into `h5::current_dims` when applicable	
	* @param h5::stride_t skip this many blocks along given dimension
	* @param h5::block_t only used when `stride` is specified 
	* @param h5::offset_t writes `T object` starting from this coordinates, considers this shift into `h5::current_dims` when applicable
	* @param h5::dcpl_t data creation property list, used only if dataset needs to be created
	* @param h5::dxpl_t data transfer property list, used for IO 
	* @param h5::dapl_t data access property list, how existing dataset is opened  
	* @param h5::lcpl_t link control property list, controls how path is created when applicable	
	* 
	* <br/><b>example:</b>
	* @code
	* std::vector<int> data = ...
	* //creates an extendable, chunked dataset, with initial size matching of the vector
	* h5::fd_t fd = h5::open("example.h5", H5F_ACC_RDWR);
	* h5::ds_t ds = h5::write(fd, "path/chunked layout dataset", data,
	* 		h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9});
	* //creates a contiguous layout dataset, with size matching `data` 
	* h5::ds_t ds = h5::write(fd, "path/contiguous layout dataset", data);
	* //interop with HDF5 CAPI: will flush dataset `ds` by directly calling libhdf5 function
	* H5Dflush(ds); 
	* @endcode 
 	*/
	template <class mem_t, class... args_t>
	inline h5::ds_t write(const h5::ds_t& ds, const mem_t& ref, args_t&&... args) try {
		using tcount = typename arg::tpos<const h5::count_t&,const args_t&...>;

		h5::dt_t<mem_t> mem_type = h5::create<mem_t>();
		h5::dt_t<file_t> file_type{H5Dget_type(ds)};
        if constexpr (meta::is_linalg<mem_t>::value){ // all linear algebra libraries
		//else if constexpr (meta::is_stl<mem_t>::value){ // the object has:  size(), data(), begin(), end(), ::value_type
        } else if constexpr( !meta::is_contiguous<mem_t>::value){
			// TODO: 
			static_assert(meta::is_contiguous<mem_t>::value,"non contigioud layout is not yet supported" );

			if constexpr (meta::has_iterator<mem_t>::value) {// this class needs scatter/gather
				//using element_t = typename meta::decay<typename mem_t::value_t>::type;
				using element_t = typename std::remove_const<typename mem_t::value_type>::type;
				std::vector <element_t> ptrs;
				h5::dt_t<element_t> dt = h5::copy<element_t>();
				if constexpr(meta::has_data<element_t>::value)
					for(auto& element: ref) ptrs.push_back(element.data());
				else if constexpr(meta::is_pod<element_t>::value)
					for(auto& element: ref) ptrs.push_back(element);
				if constexpr (meta::has_size<mem_t>::value)
					impl::write(ds, dt, ptrs.data(), h5::count{ref.size()}, args...);
				else // forward list like containers have  no ref.size() 
					impl::write(ds, dt, ptrs.data(), args...);
			} else static_assert(!meta::has_container_type<mem_t>::value,
				"no T.data() or T.begin()/T.end() found, are you trying to persist an adaptor?");
		} else if constexpr (meta::has_data<mem_t>::value) {
			using element_t = typename mem_t::value_type;
			h5::dt_t<element_t> dt = h5::create<mem_t>();
			if constexpr(meta::is_array<mem_t>::value && !tcount::present){ // array is a scalar, duh?
				impl::write(ds, dt, ref.data(), h5::count{1}, args...);
			}else 
				impl::write(ds, dt, ref.data(), h5::count{ref.size()}, args...);
		} else if constexpr( meta::is_array<mem_t>::value || meta::is_scalar<mem_t>::value){
			// count is manually set, don't overwrite it
			if constexpr( tcount::present ) impl::write(ds, h5::create<mem_t>(), &ref, args...);
			else impl::write(ds, h5::create<mem_t>(), &ref, h5::count{1}, args...);
		}
		return ds;
	} catch ( const std::exception& err ){
		throw h5::error::io::dataset::write( err.what() );
	}

    // same as above, it accepts rank 1 initializer list, but not arrays
    template <class mem_t, size_t N, class... args_t>
	typename std::enable_if<!meta::is_string_char<mem_t>::value, 
	h5::ds_t>::type write( const h5::ds_t& ds, const mem_t(& arr)[N], args_t&&... args) {
		return h5::write<mem_t[N]>(ds, arr, args...);
    }

  /** @ingroup io-write
 	*  @brief writes data within an HDF5 container specified with `h5::fd_t` descriptor 
	* HDF5 dataset may or may not exist, in first case it is opened and in the latter created. The implemantation comes with sensible 
	* default arguments, which can be tuned with optional property list.
	* By default the HDF5 dataset size, the file space, is derived from the passed object properties, or may be explicitly specified
	* with optional properties such as h5::count, h5::current_dims h5::max_dims, h5::stride, h5::block 
	* @param fd valid h5::fd_t descriptor
	* @param dataset_path where the dataset is, or will be created within the HDF5 file
	* @param ptr pointer to memory region with T* type
    * @param args[, ...] comma separated list of arguments in arbitrary order, only `T object` is required 
	* @return h5::ds_t  a valid, RAII enabled handle, binary compatible with HDF5 CAP `hid_t` 
	* 
	* @tparam T the type HDF5 dataset is created with
	*
	* <br/>The following arguments are context sensitive, may be passed in arbitrary order and with the exception
	* of `T object` being saved, the arguments are optional. The arguments are set to sensible values, and in most cases
	* will provide good performance by default, with that in mind, it is an easy high level fine tuning mechanism to 
	* get the best experience witout trading readability. 
	*
	* @param h5::current_dims_t optionaly  defines the size of the dataset, applicable only to datasets which has to be created
	* When omitted, the system computes the default value as follows  `h5::block{..}` and `h5::stride{..}` given as:
	* 		`current_dims[i] = count[i] (stride[i] - block[i] + 1) + offset[i]` and when only `h5::count` is available 
	*       `current_dims[i] = count[i] + offset[i]`
	* @param h5::max_dims_t  optional maximum size of dataset, applicable only to datasets which has to be created `max_dims[i] >= current_dims[i]`
	* or `H5S_UNLIMITED` along the dimension intended to be extendable
	*
	* @param h5::count number of element of `element_t`, compiler time error if not specified when `T*` is a pointer. Not the same as HDF5  `count`
	* in hyperslab selection; instead from this value the correct `slab_count` is derived considering the block size whenever applicable with a normalisation step:
    * `slab_count[i] = h5::count[i] / h5::block[i]`
	* @param h5::stride_t determins how many elements to move in each dimension from current position, `stride[i] >= block[i] && stride[i] != 0` 
	* @param h5::block_t only used when `stride` is specified, as `h5::count` otherwise the `slab_block` is derived from `h5::count` or number of elements 
	* @param h5::offset_t writes `T object` starting from this coordinates, considers this shift into `h5::current_dims` when applicable
	* @param h5::dcpl_t data creation property list, used only if dataset needs to be created
	* @param h5::dxpl_t data transfer property list, used for IO 
	* @param h5::dapl_t data access property list, how existing dataset is opened  
	* @param h5::lcpl_t link control property list, controls how path is created when applicable
	* 
	* <br/><b>example:</b>		
	* @code
	* std::vector<int> vec(10'000);
	* h5::fd_t fd = h5::open("example.h5", H5F_ACC_RDWR);
	* // dataset will be created with the dimensions specified
	* h5::ds_t ds = h5::write(fd, "path/dataset from direct memory", object.data(),
	* 	h5::current_dims{vec.length()}, h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9});
	* @endcode 
 	*/ 
 
	template <class file_t=undefined_t, class mem_t, class... args_t>
	typename std::enable_if<!std::is_array<mem_t>::value, 
	h5::ds_t>::type inline write( const h5::fd_t& fd, const std::string& dataset_path, const mem_t& ref,  args_t&&... args  ) {
		return h5::write<mem_t>(
			impl::open_or_create<file_t, mem_t>(fd, dataset_path, ref, args...), ref, args...);
	}

    // same as above, it accepts arrays and initializer lists
    template <class file_t=undefined_t, class mem_t, size_t N, class... args_t>
	typename std::enable_if<!meta::is_string_char<mem_t>::value, 
	h5::ds_t>::type inline write( const h5::fd_t& fd, const std::string& dataset_path, const mem_t(& arr)[N], args_t&&... args) {
		return h5::write<mem_t[N]>(
			impl::open_or_create<file_t, mem_t[N]>(fd, dataset_path, arr, args...), arr, args...);
    }

   /** @ingroup io-write
 	*  @brief writes content of an object, collection of object or memory location to a possibly not yet existing dataset 
	* within an HDF5 container opened with flag `H5F_ACC_RDWR`
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
	* @param h5::count number of element of `element_t`, compiler time error if not specified when `T*` is a pointer. Not the same as HDF5  `count`
	* in hyperslab selection; instead from this value the correct `slab_count` is derived considering the block size whenever applicable with a normalisation step:
    * `slab_count[i] = h5::count[i] / h5::block[i]`
	* @param h5::stride_t determins how many elements to move in each dimension from current position, `stride[i] >= block[i] && stride[i] != 0` 
	* @param h5::block_t only used when `stride` is specified, as `h5::count` otherwise the `slab_block` is derived from `h5::count` or number of elements 
	* @param h5::offset_t writes `T object` starting from this coordinates, considers this value into `h5::current_dims` when applicable	
	* @param h5::stride_t skip this many blocks along given dimension
	* @param h5::block_t only used when `stride` is specified 
	* @param h5::offset_t writes `T object` starting from this coordinates, considers this shift into `h5::current_dims` when applicable
	* @param h5::dcpl_t data creation property list, used only if dataset needs to be created
	* @param h5::dxpl_t data transfer property list, used for IO 
	* @param h5::dapl_t data access property list, how existing dataset is opened  
	* @param h5::lcpl_t link control property list, controls how path is created when applicabl
	* <br/><b>example:</b>
	* @code
	* std::vector<int> data = ...
	* //creates an extendable, chunked dataset, with initial size matching of the vector
	* h5::ds_t ds = h5::write("example.h5", "path/chunked layout dataset", data,
	* 		h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9});
	* //creates a contiguous layout dataset, with size matching `data` 
	* h5::ds_t ds = h5::write("example.h5", "path/contiguous layout dataset", data);
	* //interop with HDF5 CAPI: will flush dataset `ds` by directly calling libhdf5 function
	* H5Dflush(ds); 
	* @endcode 
 	*/ 
	template <class... args_t>
	inline h5::ds_t write( const std::string& file_path, const std::string& dataset_path, args_t&&... args  ){
		//TODO: refine delegation
		h5::fd_t fd = h5::open( file_path, H5F_ACC_RDWR, h5::default_fapl );
		return h5::write( fd, dataset_path, args...);
	}
}
#endif
