//===-------------------------- hash.cpp ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "__hash_table"
#include "algorithm"

_LIBCPP_BEGIN_NAMESPACE_STD

namespace {

// handle all next_prime(i) for i in [1, 210), special case 0
const unsigned small_primes[] =
{
    0,
    2,
    3,
    5,
    7,
    11,
    13,
    17,
    19,
    23,
    29,
    31,
    37,
    41,
    43,
    47,
    53,
    59,
    61,
    67,
    71,
    73,
    79,
    83,
    89,
    97,
    101,
    103,
    107,
    109,
    113,
    127,
    131,
    137,
    139,
    149,
    151,
    157,
    163,
    167,
    173,
    179,
    181,
    191,
    193,
    197,
    199,
    211
};

// potential primes = 210*k + indices[i], k >= 1
//   these numbers are not divisible by 2, 3, 5 or 7
//   (or any integer 2 <= j <= 10 for that matter).
const unsigned indices[] =
{
    1,
    11,
    13,
    17,
    19,
    23,
    29,
    31,
    37,
    41,
    43,
    47,
    53,
    59,
    61,
    67,
    71,
    73,
    79,
    83,
    89,
    97,
    101,
    103,
    107,
    109,
    113,
    121,
    127,
    131,
    137,
    139,
    143,
    149,
    151,
    157,
    163,
    167,
    169,
    173,
    179,
    181,
    187,
    191,
    193,
    197,
    199,
    209
};

}

// Returns:  If n == 0, returns 0.  Else returns the lowest prime number that
// is greater than or equal to n.
// 
// The algorithm creates a list of small primes, plus an open-ended list of
// potential primes.  All prime numbers are potential prime numbers.  However
// some potential prime numbers are not prime.  In an ideal world, all potential
// prime numbers would be prime.  Candiate prime numbers are chosen as the next
// highest potential prime.  Then this number is tested for prime by dividing it
// by all potential prime numbers less than the sqrt of the candidate.
// 
// This implementation defines potential primes as those numbers not divisible
// by 2, 3, 5, and 7.  Other (common) implementations define potential primes
// as those not divisible by 2.  A few other implementations define potential
// primes as those not divisible by 2 or 3.  By raising the number of small
// primes which the potential prime is not divisible by, the set of potential
// primes more closely approximates the set of prime numbers.  And thus there
// are fewer potential primes to search, and fewer potential primes to divide
// against.

size_t
__next_prime(size_t n)
{
    const size_t L = 210;
    const size_t N = sizeof(small_primes) / sizeof(small_primes[0]);
    // If n is small enough, search in small_primes
    if (n <= small_primes[N-1])
        return *std::lower_bound(small_primes, small_primes + N, n);
    // Else n > largest small_primes
    // Start searching list of potential primes: L * k0 + indices[in]
    const size_t M = sizeof(indices) / sizeof(indices[0]);
    // Select first potential prime >= n
    //   Known a-priori n >= L
    size_t k0 = n / L;
    size_t in = std::lower_bound(indices, indices + M, n % L) - indices;
    n = L * k0 + indices[in];
    while (true)
    {
        // Divide n by all primes or potential primes (i) until:
        //    1.  The division is even, so try next potential prime.
        //    2.  The i > sqrt(n), in which case n is prime.
        // It is known a-priori that n is not divisible by 2, 3, 5 or 7,
        //    so don't test those (j == 5 ->  divide by 11 first).  And the
        //    potential primes start with 211, so don't test against the last
        //    small prime.
        for (size_t j = 5; j < N - 1; ++j)
        {
            if (n % small_primes[j] == 0)
                goto next;
            if (n / small_primes[j] < small_primes[j])
                return n;
        }
        // n wasn't divisible by small primes, try potential primes
        {
            size_t i = 211;
            while (true)
            {
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 10;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 8;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 8;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 6;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 4;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 2;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                i += 10;
                if (n % i == 0)
                    break;
                if (n / i < i)
                    return n;

                // This will loop i to the next "plane" of potential primes
                i += 2;
            }
        }
next:
        // n is not prime.  Increment n to next potential prime.
        if (++in == M)
        {
            ++k0;
            in = 0;
        }
        n = L * k0 + indices[in];
    }
}

_LIBCPP_END_NAMESPACE_STD
