#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <format>
#include <execution>

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

bool multithreads_any_of(std::vector<int> data, std::function<bool(int)> predicate, unsigned int K) {
    if (K==0) {
        return false;
    }
    /*Тут потім буде така прекрасна реалізація такого прекрасного коду,
    взагалі зроблю таке-то-таке, що й сам не знаю. Жартую.
    Буде тут реалізація паралельного алгоритму, де кожному потоку дається своя частинка для
    перевірки чи є якесь значення у векторі. Можливо, доцільно тут використати м'ютекс,
    щоб уникнути стану гонки, але доживем і тоді побачим.*/
    return true;
}

template <typename Func>
long long measure_time(Func func) {
    auto start_time = std::chrono::high_resolution_clock::now();
    func();
    auto end_time = std::chrono::high_resolution_clock::now();

    long long duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    return duration;
}

void run_experiment(size_t data_size) {
    std::vector<int> data = generate_random_data(data_size);
    int value_to_find = -1;
    auto predicate = [value_to_find](int val) {
        return val == value_to_find;
    };

    //1
    long long m_t = measure_time([data, predicate]() {
        std::any_of(data.begin(), data.end(), predicate);
    });

    std::cout << "\nWithout politics: " << m_t << std::endl;

    //2
    m_t = measure_time([data, predicate]() {
        std::any_of(std::execution::seq, data.begin(), data.end(), predicate);
    });

    std::cout << "\nPolitic std::execution::seq: " << m_t << std::endl;

    m_t = measure_time([data, predicate]() {
        std::any_of(std::execution::par, data.begin(), data.end(), predicate);
    });

    std::cout << "\nPolitics std::execution::par: " << m_t << std::endl;

    m_t = measure_time([data, predicate]() {
        std::any_of(std::execution::par_unseq, data.begin(), data.end(), predicate);
    });

    std::cout << "\nPolitics std::execution::par_unseq: " << m_t << std::endl;
    //3
    

}

int main() {
    run_experiment(100000000);

    return 0;
}