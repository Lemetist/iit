#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using Array = std::vector<float>;
constexpr float LOW = 11020.0f, HIGH = 31702.0f;

void insertion(Array& a) {
    for (std::size_t i = 1; i < a.size(); ++i) {
        float key = a[i];
        std::size_t j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

// Разбиение Хоара: возвращает границу двух непустых частей.
int partition(Array& a, int left, int right) {
    float pivot = a[left + (right - left) / 2];
    int i = left - 1, j = right + 1;
    for (;;) {
        do { ++i; } while (a[i] < pivot);
        do { --j; } while (a[j] > pivot);
        if (i >= j) return j;
        std::swap(a[i], a[j]);
    }
}

void hoare_recursive(Array& a, int left, int right) {
    if (left >= right) return;
    int p = partition(a, left, right);
    hoare_recursive(a, left, p);
    hoare_recursive(a, p + 1, right);
}

void recursive_sort(Array& a) {
    if (!a.empty()) hoare_recursive(a, 0, int(a.size()) - 1);
}

void iterative_sort(Array& a) {
    if (a.size() < 2) return;
    std::vector<std::pair<int, int>> stack;
    stack.emplace_back(0, int(a.size()) - 1);
    while (!stack.empty()) {
        auto [left, right] = stack.back();
        stack.pop_back();
        if (left >= right) continue;
        int p = partition(a, left, right);
        if (left < p) stack.emplace_back(left, p);
        if (p + 1 < right) stack.emplace_back(p + 1, right);
    }
}

Array generate(std::size_t n, unsigned seed) {
    std::mt19937 generator(seed);
    std::uniform_real_distribution<float> dist(LOW, HIGH);
    Array a(n);
    for (float& x : a) x = dist(generator);
    return a;
}

using Sort = void (*)(Array&);
const Sort sorts[] = {iterative_sort, recursive_sort, insertion};
const char* names[] = {"Quick iterative", "Quick Hoare recursive", "Insertion"};

void verify(const Array& actual, const Array& expected) {
    if (actual != expected) throw std::runtime_error("Sort verification failed");
}

void tests() {
    std::vector<Array> cases = {{}, {3.5f}, {2, 1}, {1, 2, 3},
        {5, 4, 3, 2, 1}, {7, 7, 7, 7}, {2, -1, 2, 0, -1},
        {LOW, HIGH, LOW, HIGH}, {1.25f, 1.125f, -0.5f, 1.25f}};
    for (unsigned seed = 0; seed < 100; ++seed)
        cases.push_back(generate(seed * 3, seed));
    for (const auto& input : cases) {
        Array expected = input;
        std::sort(expected.begin(), expected.end());
        for (Sort sort : sorts) {
            Array actual = input;
            sort(actual);
            verify(actual, expected);
        }
    }
    std::cout << "Tests passed: " << cases.size() << " arrays x 3 algorithms\n";
}

void print(const Array& a) {
    for (std::size_t i = 0; i < a.size(); ++i)
        std::cout << std::setw(11) << a[i] << ((i + 1) % 5 ? ' ' : '\n');
    std::cout << '\n';
}

void demo() {
    Array input = generate(50, 12), expected = input;
    std::sort(expected.begin(), expected.end());
    std::cout << std::fixed << std::setprecision(2)
              << "Variant 12; float; N=50; [11020, 31702]; seed=12\nInput:\n";
    print(input);
    for (int k = 0; k < 3; ++k) {
        Array a = input;
        sorts[k](a);
        verify(a, expected);
        std::cout << names[k] << ": OK\n";
        print(a);
    }
}

void benchmark() {
    std::ofstream csv("timings.csv");
    if (!csv) throw std::runtime_error("Cannot create timings.csv");
    csv << "N,iterative_ms,hoare_ms,insertion_ms\n";
    for (std::size_t n : {1000, 10000, 100000, 500000, 1000000}) {
        Array input = generate(n, 12), expected = input;
        std::sort(expected.begin(), expected.end());
        csv << n;
        for (int k = 0; k < 3; ++k) {
            Array a = input; // Копирование не входит в замер.
            std::cout << "N=" << n << " " << names[k] << "..." << std::flush;
            auto start = std::chrono::steady_clock::now();
            sorts[k](a);
            auto end = std::chrono::steady_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start).count();
            verify(a, expected); // Проверка также вне замера.
            csv << ',' << std::fixed << std::setprecision(6) << ms;
            csv.flush();
            std::cout << ' ' << ms << " ms; OK\n";
        }
        csv << '\n';
        csv.flush();
    }
}

int main(int argc, char** argv) {
    try {
        std::string mode = argc > 1 ? argv[1] : "--demo";
        if (mode == "--demo") demo();
        else if (mode == "--test") tests();
        else if (mode == "--bench") benchmark();
        else throw std::runtime_error("Use --demo, --test or --bench");
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
