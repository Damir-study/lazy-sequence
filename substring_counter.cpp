#include "substring_counter.hpp"

#include <stdexcept>

#include "read_only_stream.hpp"

substring_counter::substring_counter(const char* pattern)
    : pattern(nullptr),
      prefix(nullptr),
      pattern_length(0) {
    if (pattern == nullptr) {
        throw std::invalid_argument("Pattern is null");
    }

    int length = 0;
    while (pattern[length] != '\0') {
        ++length;
    }

    if (length == 0) {
        throw std::invalid_argument("Pattern is empty");
    }

    set_pattern(pattern, length);
}

substring_counter::substring_counter(const substring_counter& other)
    : pattern(nullptr),
      prefix(nullptr),
      pattern_length(0) {
    set_pattern(other.pattern, other.pattern_length);
}

substring_counter& substring_counter::operator=(const substring_counter& other) {
    if (this == &other) {
        return *this;
    }

    set_pattern(other.pattern, other.pattern_length);
    return *this;
}

substring_counter::~substring_counter() {
    delete[] pattern;
    delete[] prefix;
}

int substring_counter::count(read_only_stream<char>* stream) const {
    return count(stream, -1);
}

int substring_counter::count(read_only_stream<char>* stream, int max_read_count) const {
    if (stream == nullptr) {
        throw std::invalid_argument("Stream is null");
    }
    if (max_read_count < -1) {
        throw std::out_of_range("IndexOutOfRange");
    }

    int result = 0;
    int matched = 0;
    int read_count = 0;

    while ((max_read_count == -1 || read_count < max_read_count) && !stream->is_end_of_stream()) {
        char current = stream->read();
        ++read_count;

        while (matched > 0 && current != pattern[matched]) {
            matched = prefix[matched - 1];
        }

        if (current == pattern[matched]) {
            ++matched;
        }

        if (matched == pattern_length) {
            ++result;
            matched = prefix[matched - 1];
        }
    }

    return result;
}

void substring_counter::set_pattern(const char* source, int length) {
    if (source == nullptr || length <= 0) {
        throw std::invalid_argument("Pattern is empty");
    }

    char* new_pattern = nullptr;
    int* new_prefix = nullptr;

    try {
        new_pattern = new char[length + 1];
        new_prefix = new int[length];
    } catch (...) {
        delete[] new_pattern;
        delete[] new_prefix;
        throw;
    }

    for (int i = 0; i < length; ++i) {
        new_pattern[i] = source[i];
        new_prefix[i] = 0;
    }
    new_pattern[length] = '\0';

    delete[] pattern;
    delete[] prefix;

    pattern = new_pattern;
    prefix = new_prefix;
    pattern_length = length;
    build_prefix();
}

void substring_counter::build_prefix() {
    int matched = 0;
    prefix[0] = 0;

    for (int i = 1; i < pattern_length; ++i) {
        while (matched > 0 && pattern[i] != pattern[matched]) {
            matched = prefix[matched - 1];
        }

        if (pattern[i] == pattern[matched]) {
            ++matched;
        }

        prefix[i] = matched;
    }
}
