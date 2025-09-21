#pragma once

#include <algorithm>
#include <flat_map>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>

#include "book_database.hpp"

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
    requires requires { typename Comparator::is_transparent; }
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string_view, std::size_t, Comparator> fl{comp};

    std::for_each(cont.cbegin(), cont.cend(), [&fl](const Book &book) {
        auto author = book.author;
        fl[author]++;
    });

    return fl;
}

///
///
///

template <BookIterator It>
auto calculateGenreRating(It begin, It end) {
    std::flat_map<Genre, std::pair<std::size_t, double>> fl{};

    std::for_each(begin, end, [&fl](const Book &book) {
        auto genre = book.genre;
        auto &stats = fl[genre];
        stats.first += 1;
        stats.second += book.rating;
    });

    std::flat_map<Genre, double> rt{};
    for (auto i = fl.cbegin(); i != fl.cend(); ++i) {
        rt.try_emplace(i->first, i->second.second / i->second.first);
    }

    return rt;
}

///
///
///

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &cont) {
    if (cont.size() == 0) {
        return 0.0;
    }

    return std::accumulate(cont.cbegin(), cont.cend(), 0.0,
                           [](double accum, const Book &book) { return accum + book.rating; }) /
           cont.size();
}

///
///
///

template <BookContainerLike T>
std::vector<std::reference_wrapper<const Book>> sampleRandomBook(const BookDatabase<T> &cont, std::size_t n) {
    std::vector<std::reference_wrapper<const Book>> rt;
    rt.reserve(n);

    if (n > cont.size()) {
        n = cont.size();
    }

    std::sample(cont.cbegin(), cont.cend(), std::back_inserter(rt), n, std::mt19937{std::random_device{}()});
    return rt;
}

///
///
///

template <BookContainerLike T, BookComparator Comporator = comp::GreaterByRating>
std::vector<std::reference_wrapper<const Book>> getTopN(BookDatabase<T> &cont, std::size_t n, Comporator comp = {}) {
    std::vector<std::reference_wrapper<const Book>> rt;
    rt.reserve(n);

    std::partial_sort(cont.begin(), std::next(cont.begin(), n), cont.end(), comp);
    for (auto i = cont.cbegin(); i != std::next(cont.begin(), n); ++i) {
        rt.emplace_back(*i);
    }

    return rt;
}

}  // namespace bookdb
