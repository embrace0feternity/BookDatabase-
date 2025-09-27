#include "book.hpp"
#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

#include <cmath>
#include <gtest/gtest.h>
#include <vector>

namespace b = bookdb;

class BookDBTestGroup : public testing::Test {
protected:
    b::BookDatabase<std::vector<b::Book>> db;

    b::Book book_5{"Pride and Prejudice", "Jane Austen", 1813, b::Genre::Fiction, 8.7, 178};
    b::Book book_6{"The Catcher in the Rye", "J.D. Salinger", 1951, b::Genre::Biography, 4.3, 112};
};

///
///
///

TEST_F(BookDBTestGroup, convertion) {
    {
        constexpr std::string_view genre = b::StringFromGenre(b::Genre::Mystery);
        ASSERT_EQ(genre, std::string_view{"Mystery"});
    }
    {
        constexpr auto genre = b::GenreFromString(std::string_view{"Mystery"});
        ASSERT_TRUE(genre == b::Genre::Mystery);
    }
}

///
///
///

TEST_F(BookDBTestGroup, pushBook) {
    db.emplaceBack("1984", "George Orwell", 1949, b::Genre::SciFi, 3., 190);
    db.emplaceBack("Animal Farm", "George Orwell", 1945, b::Genre::Fiction, 2.2, 143);
    db.emplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, b::Genre::Fiction, 7.4, 120);
    db.emplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, b::Genre::Fiction, 1.5, 156);

    auto &authors = db.getAuthors();
    auto &books = db.getBooks();
    ASSERT_EQ(4, books.size());
    ASSERT_EQ(3, authors.size());

    /// Check author std::string_view life time
    {
        db.pushBack(book_5);
        db.pushBack(std::move(book_6));
    }

    ASSERT_EQ(6, books.size());
    ASSERT_EQ(5, authors.size());

    ASSERT_FALSE(&book_5.author == &books[4].author);
    ASSERT_TRUE(book_5.author == books[4].author);

    ASSERT_FALSE(&book_6.author == &books[5].author);
    ASSERT_TRUE(book_6.author == books[5].author);
}

///
///
///

TEST_F(BookDBTestGroup, algoritmsWithDB) {
    /// Sort
    ASSERT_TRUE(book_5.author > book_6.author);
    ASSERT_TRUE(book_6.author > std::string_view{"F. Scott Fitzgerald"});
    db.pushBack(book_5);
    db.pushBack(book_6);
    db.emplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, b::Genre::Fiction, 4.5, 120);
    auto &books = db.getBooks();
    ASSERT_TRUE(books.front().author > books.back().author);
    std::sort(db.begin(), db.end(), b::comp::LessByAuthor{});
    ASSERT_TRUE(books.front().author < books.back().author);

    /// Is sorted
    bool st = std::is_sorted(db.cbegin(), db.cend(), b::comp::LessByAuthor{});
    ASSERT_TRUE(st);
    st = std::is_sorted(db.cbegin(), db.cend(), b::comp::LessByRating{});
    ASSERT_FALSE(st);

    /// min/max. The rating sequence is 4.5, 4.3, 4.7
    auto it = std::min_element(db.cbegin(), db.cend(), b::comp::LessByRating{});
    ASSERT_EQ(it->rating, 4.3);
    it = std::max_element(db.cbegin(), db.cend(), b::comp::LessByRating{});
    ASSERT_EQ(it->rating, 8.7);
}

///
///
///

TEST_F(BookDBTestGroup, stats) {
    db.emplaceBack("1984", "George Orwell", 1949, b::Genre::SciFi, 3., 190);
    db.emplaceBack("Animal Farm", "George Orwell", 1945, b::Genre::Fiction, 2.2, 143);
    db.emplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, b::Genre::Fiction, 7.4, 120);
    db.emplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, b::Genre::Fiction, 1.5, 156);
    db.emplaceBack(std::move(book_5));
    db.pushBack(book_6);

    /// Make a histogram
    {
        auto fl = b::buildAuthorHistogramFlat(db, b::TransparentStringLess{});
        ASSERT_EQ(5, fl.size());

        auto it = fl.find(std::string_view{"George Orwell"});
        ASSERT_EQ(it->second, 2);
        it = fl.find(std::string_view{"F. Scott Fitzgerald"});
        ASSERT_EQ(it->second, 1);
    }

    /// Calculate genre rating
    {
        auto rt = b::calculateGenreRating(db.begin(), db.end());
        auto fiction = rt.find(book_5.genre);
        ASSERT_TRUE(0.0001 > std::fabs(fiction->second - (book_5.rating + 2.2 + 7.4 + 1.5) / 4));
        auto biography = rt.find(book_6.genre);
        ASSERT_TRUE(0.0001 > std::fabs(biography->second - book_6.rating));
    }

    /// Calculate average rating
    {
        auto rt = b::calculateAverageRating(db);
        ASSERT_TRUE(0.01 > std::fabs(rt - (book_5.rating + book_6.rating + 2.2 + 7.4 + 1.5 + 3.) / 6));
    }

    /// Sample N
    {
        auto rt = b::sampleRandomBook(db, 1);
        ASSERT_EQ(rt.size(), 1);

        auto rt_all = b::sampleRandomBook(db, db.size());
        ASSERT_EQ(rt_all.size(), 6);

        auto it = std::find_if(rt_all.cbegin(), rt_all.cend(), [](const b::Book &book) {
            return book.title == std::string_view{"To Kill a Mockingbird"};
        });
        ASSERT_TRUE(it != rt_all.cend());
    }

    /// Get top N
    {
        /// Специально сделал рейтинги такие, чтобы можно было кастить к инту и все работало
        std::array<int, 6> ratings{8, 7, 4, 3, 2, 1};
        auto areEqual = [](int ths, const b::Book &other) { return ths == static_cast<int>(other.rating); };

        auto rt_3 = b::getTopN(db, 3);
        ASSERT_EQ(3, rt_3.size());

        auto st = std::equal(ratings.cbegin(), std::next(ratings.cbegin(), 3), rt_3.cbegin(), rt_3.cend(), areEqual);
        ASSERT_TRUE(st);
    }
    /// Apply statistics functions to an empty db
    {
        decltype(db) emptyDb;
        auto rt = b::sampleRandomBook(emptyDb, 1);
        ASSERT_EQ(rt.size(), 0);

        auto rating = b::calculateAverageRating(emptyDb);
        ASSERT_TRUE(rating == 0);

        auto genreRating = b::calculateGenreRating(emptyDb.begin(), emptyDb.end());
        ASSERT_TRUE(genreRating.empty());

        auto emptyHistogram = b::buildAuthorHistogramFlat(emptyDb);
        ASSERT_TRUE(emptyHistogram.empty());
    }
}

///
///
///

TEST_F(BookDBTestGroup, filters) {

    decltype(db) huge;
    for (int i = 0; i < 50'000; ++i) {
        huge.emplaceBack("title_" + std::to_string(i), "author", i + 1, b::Genre::Fiction, 4., 10);
    }

    for (int i = 0; i < 50'000; ++i) {
        huge.emplaceBack("title_" + std::to_string(i), "unknown", i + 50'000, b::Genre::SciFi, 6., 10);
    }

    /// YearBetween
    {
        auto filtered = b::filterBooks(huge.cbegin(), huge.cend(), b::all_of(b::YearBetween(100, 2100)));
        ASSERT_EQ(2000, filtered.size());
        auto st = std::all_of(filtered.cbegin(), filtered.cend(),
                              [](const b::Book &book) { return (book.year >= 100 && book.year < 2100); });
        ASSERT_TRUE(st);
    }
    /// YearBetween && RatingAbove
    {
        auto filtered =
            b::filterBooks(huge.cbegin(), huge.cend(), b::all_of(b::YearBetween(49'000, 51'000), b::RatingAbove(5.0)));
        ASSERT_EQ(1000, filtered.size());
        auto st = std::all_of(filtered.cbegin(), filtered.cend(),
                              [](const b::Book &book) { return book.year >= 50'000 && book.rating >= 5.0; });
        ASSERT_TRUE(st);
    }
    /// YearBetween || GenreIs
    {
        auto filtered = b::filterBooks(huge.cbegin(), huge.cend(),
                                       b::any_of(b::YearBetween(48'000, 49'000), b::GenreIs(b::Genre::SciFi)));
        ASSERT_EQ(51000, filtered.size());
        auto st = std::all_of(filtered.cbegin(), filtered.cend(), [](const b::Book &book) {
            return (book.year >= 48'000 && book.year < 49'000) || book.rating >= 5.0;
        });
        ASSERT_TRUE(st);
    }
}