#include <iostream>

#include "helper.hpp"
#include "pairing_heap.hpp"

auto main(int, char*[]) noexcept -> int
{
    sd::pairing_heap<int> heap{};

    ASSERT(heap.empty() == true);
    ASSERT(heap.size() == 0);

    heap.insert(5);
    heap.insert(4);
    heap.insert(2);
    heap.insert(7);
    heap.insert(9);
    heap.insert(3);
    heap.insert(6);
    heap.insert(8);

    ASSERT(heap.size() == 8);
    ASSERT(heap.get_min() == 2);

    sd::pairing_heap<int> heap2{};

    heap2.insert(10);
    heap2.insert(1);
    heap2.insert(1);
    heap2.insert(11);

    ASSERT(heap2.size() == 4);
    ASSERT(heap2.get_min() == 1);

    heap.join(heap2);

    ASSERT(heap.size() == 12);
    ASSERT(heap.get_min() == 1);

    heap.delete_min();

    ASSERT(heap.get_min() == 1);
    ASSERT(heap.size() == 11);

    heap.delete_min();

    ASSERT(heap.get_min() == 2);
    ASSERT(heap.size() == 10);

    heap.traverse();
}
