#pragma once

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

///
///
///

constexpr Genre GenreFromString(std::string_view s) {
    Genre genre = Genre::Unknown;
    using enum Genre;
    if (s == "Fiction") {
        genre = Fiction;
    } else if (s == "NonFiction") {
        genre = NonFiction;
    } else if (s == "SciFi") {
        genre = SciFi;
    } else if (s == "Biography") {
        genre = Biography;
    } else if (s == "Mystery") {
        genre = Mystery;
    }
    return genre;
}

constexpr std::string_view StringFromGenre(Genre genre) {
    // clang-format off
    switch (genre)
    {
    using enum Genre;
    case Fiction: return "Fiction";
    case NonFiction: return "NonFiction";
    case SciFi: return "SciFi";
    case Biography: return "Biography";
    case Mystery: return "Mystery";
    default:
        break;
    }
    // clang-format on
    return "Unknown";
}

///
///
///

struct Book {
    /// Каким образом по условию задания я должен сделать constexpr c-tors когда я использую поле std::string title...
    Book(std::string title, std::string_view author, int year, std::string_view genre, double rating, int readCount)
        : author{author}, title{title}, year{year}, genre{GenreFromString(genre)}, rating{rating},
          read_count{readCount} {}

    Book(std::string title, std::string_view author, int year, Genre genre, double rating, int readCount)
        : author{author}, title{title}, year{year}, genre{genre}, rating{rating}, read_count{readCount} {}

    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;
};

}  // namespace bookdb

///
///
///

namespace std {

template <>
struct formatter<bookdb::Genre, char> {

    template <typename FormatContext>
    auto format(bookdb::Genre g, FormatContext &fc) const {
        std::string genre_str;
        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

///
///
///

template <>
class formatter<bookdb::Book, char> {
public:
    template <typename FormatContext>
    auto format(bookdb::Book b, FormatContext &fc) const {
        std::string formatedText;
        formatedText = std::format("{} by {}", b.title, b.author);
        if (showDetails) {
            formatedText += std::format("\n\tGenre: {}, Year: {}, Rating: {}, Read count: {}", StringFromGenre(b.genre),
                                        b.year, b.rating, b.read_count);
        }
        return format_to(fc.out(), "{}", formatedText);
    }

    constexpr auto parse(format_parse_context &ctx) {
        auto it = ctx.begin();
        if ((it != ctx.end()) && (*it == 'd')) {
            showDetails = true;
            it++;
        }
        return it;
    }

private:
    bool showDetails = false;
};

}  // namespace std
