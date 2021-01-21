#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <algorithm>


static float rnd() {
    static std::random_device rd;
    static thread_local std::default_random_engine generator(rd());
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    return distribution(generator);
}

void thread_make_rnd_seq(std::vector<float>& a) {
    for(int i = 0; i < 100; i++)
        a.push_back(rnd());
}

int max_len_of_equal(std::vector<float>& a, std::vector<float>& b) {
    int tmp = 0;
    for(int i = 0; i < 100; i++) {
        for(int j = 0; j < 100; j++) {
            if(a[i] == b[j]) {
                int t = 0;
                while (a[i + t] == b[j + t]) {
                    t++;
                }
                tmp = t + 1;
            }
        }
    }

    return tmp;
}

int main() {
    std::vector<std::vector<float>> a;
    std::thread th[10];
    float resE[10];
    float resD[10];

    for(int i = 0; i < 10; i++) {
        a.push_back(std::vector<float>());
        th[i] = std::thread(thread_make_rnd_seq, std::ref(a[i]));
    }

    for(int i = 0; i < 10; i++) {
        std::cerr << "Waiting for threads, remaining:" << 9-i << std::flush;
        th[i].join();

        //check distribution to be uniform
        resE[i] = std::accumulate(a[i].begin(), a[i].end(), 0.0)/100;
        float tmp = 0.0;
        std::for_each(a[i].begin(), a[i].end(), [&] (float x) { tmp += x*x;});
        resD[i] = tmp/100 - resE[i];
    }


    //Report
    std::cout << "Results:\n-----------------------------------------------\n"
              << "1st step: checking uniformity\n";

    for(int i = 0; i < 10; i++) {
        std::cout << "Thread " << i << " E=" << resE[i] << " D=" << resD[i] <<'\n';
    }


    //max length of equal lines in matrix a
    int max_len = 0;
    for(int i = 0; i < 10; i++) {
        std::cerr << "Checking equality, remaining:" << 9-i << std::flush;
        for(int j = 9; j > i; j--) {
            int tmp = max_len_of_equal(a[i], a[j]);
            max_len = tmp > max_len ? tmp : max_len;
        }
    }

    //Report
    std::cout << "-----------------------------------------------\n"
              << "2nd step: max length of equal elements in distribution: " << max_len;

    return 0;
}
