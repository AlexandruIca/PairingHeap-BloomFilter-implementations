#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP
#pragma once

#include <bitset>
#include <functional>

namespace sd {

template<typename Key, int Size, typename Hash = std::hash<Key>>
class bloom_filter
{
private:
    std::bitset<Size> m_bits{};

public:
    constexpr bloom_filter() noexcept = default;
    bloom_filter(bloom_filter const&) noexcept = default;
    bloom_filter(bloom_filter&&) noexcept = default;
    ~bloom_filter() noexcept = default;

    auto operator=(bloom_filter const&) noexcept -> bloom_filter& = default;
    auto operator=(bloom_filter&&) noexcept -> bloom_filter& = default;

    constexpr auto insert(Key const& value) -> void
    {
        m_bits.set(Hash{}(value) % Size);
    }

    constexpr auto contains(Key const& value) const noexcept -> bool
    {
        return m_bits[Hash{}(value) % Size];
    }
};

} // namespace sd

#endif // !BLOOM_FILTER_HPP
