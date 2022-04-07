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
    std::cout <<" queue<T,deque<T>>:" << mock::data<queue<string, deque<string>>>::get(3,5, min, max)<<"\n";
    std::cout <<"  queue<T,list<T>>:" << mock::data<queue<string, list<string>>>::get(2,7, min, max)<<"\n";

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