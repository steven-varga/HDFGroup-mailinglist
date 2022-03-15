# [Example to rewrite attributes](https://github.com/steven-varga/HDFGroup-mailinglist/tree/master/extend-attribute-2022-03-15)

While it is not possible to append/extend attributes in HDF5, attributes often represent side band information with relatevily small size. In fact in previous HDF5 versions the attribute size was limited to 64kb, however Gerd Heber suggest [this limitation has been lifted](https://forum.hdfgroup.org/t/extent-the-dataspace-of-an-existing-attribute/9464/6?u=steven). 

Haveing said the above it is a good strategy to break up `append` operation to `read old dataset` and `write new dataset` operations. The implementation is straightforward, and when used properly also is performant.

```
#include <vector>
#include <armadillo>
#include <h5cpp/all>

int main(void) {
    h5::fd_t fd = h5::create("h5cpp.h5",H5F_ACC_TRUNC);
    arma::mat data(10,5);
    
    { // 
    h5::ds_t ds = h5::write(fd,"some dataset", data);  // write dataset, and obtain descriptor
    h5::awrite(ds, "attribute name", {1,2,3,4,5,6,7});
    }
}
```

will give you the following layout
```
h5dump -a /some_dataset/attribute_name  h5cpp.h5
HDF5 "h5cpp.h5" {
ATTRIBUTE "attribute_name" {
   DATATYPE  H5T_STD_I32LE
   DATASPACE  SIMPLE { ( 7 ) / ( 7 ) }
   DATA {
   (0): 1, 2, 3, 4, 5, 6, 7
   }
}
}
```
To update the attribute you need to remove it first, since H5CPP doesn't yet do this automatically; in fact there is no `h5::adelete` either! -- however by design you can interchange HDF5 C API calls with H5CPP templates, so here is the update with [H5Adelete](https://portal.hdfgroup.org/display/HDF5/H5A_DELETE) and [`h5::awrite`](http://sandbox.h5cpp.org/architecture/#attributes): 

```
H5Adelete(ds,  "attribute name");
h5::awrite(ds, "attribute name", values);
```

```
h5dump -a /some_dataset/attribute_name  h5cpp.h5
HDF5 "h5cpp.h5" {
ATTRIBUTE "attribute_name" {
   DATATYPE  H5T_STD_I32LE
   DATASPACE  SIMPLE { ( 14 ) / ( 14 ) }
   DATA {
   (0): 1, 2, 3, 4, 5, 6, 7, 20, 21, 22, 23, 24, 25, 26
   }
}
}
```