#include <iostream>
#include <iomanip>
#include <random>
#include <thread>
#include <vector>
#include <algorithm>

constexpr int MAX_THREADS = 10;
constexpr int MAX_VALUE = 100;
constexpr int AMOUNT = 100 * MAX_VALUE;

template < class T >
T sqr(T x) {
    return x * x;
}

static float rnd() {
    static std::random_device rd;
    static thread_local std::default_random_engine generator(rd());
    static std::uniform_real_distribution<float> distribution(0, MAX_VALUE);
    return distribution(generator);
}

void thread_fill_rnd_seq(std::vector<float>& a) {
    a.reserve(AMOUNT);
    for(int i = 0; i < AMOUNT; i++)
        a.push_back(rnd());
}

template < class T >
T mean(const std::vector<T>& a) {
    return std::accumulate(a.begin(), a.end(), static_cast<T>(0)) / a.size();
}

template < class T >
T dispertion(const std::vector<T>& a) {
    auto m = mean(a);
    T d = 0;

    for (const auto& item : a)
        d += sqr(item - m);
    return d / a.size();
}

template < class T >
std::vector<int> to_int(const std::vector<T>& a) {
    std::vector<int> result;
    result.reserve(a.size());

    for (const auto& item : a)
        result.push_back(static_cast<int>(item));

    return result;
}

int LCS(const std::vector<int>& a, const std::vector<int>& b) {
    const int n = a.size();
    const int m = b.size();
    int result = 0;
    std::vector<std::vector<int>> DP(n+1, std::vector<int>(m+1, 0));
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= m; j++) {
            if (i == 0 || j == 0)
                DP[i][j] = 0;
            else if (a[i-1] == b[j-1]) {
                DP[i][j] = DP[i-1][j-1] + 1;
                result = std::max(result, DP[i][j]);
            } else
                DP[i][j] = 0;
        }
    return result;
}

int main() {
    std::vector<std::vector<float>> a(MAX_THREADS);
    std::vector<float> e(MAX_THREADS);
    std::vector<float> d(MAX_THREADS);
    std::vector<std::thread> threads;

    for(int i = 0; i < MAX_THREADS; i++)
        threads.emplace_back(std::thread(thread_fill_rnd_seq, std::ref(a[i])));

    for(int i = 0; i < MAX_THREADS; i++) {
        std::cerr << "Waiting for threads, remaining: " << MAX_THREADS-1-i << std::endl;
        threads[i].join();

        e[i] = mean(a[i]);
        d[i] = dispertion(a[i]);
    }
    std::cerr << std::endl;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "E {" << 0.5 * MAX_VALUE << "} = ";
    for (int i = 0; i < MAX_THREADS; ++i)
        std::cout << e[i] << ' ';
    std::cout << "\nD {" << sqr(MAX_VALUE) / 12.f << "} = ";
    for (int i = 0; i < MAX_THREADS; ++i)
        std::cout << d[i] << ' ';
    std::cout << std::endl;

    for (int i = 0; i < MAX_THREADS; ++i)
        for (int j = i+1; j < MAX_THREADS; ++j)
            std::cout << "LCS[" << i+1 << "][" << j+1 << "] = " << LCS(to_int(a[i]), to_int(a[j])) << std::endl;

    return 0;
}
