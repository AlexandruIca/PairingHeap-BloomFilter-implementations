#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

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

    ASSERT(heap.get_min() == 2);
    ASSERT(heap.size() == 10);

    // heap.traverse();

    ASSERT(heap.remove(7) == 1);

    ASSERT(heap.size() == 9);

    // heap.traverse();

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

    // heap.traverse();
}

auto split(std::string const& str, std::string const& pattern)
    -> std::vector<std::string>
{
    std::vector<std::string> result{};
    std::string tmp{};

    for(char const c : str) {
        if(pattern.find(c) != std::string::npos) {
            if(!tmp.empty()) {
                result.push_back(tmp);
                tmp.clear();
            }
        }
        else {
            tmp.push_back(c);
        }
    }

    if(!tmp.empty()) {
        result.push_back(tmp);
    }

    return result;
}

auto to_nums(std::vector<std::string> const& v) -> std::vector<int>
{
    std::vector<int> result{};

    std::transform(v.begin(),
                   v.end(),
                   std::back_inserter(result),
                   [](std::string const& s) -> int { return std::stoi(s); });

    return result;
}

auto main(int, char*[]) noexcept -> int
{
    basic_test();

    std::ifstream f{ "test.txt" };

    if(!f) {
        std::cout << "Fisierul cu teste este gol sau a aparut o eroare!"
                  << std::endl;
        return EXIT_FAILURE;
    }

    int num_tests{ 0 };
    std::string line{};
    sd::pairing_heap<int> heap{};

    std::getline(f, line);
    num_tests = to_nums(split(line, " "))[0];

    for(int i = 0; i < num_tests; ++i) {
        std::getline(f, line);
        auto const nums = to_nums(split(line, " "));

        if(nums.size() <= 0) {
            std::cout << "Input invalid la linia: " << i + 2 << std::endl;
            return EXIT_FAILURE;
        }

        switch(nums[0]) {
        case 1: {
            heap.insert(nums[1]);
            break;
        }
        case 2: {
            int const times = heap.remove(nums[1]);
            std::cout << "Am sters " << nums[1] << " de " << times << " ori!"
                      << std::endl;
            break;
        }
        case 3: {
            std::cout << "Minimul este: " << heap.get_min() << std::endl;
            break;
        }
        case 4: {
            std::cout << "Am sters minimul de " << heap.delete_min() << " ori!"
                      << std::endl;
            break;
        }
        default: {
            break;
        }
        }
    }
}
