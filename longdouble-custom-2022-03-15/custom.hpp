/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada;  MIT license*/

// do your thing, usually this would be in a separate header file
namespace custom {
	struct ldouble_t { // wrapper to aid C++ template mechanism, zero runtime cost
		// allow conversion: auto value =  static_cast<long double>( custom::ldouble_t );
		explicit operator long double() const {
			return value;
		}

		ldouble_t() = default;
		ldouble_t(long double value_ ) : value(value_){}
		long double value;
	};
}

// BEGIN H5CPP SPECIFIC CUSTOM TYPE DEFINITION
namespace h5::impl::detail {
	template <> struct hid_t<custom::ldouble_t, H5Tclose,true,true, hdf5::type> : public dt_p<custom::ldouble_t> {
		using parent = dt_p<custom::ldouble_t>;  // h5cpp needs the following typedefs
		using dt_p<custom::ldouble_t>::hid_t;
		//using parent::hid_t; --> this style of inheriting ctor will not work with clang :(
		using hidtype = custom::ldouble_t;

		// opaque doesn't care of byte order, also since you are using single byte
		// it is not relevant
		hid_t() : parent( H5Tcopy( H5T_NATIVE_FLOAT )) {
			// herr_t H5Tset_fields( hid_t dtype_id, 
			//                   spos, epos, esize, mpos, msize )
			H5Tset_precision(handle, 80);      
			H5Tset_ebias( handle, 16383);
			H5Tset_size(handle,16);
			H5Tset_norm(handle, H5T_NORM_NONE); // not normalized
			H5Tset_fields( handle, 79,   64,    15,    0,    64);
			hid_t id = static_cast<hid_t>( *this );
		}
		/*	see: https://en.wikipedia.org/wiki/Extended_precision#x86_extended_precision_format*/
	};
}
namespace h5 {
	template <> struct name<custom::ldouble_t> {
		static constexpr char const * value = "custom::ldouble_t";
	};
}
// END H5CPP SPECIFIC TYPE DEFINEITION


// SERVICE ROUTINES
std::ostream& operator<<(std::ostream& os, const custom::ldouble_t& data){
	os << data.value;
	return os;
}

