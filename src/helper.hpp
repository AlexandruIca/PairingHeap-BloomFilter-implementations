#ifndef HELPER_HPP
#define HELPER_HPP
#pragma once

#define ASSERT(...)                                                            \
    do {                                                                       \
        if(!(__VA_ARGS__)) {                                                   \
            std::cout << "Error at line: " << __LINE__                         \
                      << ", file: " << __FILE__ << ":\n\t" << #__VA_ARGS__     \
                      << std::endl;                                            \
        }                                                                      \
    } while(false)

#endif // !HELPER_HPP
