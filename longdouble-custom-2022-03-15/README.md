# Long Double with custom datatype

There are two implementations provided, one with sustom float types, whereas an opaque dataype. None of the implementations handles architecture specific details. Please ADAPT the code snipet to your needs:


```
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
```

And here is the result:

```
HDF5 "example.h5" {
GROUP "/" {
   DATASET "custom" {
      DATATYPE  H5T_NATIVE_LDOUBLE
      DATASPACE  SIMPLE { ( 20 ) / ( 20 ) }
      DATA {
      (0) : 4.94066e-324, 4.94066e-324, 4.94066e-324, 4.94066e-324,
      (4) : 4.94066e-324, 4.94066e-324, 4.94066e-324, 4.94066e-324,
      (8) : 4.94066e-324, 4.94066e-324, 4.94066e-324, 4.94066e-324,
      (12): 4.94066e-324, 4.94066e-324, 4.94066e-324, 4.94066e-324,
      (16): 4.94066e-324, 4.94066e-324, 4.94066e-324, 4.94066e-324
      }
   }
   DATASET "opaque" {
      DATATYPE  H5T_OPAQUE {
         OPAQUE_TAG "";
      }
      DATASPACE  SIMPLE { ( 20 ) / ( 20 ) }
      DATA {
      (0) : 59:16:f5:f3:bb:e2:28:b8:01:40:00:00:00:00:00:00,
      (1) : 21:93:2c:c5:cc:f5:5b:90:00:40:00:00:00:00:00:00,
      (2) : 73:bb:b4:43:02:7a:0a:83:02:40:00:00:00:00:00:00,
      (3) : 44:d4:b4:6a:e7:6b:63:cf:fd:3f:00:00:00:00:00:00,
         ...
      (17): e0:b0:69:ef:df:4b:c9:c6:01:40:00:00:00:00:00:00,
      (18): b5:40:ab:14:42:57:65:f6:01:40:00:00:00:00:00:00,
      (19): a8:c8:78:67:ec:d4:b3:ca:fc:3f:00:00:00:00:00:00
      }
   }
}
}
```
Of course you are not limited to OPAQUE dataypes, adapting the following lines should cover all custom dataype cases. In fact the [H5CPP examples directory: datatypes](https://github.com/steven-varga/h5cpp/tree/master/examples/datatypes) covers `twobit` and `nbit` cases; and [half-float](https://github.com/steven-varga/h5cpp/tree/master/examples/half-float) directory implements support for two popular half float dataype. I recommend you to review them.
```
hid_t() : parent( H5Tcreate( H5T_OPAQUE, 10)) {
    hid_t id = static_cast<hid_t>( *this );
}
```

best wishes: steve
