#ifndef PAIRING_HEAP_HPP
#define PAIRING_HEAP_HPP
#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

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
    heap_node* prev_sibling{ nullptr };
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
        left_child->prev_sibling = node.get();
        left_child = node;
    }
};

template<typename Key>
auto add_as_child(ptr<Key> parent, ptr<Key> child) -> void
{
    if(parent->left_child == nullptr) {
        parent->left_child = child;
        child->prev_sibling = parent.get();
        return;
    }

    child->next_sibling = parent->left_child;
    // parent->prev_sibling = child;
    parent->left_child = child;
    child->prev_sibling = parent.get();
    child->next_sibling->prev_sibling = child.get();
}

///
/// \brief AKA compare-link, meld.
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
        // a->add_as_child(b);
        add_as_child(a, b);
        return a;
    }

    // b->add_as_child(a);
    add_as_child(b, a);
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

    [[nodiscard]] auto find_node(heap_node_t const* const root,
                                 Key const value) const -> heap_node_t const*
    {
        if(root == nullptr) {
            return nullptr;
        }
        if(root->value == value) {
            return root;
        }

        auto find_left = find_node(root->left_child.get(), value);

        if(find_left != nullptr) {
            return find_left;
        }

        return find_node(root->next_sibling.get(), value);
    }

    [[nodiscard]] auto find_node(std::shared_ptr<heap_node_t> root,
                                 Key const value)
        -> std::shared_ptr<heap_node_t>
    {
        if(root == nullptr) {
            return nullptr;
        }
        if(root->value == value) {
            return root;
        }

        auto find_left = find_node(root->left_child, value);

        if(find_left != nullptr) {
            return find_left;
        }

        return find_node(root->next_sibling, value);
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

    auto delete_min() -> int
    {
        Key const min = m_root->value;
        int count{ 0 };

        while(min == m_root->value) {
            m_root = impl::two_pass_merge(m_root->left_child);
            --m_size;
            ++count;
        }

        return count;
    }

    [[nodiscard]] auto find2(Key const value) const -> heap_node_t const*
    {
        return this->find_node(m_root.get(), value);
    }

    [[nodiscard]] auto find(Key const value) -> std::shared_ptr<heap_node_t>
    {
        return this->find_node(m_root, value);
    }

    auto remove(Key const value) -> int
    {
        int count{ 0 };

        if(m_root == nullptr) {
            return count;
        }

        for(;; ++count) {
            if(m_root->value == value) {
                return this->delete_min();
            }

            auto node = this->find(value);

            if(node == nullptr) {
                return count;
            }

            // because they are shared pointers, nodes will not be deleted if
            // they have a sibling or a left child.
            // This is useful for not checking whether node is a left child
            // or a sibling.
            node->prev_sibling->next_sibling = nullptr;
            node->prev_sibling->left_child = nullptr;
            node->prev_sibling = nullptr;

            m_root = impl::merge(
                m_root,
                impl::merge(impl::two_pass_merge(node->left_child),
                            impl::two_pass_merge(node->next_sibling)));
            --m_size;
        }

        return count;
    }

    ///
    /// \brief Deletes all appearances of value, replaces them with a node
    ///        with value=new_value.
    ///
    auto modify(Key const value, Key const new_value) -> void
    {
        this->remove(value);
        this->insert(new_value);
    }

    ///
    /// \brief Performs node->value--;
    ///
    auto decrease_key(std::shared_ptr<heap_node_t> node) -> void
    {
        --node->value;
        if(node->next_sibling == nullptr || node->prev_sibling == nullptr) {
            return;
        }
        if(node->value >= node->prev_sibling->value) {
            return;
        }

        node->prev_sibling->left_child = nullptr;
        node->prev_sibling->next_sibling = nullptr;
        node->prev_sibling = nullptr;

        m_root = impl::merge(m_root, node);
    }

    [[nodiscard]] auto root() -> std::shared_ptr<heap_node_t>
    {
        return m_root;
    }

    auto traverse() const -> void
    {
        this->traverse(m_root.get());
        std::cout << std::endl;
    }
};

template<typename Key>
auto build_heap(pairing_heap<Key>& heap, std::vector<int> const& values) -> void
{
    for(int const num : values) {
        heap.insert(num);
    }
}

} // namespace sd

#endif // !PAIRING_HEAP_HPP
