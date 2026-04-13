
# AMR Storage in HDF5 with H5CPP — A Practical Sketch

This short note outlines a simple and practical approach to storing block-structured Adaptive Mesh Refinement (AMR) data in HDF5, based on guidance from the HDF Group community.

> **Credit:** The layout and initial idea were inspired by a response and Python sketch from Gerd Heber. Many thanks for the clear direction.

## Problem Overview

We consider an AMR mesh with:

* **N blocks**
* Each block has **I × J × K cells**
* Block sizes vary
* Cell spacing is uniform *within* each block, but differs *across* blocks

Goal:

> Store AMR data in **HDF5 + XDMF** such that it is compatible with tools like ParaView and VisIt.

## Recommended HDF5 Layout

The key design decision:

> **Each block must be stored separately** (because shapes differ).

```text
/
├── metadata/
│   ├── block_origins   [N,3]   # (x0,y0,z0)
│   ├── block_spacings  [N,3]   # (dx,dy,dz)
│   ├── block_dims      [N,3]   # (I,J,K)
│   └── block_levels    [N]
└── fields/
    ├── block_0000/
    │   ├── density     [I,J,K]
    │   └── velocity    [I,J,K,3]
    ├── block_0001/
    └── ...
```

This layout is simple, explicit, and aligns well with visualization tools.

## XDMF Companion Structure

Each block becomes a **Uniform Grid**, grouped into a **Spatial Collection**:

```xml
<Grid GridType="Collection" CollectionType="Spatial" Name="AMR">

  <Grid GridType="Uniform" Name="block_0000">
    <Topology TopologyType="3DCoRectMesh" Dimensions="K+1 J+1 I+1"/>
    <Geometry GeometryType="ORIGIN_DXDYDZ">
      <DataItem Format="HDF" Dimensions="3">mesh.h5:/metadata/block_origins[0]</DataItem>
      <DataItem Format="HDF" Dimensions="3">mesh.h5:/metadata/block_spacings[0]</DataItem>
    </Geometry>

    <Attribute Name="density" AttributeType="Scalar" Center="Cell">
      <DataItem Format="HDF" Dimensions="K J I">
        mesh.h5:/fields/block_0000/density
      </DataItem>
    </Attribute>
  </Grid>

</Grid>
```

### Important detail

* XDMF uses **ZYX ordering**, so:

  * dataset `[I,J,K]` → `"K J I"`
  * topology → `"K+1 J+1 I+1"`

## Implementation Strategy with H5CPP
1. Define **plain C++ POD structures**
2. Write code *as if HDF5 types already exist*
3. Write the map between C++ and HDF5 types
4. Compile and link normally

## Example: Block Representation

```cpp
struct block_t {
    std::array<double,3> origin, spacing;
    std::array<std::size_t,3> dims;
    std::uint32_t level;
    std::vector<double> density, velocity;
};
```

## Example: Metadata Writing

```cpp
h5::write(fd, "/metadata/block_origins",  block_origins);
h5::write(fd, "/metadata/block_spacings", block_spacings);
h5::write(fd, "/metadata/block_dims",     block_dims);
h5::write(fd, "/metadata/block_levels",   block_levels);
```

## Example: Per-Block Fields

```cpp
for (std::size_t i = 0; i < blocks.size(); ++i) {
    const auto tag = std::format("/fields/block_{:04d}", i);

    h5::write(fd, tag + "/density", b.density, h5::current_dims{I,J,K}, h5::chunk{...} | h5::gzip{4});
    h5::write(fd, tag + "/velocity", b.velocity, h5::current_dims{I,J,K,3}, h5::chunk{...} | h5::gzip{4});
}
```

## Type Mapping (Key Detail)

For `std::array<T,3>`:

```cpp
template<>
hid_t register_struct<std::array<double,3>>() {
    hsize_t dims[1] = {3};
    return H5Tarray_create2(H5T_IEEE_F64LE, 1, dims);
}
```

This ensures correct mapping to:

```text
H5T_ARRAY { [3] H5T_IEEE_F64LE }
```

## Takeaway

* Use **per-block groups** for variable AMR blocks
* Store geometry separately in **metadata**
* Use **XDMF Spatial Collection**
* Keep types simple: arrays → HDF5 array types, structs → compound
* Avoid over-engineering — this layout is already widely usable

## Final structure
```txt
h5dump -H amr_synthetic.h5 
HDF5 "amr_synthetic.h5" {
GROUP "/" {
   GROUP "fields" {
      GROUP "block_0000" {
         DATASET "density" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 8, 8 ) / ( 8, 8, 8 ) }
         }
         DATASET "velocity" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 8, 8, 3 ) / ( 8, 8, 8, 3 ) }
         }
      }
      GROUP "block_0001" {
         DATASET "density" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 8, 8 ) / ( 8, 8, 8 ) }
         }
         DATASET "velocity" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 8, 8, 3 ) / ( 8, 8, 8, 3 ) }
         }
      }
      GROUP "block_0002" {
         DATASET "density" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 8, 8 ) / ( 8, 8, 8 ) }
         }
         DATASET "velocity" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 8, 8, 3 ) / ( 8, 8, 8, 3 ) }
         }
      }
      GROUP "block_0003" {
         DATASET "density" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 12, 10 ) / ( 8, 12, 10 ) }
         }
         DATASET "velocity" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 8, 12, 10, 3 ) / ( 8, 12, 10, 3 ) }
         }
      }
      GROUP "block_0004" {
         DATASET "density" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 10, 9, 11 ) / ( 10, 9, 11 ) }
         }
         DATASET "velocity" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 10, 9, 11, 3 ) / ( 10, 9, 11, 3 ) }
         }
      }
   }
   GROUP "metadata" {
      DATASET "block_dims" {
         DATATYPE  H5T_ARRAY { [3] H5T_STD_U64LE }
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
      }
      DATASET "block_levels" {
         DATATYPE  H5T_STD_U32LE
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
      }
      DATASET "block_origins" {
         DATATYPE  H5T_ARRAY { [3] H5T_IEEE_F64LE }
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
      }
      DATASET "block_spacings" {
         DATATYPE  H5T_ARRAY { [3] H5T_IEEE_F64LE }
         DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
      }
   }
}
}
steven@gauss:~/projects/HDFGroup-mailinglist/amr_to_hdf5-2026-04-13$ 


```