# Adding extended STL support to H5CPP including **std::tuple<T,K...>**

Recently I picked up the somewhat delayed but planned extension of H5CPP and now I am reaching out 
to you, The HDF5 community member to ask for help/input on STL container usage in your field.  

**Topic/Question**: `some_container_t<T,Ts...>(...)` recursively instantiated with `T={numeric_types}` and `Ts={some_container_t<T>}` may result in a massive set of containers, being useful only a few of them given a field say {physics, chemistry, biology, ...} . What are the useful combinations in your field?

**Examples:**
The following functional example represents a sparse matrix in [COO][1] format where data stored column wise, which can be an intermediate step to persist `arma::spmat` or other popular **sparse** linear algebra constructs. This step is a necessary decomposition in order to persist arbitrary sparse matrix into multiple HDF5 datasets: 
```c++
// sparse matrix can be represented as 2 vectors of `coordinates` and a 
// vector of `values` fast access by columns
using coo_t = std::tuple<
    std::vector<size_t>, std::vector<size_t>, std::vector<value_t>>;
// request random sample data from H5CPP utils:
coo_t sparse_matrix = h5::utils::data<coo_t>::get(lower, upper, min, max);
```
with output:
```bash
./tuple-test
[ sparse matrix with size: 46 x 49  ]
     x:[30,8,34,46,38]
     y:[24,10,46,49,26]
values:[20.0288,45.3326,34.275,19.1391,31.8541]
```

The above is trivial, however creating a larger subset is not so much.  Allow me to walk you through an example where we have some container element types: 
```c++
using element_t = std::tuple<
    bool, std::string, ns::pod_t,
    unsigned char, unsigned short, unsigned int, unsigned long long int,
    char, short, int, long long int, float, double>;
```
and a set of containers with STL like properties *(ie: T.data() .size() maybe iterators...)* lets say:

```c++
template <class T> using stl_t = std::tuple<
    std::vector<T>, std::deque<T>, std::list<T>, std::forward_list<T>, 
    std::set<T>, std::unordered_set<T>, std::multiset<T>, std::unordered_multiset<T>,
    std::stack<T, std::deque<T>>, std::stack<T, std::vector<T>>, std::stack<T, std::list<T>>,
    std::queue<T, std::deque<T>>, std::queue<T, std::list<T>>,
    std::priority_queue<T, std::deque<T>>, std::priority_queue<T, std::vector<T>>
	>;
```
Then compute some possible instantiations and automatically generate random test data for the object with level one recursion: `std::list<std::string>,  std::vector<stl::vector<double>>, ...` [but not `std::vector< std::vector< std::vector<double> > >`] *note the triple embedding* yields to the triple  `static_for` loop, *a mechanism shamelessly borrowed from various C++ researchers* :


```c++
void full_cross_product(){
	constexpr size_t lower=4, upper=50, min=3, max=12;
    size_t num_tests = 0;

    h5::meta::static_for<element_t>( [&]( auto x ){
        constexpr size_t x_ = x;
        using Tx = typename std::tuple_element<x_, element_t>::type;
        using container_t = stl_t<Tx>;
        h5::meta::static_for<container_t>( [&]( auto y ){
            constexpr size_t y_ = y;
            Tx element;
            using Ty = typename std::tuple_element<y_, container_t>::type;
            using crossproduct_t = subset_t<Ty>;
            h5::meta::static_for<crossproduct_t>( [&]( auto z ){
                constexpr size_t z_ = z;
                Ty element;
                using Tz = typename std::tuple_element<z_, crossproduct_t>::type;
				// ACTUAL RANDOM DATA generated of the requested type with:
				// element value range := {lower,upper} and quantity/size := {min,max}
                auto M = h5::utils::data<crossproduct_t>::get(lower, upper, min, max);
                num_tests ++;
            }); // Tz container<A> x container<B>
        }); // Ty container x element_t 
    }); // Tx element_t
    std::cout<< std::setw(4) << num_tests << " tests has been performed"<<std::endl;
}
```
Computes a whopping **1755** cases! (all dataset correctly generated) The next examples generate associated containers

```c++
void string_map(){
    constexpr size_t lower=2, upper=10, min=5, max=5;      
    //                         key             values
    using map_t =  std::map<std::string, std::vector<std::string>>;

    // request random sample data from H5CPP utils:
    map_t map = h5::utils::data<map_t>::get(lower, upper, min, max);
    for (const auto &p : map)
        std::cout << p.first << "=>" << p.second << '\n';
    std::cout <<"\n\n";
}

void int_unordered_map(){
    constexpr size_t lower=2, upper=10, min=5, max=5;      
    //                         key             values
    using map_t =  std::unordered_map<int, std::vector<double>>;

    // request random sample data from H5CPP utils:
    map_t map = h5::utils::data<map_t>::get(lower, upper, min, max);
    for (const auto &p : map)
        std::cout << p.first << "=>" << p.second << '\n';
    std::cout <<"\n\n";
}

```

Expected result:
```
g++ -I./include -o tuple-test.o   -std=c++17 -DFMT_HEADER_ONLY -c tuple-test.cpp
g++ tuple-test.o -lhdf5  -lz -ldl -lm  -o tuple-test
./tuple-test
[ sparse matrix with: 50 x 48  ]
     x:[11,40,50,28,30]
     y:[48,28,8,37,21]
values:[33.6047,26.3818,16.6548,23.1324,44.084]
1755 tests has been performed

AErL=>[kdTFO,ZWAFxpbgXk,ZTw,QhAC,oyqcsX]
BuS=>[JNsbr,ZxNTmwTCi,ti,xTisqNnQY,lqvSOdDeO]
XzsbVgEluZ=>[DkVtWhSujh,QG,ESujw,PuHVLYh,SVmQm]
qlo=>[GqQWq,xNlh,yUrsdj,OmtiYEOs,TY]
xvfcS=>[SNGXhCYWfy,aD,btoox,tcqCCHwmXB,tySIML]


5=>[6.99629,7.93517,4.91782,2.0825,7.36592]
2=>[7.34483,5.90255,7.77281,8.19388,6.95707]
6=>[6.62439,2.78461,5.73858,3.45465,2.11209]
8=>[2.74198,5.66705,3.01571,8.19023,4.43414]
```


# What kind of STL containers do you use in your field?


If you find this topic interesting, within the ballpark of your expertise please get in touch on this forum, or directly through my email.

steven

**ps.:** the functional compilable examples are on [this GitHUB page][4]

*Note:*
The demonstrated data generators for arbitrary C++ datatype type will be the part of H5CPP distribution and is the missing piece of [h5rnd][3] project, a prufer sequence based random HDF5 dataset generator.

[1]: https://en.wikipedia.org/wiki/Sparse_matrix#Coordinate_list_(COO)
[2]: https://github.com/steven-varga/HDFGroup-mailinglist
[3]: https://github.com/steven-varga/h5rnd
[4]: https://github.com/steven-varga/HDFGroup-mailinglist/tree/master/tuple-2022-aprl-06