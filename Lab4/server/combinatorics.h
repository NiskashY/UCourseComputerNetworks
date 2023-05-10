#pragma once

#include <iostream>
#include <vector>
#include <iostream>
#include <vector>
#include <array>

class Combinatorics {
public:
    Combinatorics() {
        factorial.resize(kMaxN, 1);
        invfact.resize(kMaxN, 1);

        for (int i = 1; i < kMaxN; ++i) {
            factorial[i] = (factorial[i - 1] * i) % kMod;
        }

        int n = kMaxN - 1;
        invfact[n] = BinPow(factorial[n], kMod - 2);

        for (int i = n; i > 0; --i) {
            invfact[i - 1] = invfact[i] * i % kMod;
        }
    }

    int32_t BinPow(int32_t a, int32_t x) {
        if (x == 0) {
            return 1;
        } else if (x & 1) {
            return (BinPow(a, x - 1) * a) % kMod ;
        } else {
            int32_t tmp = BinPow(a, x / 2);
            return (tmp * tmp) % kMod;
        }
    }

    int32_t C(int n, int r) {
        auto invdenominator = (invfact[r] * invfact[n - r]) % kMod;
        return (factorial[n] * invdenominator) % kMod;
    }

    int32_t getFactorial(int32_t n) {
        if (n < 0 || n >= kMaxN) {
            throw std::out_of_range("Invalid range");
        }
        return factorial[n];
    }

    int32_t getSumOfFactorials(int32_t n, int32_t m) {
        return (getFactorial(n) + getFactorial(m)) % kMod;
    }

private:
    static const int kMaxN = 1e6, kMod = 1e9 + 7;
    std::vector<int32_t> factorial;
    std::vector<int32_t> invfact;
};
