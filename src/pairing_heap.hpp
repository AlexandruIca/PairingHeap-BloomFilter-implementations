#ifndef PAIRING_HEAP_HPP
#define PAIRING_HEAP_HPP
#pragma once

#include <iostream>
#include <memory>
#include <utility>

namespace sd {

namespace impl {

template<typename Key>
struct heap_node;

template<typename Key>
using ptr = std::shared_ptr<heap_node<Key>>;

template<typename Key>
struct heap_node
{
public:
    std::shared_ptr<heap_node> left_child{ nullptr };
    std::shared_ptr<heap_node> next_sibling{ nullptr };
    std::shared_ptr<heap_node> prev_sibling{ nullptr };
    Key value{};

    heap_node() noexcept = default;
    heap_node(heap_node const&) = default;
    heap_node(heap_node&&) noexcept = default;
    ~heap_node() noexcept = default;

    heap_node(Key new_value)
        : value{ new_value }
    {
    }

    auto operator=(heap_node const&) -> heap_node& = default;
    auto operator=(heap_node&&) noexcept -> heap_node& = default;

    ///
    /// \brief Makes the \ref left_child equal to node.
    ///
    auto add_as_child(std::shared_ptr<heap_node> node) -> void
    {
        if(left_child == nullptr) {
            left_child = node;
            return;
        }

        node->next_sibling = left_child;
        left_child->prev_sibling = node;
        left_child = node;
    }
};

///
/// \brief AKA compare-link, melding.
///
template<typename Key>
[[nodiscard]] auto merge(ptr<Key> a, ptr<Key> b) -> ptr<Key>
{
    if(a == nullptr) {
        return b;
    }
    if(b == nullptr) {
        return a;
    }

    if(a->value < b->value) {
        a->add_as_child(b);
        return a;
    }

    b->add_as_child(a);
    return b;
}

template<typename Key>
[[nodiscard]] auto insert(ptr<Key> node, Key const value) -> ptr<Key>
{
    return merge(node, std::make_shared<heap_node<Key>>(value));
}

template<typename Key>
[[nodiscard]] auto two_pass_merge(ptr<Key> node) -> ptr<Key>
{
    if(node == nullptr || node->next_sibling == nullptr) {
        return node;
    }

    auto a = node;
    auto b = node->next_sibling;
    auto new_node = b->next_sibling;

    a->next_sibling->prev_sibling = nullptr;
    a->next_sibling = nullptr;

    if(b->next_sibling != nullptr) {
        b->next_sibling->prev_sibling = nullptr;
        b->next_sibling = nullptr;
    }

    return merge(merge(a, b), two_pass_merge(new_node));
}

} // namespace impl

template<typename Key>
class pairing_heap
{
private:
    using heap_node_t = ::sd::impl::heap_node<Key>;
    std::shared_ptr<heap_node_t> m_root{ nullptr };
    int m_size{ 0 };

    auto traverse(heap_node_t const* const node) const noexcept -> void
    {
        if(node == nullptr) {
            return;
        }

        std::cout << node->value << ' ';

        this->traverse(node->left_child.get());
        this->traverse(node->next_sibling.get());
    }

public:
    pairing_heap() noexcept = default;
    pairing_heap(pairing_heap const&) = delete;
    pairing_heap(pairing_heap&&) noexcept = default;
    ~pairing_heap() noexcept = default;

    auto operator=(pairing_heap const&) -> pairing_heap& = delete;
    auto operator=(pairing_heap&&) noexcept -> pairing_heap& = default;

    [[nodiscard]] inline auto empty() const noexcept -> bool
    {
        return m_root == nullptr;
    }

    [[nodiscard]] inline auto size() const noexcept -> int
    {
        return m_size;
    }

    [[nodiscard]] inline auto get_min() const -> Key
    {
        return m_root->value;
    }

    auto insert(Key const value) -> void
    {
        m_root = impl::insert(m_root, value);
        ++m_size;
    }

    auto join(pairing_heap& other) -> void
    {
        m_root = impl::merge(m_root, other.m_root);
        m_size += other.size();
    }

    auto delete_min() -> void
    {
        m_root = impl::two_pass_merge(m_root->left_child);
        --m_size;
    }

    auto traverse() const -> void
    {
        this->traverse(m_root.get());
        std::cout << std::endl;
    }
};

} // namespace sd

#endif // !PAIRING_HEAP_HPP
