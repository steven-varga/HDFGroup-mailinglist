# [Automated formatted display of STL like containers][5]

Debugging C++ is an art and may require elaborated setup of [`valgrind`][1], `cachegrind`, [gdb][2] or other specialized tools, to avoid this overhead why not just print on the screen?

To facilitate software development H5CPP will have the following, feature detection idiom based (see: [Walter Brown's WG21 N4436][3] paper), mechanism to print on `std::cout | std::cerr | ...` with intuitive syntax: `std::cout << stl_like_object` where `stl_like_object` maybe arbitrary implementation (not just the STL) with the following properties:

* provides STL iterators: `.begin(), .end()` lists, vectors, maps,  etc...
* `.top(), .pop(), .size()` such as `std::stack`, `std::priority_queue`
* `.front(), .pop(), .size()` such as `std::queue`

The mechanism is simple, robust and will replace/enhance the H5CPP persistence infrastructure to able to generalize to all STL like containers and provide IO capability in a non-intrusive way. 
Expect the display/printing functionality to be gradually improving, the current capabilities are recursively pretty print STL like objects **in-line** with maximum width support. If you are interested in further improving this printing feature, say bring it on par to Julia pretty print -- please get in touch.  

Combined with my previous post that touches on [std::tuple<T,Ts...>][4] I dare to exhibit the following examples, to demonstrate
H5CPP(next gen) capabilities to generate and display near arbitrary complex data:

```
g++ -I./include -o pprint-test.o   -std=c++17 -DFMT_HEADER_ONLY -c pprint-test.cpp
g++ pprint-test.o -lhdf5  -lz -ldl -lm  -o pprint-test
./pprint-test


LISTS/VECTORS/SETS:
---------------------------------------------------------------------
   array<string,7>:[xSs,wc,gu,Ssi,Sx,pzb,OY]
            vector:[CDi,PUs,zpf,Hm,teO,XG,bu,QZs]
             deque:[256,233,23,89,128,268,69,278,130]
              list:[95,284,24,124,49,40,200,108,281,251,57, ...]
      forward_list:[147,76,81,193,44]
               set:[V,f,szy,v]
     unordered_set:[2.59989,1.86124,2.93324,1.78615,2.43869,2.04857,1.69145]
          multiset:[3,5,12,21,23,28,30,30]
unordered_multiset:[gZ,rb,Dt,Q,Ark,dW,Ez,wmE,GwF]


ADAPTORS:
---------------------------------------------------------------------
stack<T,vector<T>>:[172,252,181,11]
 stack<T,deque<T>>:[54,278,66,70,230,44,121,15,58,149,224, ...]
  stack<T,list<T>>:[251,82,278,86,66,40,278,45,211,225,271, ...]
    priority_queue:[zdbUzd,tTknDw,qorxgk,mCcEay,gDeJ,FYPOEd,CIhMU]
 queue<T,deque<T>>:[bVG,Bbs,vchuT,FfxEw,CXFrr,JAx,sVlcI]  
  queue<T,list<T>>:[ARPl,dddmHT,mEiCJ,OVEYS,FIJi,jbQwb,tpJnpj,rlCRoKn,nBKjJ,KPlU,jatsUI, ...]

ASSOCIATE CONTAINERS:
---------------------------------------------------------------------
                    map<string,int>:[{LID:2},{U:2},{Xr:1},{e:2},{esU:1},{kbj:1},{qFc:3}]
            map<short,list<string>>:[{LjwUkey:5},{jZxhk:6},{sxKKVu:8},{vSmHmu:8},{wRBTdGS:7}]
          multimap<short,list<int>>:[{ALpPkqbJ:[6,6,8,7,8,5,7,8,5,5,6, ...]},{AwsHR:[8,5,6,6,5,6,7,6,7,6,8, ...]},{HtLQMvHv:[5,7,6,7,8,6,7]},{KbseLYEs:[5,8,6,8]},{RzsJm:[7,6,8,7,7,7,7,6,6,8,7, ...]},{XpNSkhDa:[7,5,8,8,7,8,5,5,5]},{cXPImNk:[6,8,6,5,8,7,5,6,6,8,6, ...]},{gkKHyh:[5,8,6,6,6,6,5,5,6]},{iPmaraP:[7,6,7,6,7,7,5,7,5,7,7, ...]},{pLmqL:[6,5,5,5,6]}]
  unordered_map<short,list<string>>:[{udXahPXD:7},{hUgYjak:5},{OpOmaBqA:7},{vTldeWdS:5},{jEHQST:8},{UZxId:7},{IslGsnGY:8}]
unordered_multimap<short,list<int>>:[{JldxFw:[5,6,8,6,6]},{tnzhP:[8,6,8,5,5,8,8,8]},{cvMaS:[5,7,5,5,5,5]},{eGlyp:[8,7,8,8,7]}]


RAGGED ARRAYS:
---------------------------------------------------------------------
vector<vector<string>>:[[pkwZZ,lBqsR,cmKt,PDjaS,Zj],[Nr,jj,xe,uC,bixzV],[uBAU,pXCa,fZEH,FIAIO],[Vczda,HKEzO,ySqr,Fjd,nh,pgb,zcsw],[fLCgg,qQ,Reul,aTXp,DENn,ZDtkV,VXcB]]
array<vector<short>,N>:[[29,49,29,42,25,33,49,33,44,49],[50,48,35,22,35,33,33],[46,27,23,20,48,38,45,28,45],[25,33,41,22,36]]
array<array<short,M>,N>:[[90,35,99],[47,58,53],[82,25,72],[76,92,62],[39,88,32]]


MISC:
---------------------------------------------------------------------
pair<int,string>:{10:nTIHxSfPCB}
pair<string,vector<int>>:{LwTzsC:[4,6,4,6,5,2,8,4,5,8,4, ...]}
```

```c++
/* copyright steven varga, vargaconsulting 2021, june 08, Toronto, ON, Canada;  MIT license
*/
// must preceed <h5cpp/???> includes; usually placed in makefile with -DH5CPP_CONSOLE_WIDTH=20
#define H5CPP_CONSOLE_WIDTH 10

#include <iostream>
#include <h5cpp/H5Uall.hpp>
#include <h5cpp/H5cout.hpp>
#include <utils/types.hpp>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fmt/format.h>

namespace ns = h5::test;

int main(int argc, char **argv) {
    namespace mock = h5::utils;
    using namespace std;
    constexpr size_t lower=2, upper=3, min=4, max=12;      

    // lists 
    std::cout <<"\n\nLISTS/VECTORS/SETS:\n";
    std::cout <<"---------------------------------------------------------------------\n";
    std::cout <<"   array<string,7>:" << mock::data<array<string,7>>::get(lower, upper, min, max) <<"\n";
    std::cout <<"            vector:" << mock::data<vector<string>>::get(lower, upper, min, max) <<"\n";
    std::cout <<"             deque:" << mock::data<deque<int>>::get(0, 300, min, max) <<"\n";
    std::cout <<"              list:" << mock::data<list<int>>::get(0, 300, min, max) <<"\n";
    std::cout <<"      forward_list:" << mock::data<forward_list<int>>::get(0, 300, min, max) <<"\n";
    std::cout <<"               set:" << mock::data<set<string>>::get(1, 3, min, max) <<"\n";
    std::cout <<"     unordered_set:" << mock::data<unordered_set<long double>>::get(1, 3, min, max) <<"\n";
    std::cout <<"          multiset:" << mock::data<multiset<unsigned short>>::get(1, 30, min, max) <<"\n";
    std::cout <<"unordered_multiset:" << mock::data<unordered_multiset<string>>::get(1, 3, min, max) <<"\n";

    std::cout<< "\n\nADAPTORS:\n";
    std::cout <<"---------------------------------------------------------------------\n";
    std::cout <<"stack<T,vector<T>>:" << mock::data<stack<int,vector<int>>>::get(0, 300, min, max) <<"\n";
    std::cout <<" stack<T,deque<T>>:" << mock::data<stack<int,deque<int>>>::get(0, 300, min, 300) <<"\n";
    std::cout <<"  stack<T,list<T>>:" << mock::data<stack<int,list<int>>>::get(0, 300, min, max) <<"\n";
    std::cout <<"    priority_queue:" << mock::data<priority_queue<string>>::get(4, 6, min, max) <<"\n";
    std::cout <<" queue<T,deque<T>>:" << mock::data<queue<string, deque<string>>>::get(3,5, min, max) <<"\n";
    std::cout <<"  queue<T,list<T>>:" << mock::data<queue<string, list<string>>>::get(2,7, min, max) <<"\n";

    std::cout<< "\n\nASSOCIATE CONTAINERS:\n";
    std::cout <<"---------------------------------------------------------------------\n";
    std::cout <<"                    map<string,int>:" << mock::data<map<string,int>>::get(1, 3, min, max)<<"\n";
    std::cout <<"            map<short,list<string>>:" << mock::data<map<string,int>>::get(5, 8, min, max)<<"\n";
    std::cout <<"          multimap<short,list<int>>:" << mock::data<multimap<string,list<int>>>::get(5, 8, min, max)<<"\n";
    std::cout <<"  unordered_map<short,list<string>>:" << mock::data<unordered_map<string,int>>::get(5, 8, min, max)<<"\n";
    std::cout <<"unordered_multimap<short,list<int>>:" << mock::data<unordered_multimap<string,list<int>>>::get(5, 8, min, max)<<"\n";

    
    std::cout <<"\n\nRAGGED ARRAYS:\n";
    std::cout <<"---------------------------------------------------------------------\n";
    //ragged arrays
    std::cout <<"vector<vector<string>>:" << mock::data<vector<vector<string>>>::get(2, 5, 3, 7) <<"\n";
    std::cout <<"array<vector<short>,N>:" << mock::data<array<vector<short>,4>>::get(20, 50, 1, 7) <<"\n";
	std::cout <<"array<array<short,M>,N>:" << mock::data<array<array<short,3>,5>>::get(20, 110, 3, 7) <<"\n";

    std::cout <<"\n\nMISC:\n";
    std::cout <<"---------------------------------------------------------------------\n";
    //ragged arrays
    std::cout <<"pair<int,string>:" << mock::data<pair<int,string>>::get(2, 10, 3, 30) <<"\n";
    std::cout <<"pair<string,vector<int>>:" << mock::data<pair<string,vector<int>>>::get(2, 10, 3, 30) <<"\n";
	return 0;
}
```

Source code may be downloaded from this [GitHUB page][5], if you are interested in the topic, feel free to leave comments on this thread.  

*Note:*
The demonstrated data generators for arbitrary C++ datatype type will be the part of H5CPP distribution and is the missing piece of [h5rnd][3] project, a prufer sequence based random HDF5 dataset generator.

steve


[1]: https://valgrind.org/
[2]: https://www.sourceware.org/gdb/
[3]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4436.pdf
[4]: https://github.com/steven-varga/HDFGroup-mailinglist/tree/master/tuple-2022-aprl-06
[5]: https://github.com/steven-varga/HDFGroup-mailinglist/tree/master/automated-printing-2022-04-07