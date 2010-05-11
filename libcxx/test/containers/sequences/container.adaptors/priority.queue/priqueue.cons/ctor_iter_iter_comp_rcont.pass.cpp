//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <queue>

// template <class InputIterator>
//   priority_queue(InputIterator first, InputIterator last,
//                  const Compare& comp, container_type&& c);

#include <queue>
#include <cassert>

#include "../../../../MoveOnly.h"

int main()
{
#ifdef _LIBCPP_MOVE
    int a[] = {3, 5, 2, 0, 6, 8, 1};
    const int n = sizeof(a)/sizeof(a[0]);
    std::priority_queue<MoveOnly> q(a+n/2, a+n,
                                    std::less<MoveOnly>(),
                                    std::vector<MoveOnly>(a, a+n/2));
    assert(q.size() == n);
    assert(q.top() == MoveOnly(8));
#endif
}
