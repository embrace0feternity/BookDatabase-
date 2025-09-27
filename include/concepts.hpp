#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

// template <typename T>
// concept BookContainerLike = true;

template <typename T>
concept BookContainerLike = requires(T container, typename T::value_type book) {
    typename T::value_type;
    typename T::size_type;
    typename T::iterator;
    typename T::const_iterator;
    typename T::reverse_iterator;
    typename T::const_reverse_iterator;

    requires std::same_as<typename T::value_type, bookdb::Book>;

    { container.begin() } -> std::same_as<typename T::iterator>;
    { container.cbegin() } -> std::same_as<typename T::const_iterator>;
    { container.rbegin() } -> std::same_as<typename T::reverse_iterator>;
    { container.crbegin() } -> std::same_as<typename T::const_reverse_iterator>;
    { container.end() } -> std::same_as<typename T::iterator>;
    { container.cend() } -> std::same_as<typename T::const_iterator>;
    { container.rend() } -> std::same_as<typename T::reverse_iterator>;
    { container.crend() } -> std::same_as<typename T::const_reverse_iterator>;
    { container.size() } -> std::same_as<typename T::size_type>;
    { container.clear() } -> std::same_as<void>;
    { container.push_back(book) } -> std::same_as<void>;
};

template <typename It>
concept BookIterator = requires(It it) {
    requires std::input_iterator<It>;

    typename std::iterator_traits<It>::value_type;
    requires std::same_as<typename std::iterator_traits<It>::value_type, bookdb::Book>;

    { *it } -> std::same_as<typename std::iterator_traits<It>::reference>;

    { ++it } -> std::same_as<It &>;
    { it++ } -> std::same_as<It>;
};

template <typename P>
concept BookPredicate = requires(P predicate, const bookdb::Book &book) {
    { predicate(book) } -> std::same_as<bool>;
};

template <typename C>
concept BookComparator = requires(C c, const Book &ths, const Book &other) {
    { c(ths, other) } -> std::same_as<bool>;
};

}  // namespace bookdb