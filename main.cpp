#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <execution>
#include <atomic>

std::vector<int> generate_random_data(size_t size) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 1000000);
    for (size_t i=0; i<size; ++i) {
        data[i] = distrib(gen);
    }

    return data;
}

template<typename Predicate>
bool multithreads_any_of(const std::vector<int>& data, Predicate predicate, int K) {
    if (K==0) {
        return false;
    }

    if (K==1) {
        return std::any_of(data.begin(), data.end(), predicate);
    }

    std::atomic<bool> found{false};
    {
        std::vector<std::jthread> threads;
        threads.reserve(K);
        size_t chunk_size = data.size() / K;

        for (int i=0; i<K; ++i) {
            auto start = data.begin() + i * chunk_size;
            auto end = (i==K-1) ? data.end() : start + chunk_size;

            threads.emplace_back([&found, start, end, predicate] {
                for (auto it = start; it != end; ++it) {
                    if (found.load(std::memory_order_acquire)) {
                        return;
                    }
                    if (predicate(*it)) {
                        found.store(true, std::memory_order_release);
                        return;
                    }
                }
            });
        }
    }   
    return found.load();
}

template <typename Func>
long long measure_time(Func func) {
    auto start_time = std::chrono::high_resolution_clock::now();
    func();
    auto end_time = std::chrono::high_resolution_clock::now();

    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    return duration;
}

void run_experiment(const std::vector<int>& data, const int& value_to_find) {
    auto predicate = [value_to_find](int val) {
        return val == value_to_find;
    };

    volatile bool res;
    //1
    long long m_t = measure_time([&data, predicate, &res]() {
        res = std::any_of(data.begin(), data.end(), predicate);
    });

    std::cout << "\nWithout politics: " << m_t << std::endl;

    //2
    m_t = measure_time([&data, predicate, &res]() {
        res = std::any_of(std::execution::seq, data.begin(), data.end(), predicate);
    });

    std::cout << "Politic std::execution::seq: " << m_t << std::endl;

    m_t = measure_time([&data, predicate, &res]() {
        res = std::any_of(std::execution::par, data.begin(), data.end(), predicate);
    });

    std::cout << "Politics std::execution::par: " << m_t << std::endl;

    m_t = measure_time([&data, predicate, &res]() {
        res = std::any_of(std::execution::par_unseq, data.begin(), data.end(), predicate);
    });

    std::cout << "Politics std::execution::par_unseq: " << m_t << std::endl << std::endl;

    //3
    const int core_count = std::thread::hardware_concurrency();
    long long best_time = -1;
    int best_K = 0;

    std::cout << "Speed of own parallel algorithm with different number of threads (K):" << std::endl;
    for (int k=1; k<=core_count+4; ++k) {
        bool res_custom;
        long long time_custom = measure_time([&res_custom, &data, &predicate, k]{
            res_custom = multithreads_any_of(data, predicate, k);
        });
        if (best_time == -1 || time_custom < best_time) {
            best_time = time_custom;
            best_K = k;
        }

        std::cout << "K = "<< k << ": " << time_custom << std::endl;
    }

    std::cout << "Best performance at K = " << best_K << " (" << best_time << " microseconds)\n";
}

int main() {
    std::vector<size_t> data_sizes = {100'000, 1'000'000, 100'000'00, 100'000'000};
    const int inique = -10;
    const int not_found = -1;
    for (size_t data_size : data_sizes) {
        std::cout << "\n----------------------------------------------" << 
                    "\nData size: " << data_size << std::endl;
        std::vector<int> data = generate_random_data(data_size);
        std::cout << "Element not foud:" << std::endl;
        run_experiment(data, not_found);
    }
    return 0;
}