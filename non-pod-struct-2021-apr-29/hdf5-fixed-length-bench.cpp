#include <criterion/criterion.hpp>


BENCHMARK(MergeSort, std::size_t) // <- one parameter to be passed to the benchmark
{
  SETUP_BENCHMARK(
    const auto size = GET_ARGUMENT(0); // <- get the argument passed to the benchmark
    std::vector<int> vec(size, 0);
  )
 
  // Code to be benchmarked
  std::vector vec1(size, 4);
  
  TEARDOWN_BENCHMARK(
    vec.clear();
  )
  return 0.0;
}

// Run the above benchmark for a number of inputs:

INVOKE_BENCHMARK_FOR_EACH(MergeSort,
  ("/10", 10),
  ("/100", 100),
  ("/1K", 1000),
  ("/10K", 10000),
  ("/100K", 100000)
)
CRITERION_BENCHMARK_MAIN()