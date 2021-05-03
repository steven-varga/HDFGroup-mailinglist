#include <iostream>
#include <vector>
#include <algorithm>
#include "non-pod-struct.hpp"

int main(void) {
    std::cout <<"[0: false 1: true] " 
        << std::is_pod<shim::pod_t>::value
        << " std::is_pod<shim::pod_t>::value" << std::endl;

    std::cout <<"[0: false 1: true] "
        << std::is_pod<non::intrusive::non_pod_t>::value 
        << " std::is_pod<non::intrusive::non_pod_t>::value" << std::endl;

    std::cout <<"[0: false 1: true] "
        << std::is_pod<intrusive::non_pod_t>::value 
        << " std::is_pod<intrusive::non_pod_t>::value " << std::endl;
    
    return 0;
}