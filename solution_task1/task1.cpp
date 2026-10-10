#include <array>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    constexpr int N = 750, K = 10;
    constexpr double a = 100.0, b = 110.0;
    constexpr double width = (b - a) / K;
    unsigned seed = static_cast<unsigned>(std::time(nullptr));
    if (argc == 2) seed = static_cast<unsigned>(std::stoul(argv[1]));
    std::srand(seed);
    std::array<double, N> numbers{};
    std::array<int, K> counts{};
    for (double& x : numbers) {
        x = a + (b - a) * (static_cast<double>(std::rand()) / RAND_MAX);
    }
    for (double x : numbers) {
        int index = static_cast<int>((x - a) / width);
        if (index >= K) index = K - 1; // Правая граница b.
        ++counts[index];
    }
    std::cout << "N = " << N << ", seed = " << seed
              << "\nHistogram (one * = one number):\n";
    std::cout << std::fixed << std::setprecision(1);
    int total = 0;
    double max_deviation = 0;
    const double expected = static_cast<double>(N) / K;
    for (int i = 0; i < K; ++i) {
        std::cout << '[' << a + i * width << "; "
                  << a + (i + 1) * width << (i == K - 1 ? ']' : ')')
                  << ' ' << std::string(counts[i], '*')
                  << " (" << counts[i] << ")\n";
        total += counts[i];
        const double deviation = std::abs(counts[i] - expected);
        if (deviation > max_deviation) max_deviation = deviation;
    }
    std::cout << "Total: " << total << "\nExpected: " << expected
              << "\nMaximum deviation: " << max_deviation
              << " (" << 100 * max_deviation / expected << "%)\n";
}
