//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <chrono>

// duration

// static constexpr duration zero();

#include <chrono>
#include <cassert>

#include "../../rep.h"

template <class D>
void test()
{
    typedef typename D::rep Rep;
    Rep zero_rep = std::chrono::duration_values<Rep>::zero();
    assert(D::zero().count() == zero_rep);
}

int main()
{
    test<std::chrono::duration<int> >();
    test<std::chrono::duration<Rep> >();
}
