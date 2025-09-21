#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book &ths, const Book &other) const noexcept { return ths.author < other.author; }
};

struct LessByRating {
    bool operator()(const Book &ths, const Book &other) const noexcept { return ths.rating < other.rating; }
};

struct GreaterByRating {
    bool operator()(const Book &ths, const Book &other) const noexcept { return ths.rating > other.rating; }
};

}  // namespace bookdb::comp