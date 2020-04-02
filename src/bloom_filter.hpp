#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP
#pragma once

#include <bitset>
#include <functional>
#include <random>

namespace sd {

///
/// Instead of double hashing use a random number generator(in this case
/// std::minstd_rand) ang give it the hash of the value as the seed.
/// Then call that rng for how many times you want(number of hashes).
/// This should make the bloom filter more predictable.
///
/// \tparam Size how many bits to store.
///
template<typename Key, int Size, typename Hash = std::hash<Key>>
class mixer_basic
{
private:
    Hash m_hash{};
    std::minstd_rand m_rng{};

public:
    mixer_basic() noexcept = default;
    mixer_basic(mixer_basic const&) noexcept = default;
    mixer_basic(mixer_basic&&) noexcept = default;
    ~mixer_basic() noexcept = default;

    mixer_basic(Key const& value)
        : m_rng{ m_hash(value) }
    {
    }

    auto operator=(mixer_basic const&) noexcept -> mixer_basic& = default;
    auto operator=(mixer_basic&&) noexcept -> mixer_basic& = default;

    [[nodiscard]] auto operator()() const noexcept -> std::size_t
    {
        return m_rng() % static_cast<std::size_t>(Size);
    }

    [[nodiscard]] constexpr auto bit_count() const noexcept -> int
    {
        return Size;
    }
};

///
/// \brief Same as \ref mixer but aligned to the typical size of a cache line
///        (64 bytes). This should make it faster.
///
template<typename Key,
         int Size,
         typename Hash = std::hash<Key>,
         int Align = 512>
class cache_friendly_mixer
{
private:
    Hash m_hash{};
    mutable std::minstd_rand m_rng{};
    std::size_t m_base_offset{ 0u };

public:
    cache_friendly_mixer() noexcept = default;
    cache_friendly_mixer(cache_friendly_mixer const&) noexcept = default;
    cache_friendly_mixer(cache_friendly_mixer&&) noexcept = default;
    ~cache_friendly_mixer() noexcept = default;

    cache_friendly_mixer(Key const& value)
        : m_rng{ m_hash(value) }
        , m_base_offset{ (m_rng() % (Size / Align)) * Align }
    {
        static_assert(Size % Align == 0, "Memory access violation!");
    }

    auto operator=(cache_friendly_mixer const&) noexcept
        -> cache_friendly_mixer& = default;
    auto operator=(cache_friendly_mixer&&) noexcept
        -> cache_friendly_mixer& = default;

    [[nodiscard]] auto operator()() const noexcept -> std::size_t
    {
        return m_base_offset + (m_rng() % Align);
    }

    [[nodiscard]] constexpr auto bit_count() const noexcept -> int
    {
        return Size;
    }
};

///
/// false positive probability:
/// ```cpp
/// std::pow(
///     1.0 - std::exp(-1.0 * NumHashes * num_inserted / Size),
///     1.0 * NumHashes);
/// ```
///
template<typename Key,
         int Size = 512,
         int NumHashes = 7,
         typename Mixer = cache_friendly_mixer<Key, Size>>
class bloom_filter
{
private:
    alignas(64) std::bitset<Size> m_bits{};

public:
    constexpr bloom_filter() noexcept = default;
    bloom_filter(bloom_filter const&) noexcept = default;
    bloom_filter(bloom_filter&&) noexcept = default;
    ~bloom_filter() noexcept = default;

    auto operator=(bloom_filter const&) noexcept -> bloom_filter& = default;
    auto operator=(bloom_filter&&) noexcept -> bloom_filter& = default;

    constexpr auto insert(Key const& value) -> void
    {
        Mixer m{ value };

        for(int n = 0; n < NumHashes; ++n) {
            m_bits[m()] = true;
        }
    }

    constexpr auto contains(Key const& value) const noexcept -> bool
    {
        Mixer m{ value };

        for(int n = 0; n < NumHashes; ++n) {
            if(!m_bits[m()]) {
                return false;
            }
        }

        return true;
    }
};

} // namespace sd

#endif // !BLOOM_FILTER_HPP
