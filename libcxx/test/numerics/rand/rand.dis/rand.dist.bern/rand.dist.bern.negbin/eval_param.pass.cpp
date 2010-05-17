//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <random>

// template<class IntType = int>
// class negative_binomial_distribution

// template<class _URNG> result_type operator()(_URNG& g, const param_type& parm);

#include <random>
#include <numeric>
#include <vector>
#include <cassert>

template <class T>
inline
T
sqr(T x)
{
    return x * x;
}

int main()
{
    {
        typedef std::negative_binomial_distribution<> D;
        typedef D::param_type P;
        typedef std::minstd_rand G;
        G g;
        D d(16, .75);
        P p(5, .75);
        const int N = 1000000;
        std::vector<D::result_type> u;
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g, p);
            assert(d.min() <= v && v <= d.max());
            u.push_back(v);
        }
        double mean = std::accumulate(u.begin(), u.end(),
                                              double(0)) / u.size();
        double var = 0;
        double skew = 0;
        double kurtosis = 0;
        for (int i = 0; i < u.size(); ++i)
        {
            double d = (u[i] - mean);
            double d2 = sqr(d);
            var += d2;
            skew += d * d2;
            kurtosis += d2 * d2;
        }
        var /= u.size();
        double dev = std::sqrt(var);
        skew /= u.size() * dev * var;
        kurtosis /= u.size() * var * var;
        kurtosis -= 3;
        double x_mean = p.k() * (1 - p.p()) / p.p();
        double x_var = x_mean / p.p();
        double x_skew = (2 - p.p()) / std::sqrt(p.k() * (1 - p.p()));
        double x_kurtosis = 6. / p.k() + sqr(p.p()) / (p.k() * (1 - p.p()));
        assert(std::abs(mean - x_mean) / x_mean < 0.01);
        assert(std::abs(var - x_var) / x_var < 0.01);
        assert(std::abs(skew - x_skew) / x_skew < 0.01);
        assert(std::abs(kurtosis - x_kurtosis) / x_kurtosis < 0.01);
    }
    {
        typedef std::negative_binomial_distribution<> D;
        typedef D::param_type P;
        typedef std::mt19937 G;
        G g;
        D d(16, .75);
        P p(30, .03125);
        const int N = 1000000;
        std::vector<D::result_type> u;
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g, p);
            assert(d.min() <= v && v <= d.max());
            u.push_back(v);
        }
        double mean = std::accumulate(u.begin(), u.end(),
                                              double(0)) / u.size();
        double var = 0;
        double skew = 0;
        double kurtosis = 0;
        for (int i = 0; i < u.size(); ++i)
        {
            double d = (u[i] - mean);
            double d2 = sqr(d);
            var += d2;
            skew += d * d2;
            kurtosis += d2 * d2;
        }
        var /= u.size();
        double dev = std::sqrt(var);
        skew /= u.size() * dev * var;
        kurtosis /= u.size() * var * var;
        kurtosis -= 3;
        double x_mean = p.k() * (1 - p.p()) / p.p();
        double x_var = x_mean / p.p();
        double x_skew = (2 - p.p()) / std::sqrt(p.k() * (1 - p.p()));
        double x_kurtosis = 6. / p.k() + sqr(p.p()) / (p.k() * (1 - p.p()));
        assert(std::abs(mean - x_mean) / x_mean < 0.01);
        assert(std::abs(var - x_var) / x_var < 0.01);
        assert(std::abs(skew - x_skew) / x_skew < 0.01);
        assert(std::abs(kurtosis - x_kurtosis) / x_kurtosis < 0.01);
    }
    {
        typedef std::negative_binomial_distribution<> D;
        typedef D::param_type P;
        typedef std::mt19937 G;
        G g;
        D d(16, .75);
        P p(40, .25);
        const int N = 1000000;
        std::vector<D::result_type> u;
        for (int i = 0; i < N; ++i)
        {
            D::result_type v = d(g, p);
            assert(d.min() <= v && v <= d.max());
            u.push_back(v);
        }
        double mean = std::accumulate(u.begin(), u.end(),
                                              double(0)) / u.size();
        double var = 0;
        double skew = 0;
        double kurtosis = 0;
        for (int i = 0; i < u.size(); ++i)
        {
            double d = (u[i] - mean);
            double d2 = sqr(d);
            var += d2;
            skew += d * d2;
            kurtosis += d2 * d2;
        }
        var /= u.size();
        double dev = std::sqrt(var);
        skew /= u.size() * dev * var;
        kurtosis /= u.size() * var * var;
        kurtosis -= 3;
        double x_mean = p.k() * (1 - p.p()) / p.p();
        double x_var = x_mean / p.p();
        double x_skew = (2 - p.p()) / std::sqrt(p.k() * (1 - p.p()));
        double x_kurtosis = 6. / p.k() + sqr(p.p()) / (p.k() * (1 - p.p()));
        assert(std::abs(mean - x_mean) / x_mean < 0.01);
        assert(std::abs(var - x_var) / x_var < 0.01);
        assert(std::abs(skew - x_skew) / x_skew < 0.01);
        assert(std::abs(kurtosis - x_kurtosis) / x_kurtosis < 0.03);
    }
}
