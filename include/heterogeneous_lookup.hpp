#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = void;

    bool operator()(const std::string_view &ths, const std::string_view &other) const { return ths < other; }

    bool operator()(const std::string &ths, const std::string &other) const { return ths < other; }

    bool operator()(const std::string &ths, const std::string_view &other) const { return ths < other; }

    bool operator()(const std::string_view &ths, const std::string &other) const { return ths < other; }
};

struct TransparentStringEqual {
    using is_transparent = void;

    bool operator()(const std::string_view &ths, const std::string_view &other) const { return ths == other; }

    bool operator()(const std::string &ths, const std::string &other) const { return ths == other; }

    bool operator()(const std::string &ths, const std::string_view &other) const { return ths == other; }

    bool operator()(const std::string_view &ths, const std::string &other) const { return ths == other; }
};

struct TransparentStringHash {
    using is_transparent = void;

    size_t operator()(const std::string_view &str) const { return std::hash<std::string_view>{}(str); }

    size_t operator()(const std::string &str) const { return std::hash<std::string>{}(str); }
};

}  // namespace bookdb
