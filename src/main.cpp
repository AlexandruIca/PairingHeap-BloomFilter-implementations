#include <iostream>

#include "helper.hpp"
#include "pairing_heap.hpp"

auto basic_test() -> void
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

    sd::build_heap(heap2, { 10, 1, 1, 11 });

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

    ASSERT(heap.remove(7) == 1);

    ASSERT(heap.size() == 9);

    heap.traverse();

    heap.insert(25);
    heap.insert(25);
    heap.insert(25);

    ASSERT(heap.remove(25) == 3);
    ASSERT(heap.remove(170) == 0);
    ASSERT(heap.size() == 9);
    ASSERT(heap.get_min() == 2);

    heap.insert(13);
    heap.insert(13);
    heap.modify(13, 26);

    ASSERT(heap.size() == 10);

    heap.traverse();
}

auto main(int, char*[]) noexcept -> int
{
    basic_test();
}
