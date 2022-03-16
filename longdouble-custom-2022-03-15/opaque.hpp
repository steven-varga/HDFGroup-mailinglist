/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada;  MIT license*/

// do your thing, usually this would be in a separate header file
namespace opaque {
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
	template <> struct hid_t<opaque::ldouble_t, H5Tclose,true,true, hdf5::type> : public dt_p<opaque::ldouble_t> {
		using parent = dt_p<opaque::ldouble_t>;  // h5cpp needs the following typedefs
		using dt_p<opaque::ldouble_t>::hid_t;
		//using parent::hid_t; --> this style of inheriting ctor will not work with clang :(
		using hidtype = opaque::ldouble_t;

		// opaque doesn't care of byte order, also since you are using single byte
		// it is not relevant
		hid_t() : parent( H5Tcreate( H5T_OPAQUE, sizeof(opaque::ldouble_t) )) {
			hid_t id = static_cast<hid_t>( *this );
		}
		/*	see: https://en.wikipedia.org/wiki/Extended_precision#x86_extended_precision_format*/
	};
}
namespace h5 {
	template <> struct name<opaque::ldouble_t> {
		static constexpr char const * value = "opaque::ldouble_t";
	};
}
// END H5CPP SPECIFIC TYPE DEFINEITION


// SERVICE ROUTINES
std::ostream& operator<<(std::ostream& os, const opaque::ldouble_t& data){
	os << data.value;
	return os;
}

