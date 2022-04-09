/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */

#include <h5cpp/all>
#include <h5cpp/H5Uall.hpp>
#include <h5cpp/H5cout.hpp>
#include <utils/types.hpp>
#include <benchmark/all>

#include <thread>
#include "queue.hpp"
#include "utils.hpp"

namespace bh = h5::bench;

constexpr int number_of_threads=60;

constexpr int dataset_name_min=5;     //dataset random name
constexpr int dataset_name_max=21;
constexpr int dataset_size_min=40000; // dataset size control
constexpr int dataset_size_max=80000;
// value control, currently restricted to be `size_t` type (work in progress)
constexpr size_t value_lower_bound=0, value_upper_bound=123;


// BECNCHMARKING SETUP: TBD


// H5CPP nextgen IO pipeline may not have all data types available 
using payload_t = std::vector<double>;

struct task_t {
	std::string name;
	payload_t data;
};

int main(){
	h5::exp::queue<task_t> io;

	auto n_threads = number_of_threads;
	std::atomic<unsigned> n_jobs = n_threads;
	std::vector<std::jthread> pool(n_threads); 

	h5::fd_t fd = h5::create("example.h5", H5F_ACC_TRUNC);

	std::jthread io_thread([&]{
		while(n_jobs--){
			auto task = io.pop();
			h5::write(fd, task.name, task.data);
		}
	});

	for(auto& current_thread: pool) 
		current_thread = std::jthread([&]{
			payload_t payload = h5::utils::data<payload_t>::get(
				dataset_size_min, dataset_size_max, dataset_size_min, dataset_size_max);
			std::string path = h5::utils::data<std::string>::get(dataset_name_min, dataset_name_max);
			io.push(
				task_t{path, payload});
		});
}

