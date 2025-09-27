#pragma once

#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = BookContainer::value_type;
    using size_type = BookContainer::size_type;
    using iterator = BookContainer::iterator;
    using const_iterator = BookContainer::const_iterator;
    using reverse_iterator = BookContainer::reverse_iterator;
    using const_reverse_iterator = BookContainer::const_reverse_iterator;

    using AuthorContainer = std::unordered_set<std::string>;

    constexpr BookDatabase() = default;

    constexpr BookDatabase(std::initializer_list<Book> list) {
        for (auto &i : list) {
            pushBack(std::move(i));
        }
    }

    void clear() {
        books_.clear();
        authors_.clear();
    }

    template <typename... Argv>
        requires std::constructible_from<Book, Argv...>
    void emplaceBack(Argv &&...argv) {
        Book temp{std::forward<Argv>(argv)...};
        std::string author{temp.author};

        auto [it, success] = authors_.emplace(std::move(author));
        temp.author = *it;
        books_.push_back(std::move(temp));
    }

    void pushBack(value_type &&book) {
        std::string author{book.author};

        auto [it, success] = authors_.emplace(std::move(author));
        book.author = *it;
        books_.push_back(std::move(book));
    }

    void pushBack(const value_type &book) {
        std::string author{book.author};
        Book temp = book;

        auto [it, success] = authors_.emplace(std::move(author));
        temp.author = *it;
        books_.push_back(std::move(temp));
    }

    [[nodiscard]] size_type size() const noexcept { return books_.size(); }

    ///
    ///
    ///

    iterator begin() noexcept { return books_.begin(); }

    const_iterator begin() const noexcept { return books_.begin(); }

    const_iterator cbegin() const noexcept { return books_.begin(); }

    reverse_iterator rbegin() noexcept { return books_.rbegin(); }

    const_reverse_iterator rbegin() const noexcept { return books_.rbegin(); }

    const_reverse_iterator crbegin() const noexcept { return books_.crbegin(); }

    ///
    ///
    ///

    iterator end() noexcept { return books_.end(); }

    const_iterator end() const noexcept { return books_.end(); }

    const_iterator cend() const noexcept { return books_.cend(); }

    reverse_iterator rend() noexcept { return books_.rend(); }

    const_reverse_iterator rend() const noexcept { return books_.rend(); }

    const_reverse_iterator crend() const noexcept { return books_.crend(); }

    ///
    ///
    ///

    const BookContainer &getBooks() const noexcept { return books_; }

    const AuthorContainer &getAuthors() const noexcept { return authors_; }

protected:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.getBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.getAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
