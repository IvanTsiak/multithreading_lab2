#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <format>

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

void run_experiment(size_t data_size) {
    std::vector<int> data = generate_random_data(data_size);
    int value_to_find = -1;
    auto predicate = [value_to_find](int val) {
        return val == value_to_find;
    };

    //1
    // Можливо, потім заміню розрахунок часу на щось інше,
    // а то забагато повторень.
    auto start_time = std::chrono::high_resolution_clock::now();
    bool result = std::any_of(data.begin(), data.end(), predicate);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = end_time - start_time;
    long long microsec_duration = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    std::cout << microsec_duration << std::endl;



    //2


    //3
    

}

int main() {
    run_experiment(1000);

    return 0;
}