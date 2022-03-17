/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */

#include <thread>
#include "queue.hpp"
#include "utils.hpp"

constexpr int dataset_name_min=5;
constexpr int dataset_name_max=21;
constexpr int dataset_size=40000;
constexpr int number_of_threads=60;

struct task_t {
	std::string name;
	std::vector<double> data;
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
			auto payload = h5::utils::get_test_data<double>(dataset_size);
			std::string path = h5::utils::get_random_string(dataset_name_min, dataset_name_max);
			io.push(
				task_t{path, payload});
		});
}

