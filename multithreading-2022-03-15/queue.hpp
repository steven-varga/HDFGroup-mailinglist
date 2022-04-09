
#ifndef H5CPP_QUEUE
#define H5CPP_QUEUE

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stop_token>
#include <queue>
#include <iterator>
#include <atomic>


namespace h5::exp {
	template <typename T> struct queue {
		queue(): mutex(), cv(){
		}
        // running on different thread than CTOR
		void push(const T& value) {
            std::lock_guard<std::mutex> lock(mutex);
            data.push(value);
            cv.notify_one();
		}

        T pop(void){
            std::unique_lock<std::mutex> lock(mutex);
            while(data.empty())
                cv.wait(lock);
            T value = data.front();
            data.pop();
            return value;
        }

    private:
		mutable std::mutex mutex;
		std::condition_variable cv;
        std::queue<T> data;
	};
}

#endif