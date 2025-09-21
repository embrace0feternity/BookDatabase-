#pragma once

#include <algorithm>
#include <functional>

#include "book.hpp"
#include "concepts.hpp"
#include <vector>

namespace bookdb {

auto YearBetween(int min, int max) noexcept {
    return [min, max](const Book &book) { return book.year >= min && book.year < max; };
}

///
///
///

auto RatingAbove(double rating) noexcept {
    return [rating](const Book &book) { return book.rating >= rating; };
}

///
///
///

auto GenreIs(std::string_view genre) noexcept {
    return [genre](const Book &book) { return StringFromGenre(book.genre) == genre; };
}

auto GenreIs(Genre genre) noexcept {
    return [genre](const Book &book) noexcept { return book.genre == genre; };
}

///
///
///

template <BookPredicate... Predicate>
auto all_of(Predicate... predicate) {
    return [predicate...](const Book &book) { return (predicate(book) && ...); };
}

template <BookPredicate... Predicate>
auto any_of(Predicate... predicate) {
    return [predicate...](const Book &book) { return (predicate(book) || ...); };
}

///
///
///

template <BookIterator It, typename Filter>
std::vector<std::reference_wrapper<const Book>> filterBooks(It begin, It end, Filter filter) {
    std::vector<std::reference_wrapper<const Book>> rt;

    for (; begin != end; ++begin) {
        if (filter(*begin)) {
            rt.push_back(*begin);
        }
    }

    return rt;
}

}  // namespace bookdb