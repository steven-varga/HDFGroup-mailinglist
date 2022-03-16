/* Copyright (c) 2022 vargaconsulting, Toronto,ON Canada */

#include <thread>
#include "queue.hpp"
#include "utils.hpp"
#include <csignal>
#include <iostream>


constexpr int dataset_name_min=5;
constexpr int dataset_name_max=21;
constexpr int dataset_size=40000;

using T = std::pair<std::string, std::vector<double>>;



void signal_handler(int signal){
	std::cout << "signal: " << signal <<std::endl;
	exit(0);
}

int main(){
	using namespace std::chrono_literals;
	
	h5::exp::queue<T> io;

	auto n_threads = 60;//std::jthread::hardware_concurrency();
	std::atomic<unsigned> n_jobs = n_threads;
	std::vector<std::jthread> pool(n_threads); 

	std::signal(SIGINT, signal_handler);
	h5::fd_t fd = h5::create("example.h5", H5F_ACC_TRUNC);

	std::jthread io_thread([&]{
		while(n_jobs--){
			auto data = io.pop();
			{
				h5::write(fd, data.first, data.second);
			}
			std::cout << data.first << "\n";
		}
		std::cout << "done!!!\n";
	
	});

	for(auto& current_thread: pool) 
		current_thread = std::jthread([&]{
			auto payload = h5::utils::get_test_data<double>(dataset_size);
			std::string path = h5::utils::get_random_string(dataset_name_min, dataset_name_max);
			// simulate workload:
			std::this_thread::sleep_for(200ms);
			io.push(std::make_pair(path, payload));
		});
}

