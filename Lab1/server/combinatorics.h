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

    long long BinPow(long long a, long long x) {
        if (x == 0) {
            return 1;
        } else if (x & 1) {
            return (BinPow(a, x - 1) * a) % kMod ;
        } else {
            long long tmp = BinPow(a, x / 2);
            return (tmp * tmp) % kMod;
        }
    }

    long long C(int n, int r) {
        auto invdenominator = (invfact[r] * invfact[n - r]) % kMod;
        return (factorial[n] * invdenominator) % kMod;
    }

    long long getFactorial(long long n) {
        if (n < 0 || n >= kMaxN) {
            throw std::out_of_range("Invalid range");
        }
        return factorial[n];
    }

    long long getSumOfFactorials(long long n, long long m) {
        return (getFactorial(n) + getFactorial(m)) % kMod;
    }

private:
    static const int kMaxN = 1e6, kMod = 1e9 + 7;
    std::vector<long long> factorial;
    std::vector<long long> invfact;
};