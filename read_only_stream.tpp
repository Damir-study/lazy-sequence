#include "read_only_stream.hpp"

#include <cctype>
#include <stdexcept>

template <typename T>
read_only_stream<T>::read_only_stream()
    : kind(no_source_kind),
      source(nullptr),
      enumerator(nullptr),
      source_stream(nullptr),
      text_index(0),
      position(0),
      opened(false),
      end_reached(true) {}

template <typename T>
read_only_stream<T>::read_only_stream(sequence<T>* source)
    : kind(sequence_source_kind),
      source(source),
      enumerator(nullptr),
      source_stream(nullptr),
      text_index(0),
      position(0),
      opened(false),
      end_reached(source == nullptr) {}

template <typename T>
read_only_stream<T>::read_only_stream(lazy_sequence<T>* source)
    : read_only_stream(static_cast<sequence<T>*>(source)) {}

template <typename T>
read_only_stream<T>::read_only_stream(const std::string& data, deserializer<T> convert)
    : kind(string_source_kind),
      source(nullptr),
      enumerator(nullptr),
      source_stream(nullptr),
      text(data),
      text_index(0),
      convert(convert),
      position(0),
      opened(false),
      end_reached(data.empty()) {
    if (!convert) {
        throw std::invalid_argument("Deserializer is empty");
    }
}

template <typename T>
read_only_stream<T>::read_only_stream(read_only_stream<T>* source)
    : kind(stream_source_kind),
      source(nullptr),
      enumerator(nullptr),
      source_stream(source),
      text_index(0),
      position(0),
      opened(false),
      end_reached(source == nullptr) {}

template <typename T>
read_only_stream<T>::read_only_stream(const read_only_stream<T>& other)
    : kind(other.kind),
      source(other.source),
      enumerator(nullptr),
      source_stream(other.source_stream),
      text(other.text),
      text_index(0),
      convert(other.convert),
      position(0),
      opened(false),
      end_reached(other.end_reached) {
    if (other.opened && is_can_seek()) {
        open();
        seek(other.position);
    }
}

template <typename T>
read_only_stream<T>::~read_only_stream() {
    close();
}

template <typename T>
bool read_only_stream<T>::is_end_of_stream() const {
    if (kind == no_source_kind) {
        return true;
    }

    if (kind == sequence_source_kind) {
        if (source == nullptr) {
            return true;
        }
        try {
            return position >= source->get_length();
        } catch (...) {
            return end_reached;
        }
    }

    if (kind == string_source_kind) {
        int index = text_index;
        while (index < static_cast<int>(text.length()) &&
               std::isspace(static_cast<unsigned char>(text[index]))) {
            ++index;
        }
        return index >= static_cast<int>(text.length());
    }

    if (kind == stream_source_kind) {
        return source_stream == nullptr || source_stream->is_end_of_stream();
    }

    return end_reached;
}

template <typename T>
T read_only_stream<T>::read() {
    require_open();

    if (kind == sequence_source_kind) {
        if (!enumerator->move_next()) {
            end_reached = true;
            throw std::out_of_range("EndOfStream");
        }

        ++position;
        return enumerator->get_current();
    }

    if (kind == string_source_kind) {
        std::string token;
        if (!read_string_token(token)) {
            end_reached = true;
            throw std::out_of_range("EndOfStream");
        }

        ++position;
        return convert(token);
    }

    if (kind == stream_source_kind) {
        if (source_stream == nullptr || source_stream->is_end_of_stream()) {
            end_reached = true;
            throw std::out_of_range("EndOfStream");
        }

        ++position;
        return source_stream->read();
    }

    throw std::out_of_range("EndOfStream");
}

template <typename T>
int read_only_stream<T>::get_position() const {
    return position;
}

template <typename T>
bool read_only_stream<T>::is_can_seek() const {
    return kind == sequence_source_kind || kind == string_source_kind;
}

template <typename T>
int read_only_stream<T>::seek(int index) {
    require_open();
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    if (!is_can_seek()) {
        throw std::logic_error("Cannot seek");
    }

    if (kind == sequence_source_kind) {
        reset_enumerator();
        for (int i = 0; i < index; ++i) {
            if (!enumerator->move_next()) {
                end_reached = true;
                throw std::out_of_range("EndOfStream");
            }
        }

        position = index;
        end_reached = false;
        return position;
    }

    text_index = 0;
    position = 0;
    end_reached = false;
    for (int i = 0; i < index; ++i) {
        if (!skip_string_token()) {
            end_reached = true;
            throw std::out_of_range("EndOfStream");
        }
    }

    position = index;
    return position;
}

template <typename T>
bool read_only_stream<T>::is_can_go_back() const {
    return is_can_seek();
}

template <typename T>
void read_only_stream<T>::open() {
    close();

    if (kind == sequence_source_kind) {
        if (source == nullptr) {
            throw std::invalid_argument("Stream source is null");
        }
        reset_enumerator();
    } else if (kind == string_source_kind) {
        text_index = 0;
    } else if (kind == stream_source_kind) {
        if (source_stream == nullptr) {
            throw std::invalid_argument("Stream source is null");
        }
    } else {
        throw std::invalid_argument("Stream source is null");
    }

    opened = true;
    position = 0;
    end_reached = false;
}

template <typename T>
void read_only_stream<T>::close() {
    delete enumerator;
    enumerator = nullptr;
    opened = false;
}

template <typename T>
void read_only_stream<T>::reset_enumerator() {
    delete enumerator;
    enumerator = nullptr;

    if (source != nullptr) {
        enumerator = source->get_enumerator();
    }
}

template <typename T>
bool read_only_stream<T>::read_string_token(std::string& token) {
    while (text_index < static_cast<int>(text.length()) && std::isspace(static_cast<unsigned char>(text[text_index]))) {
        ++text_index;
    }

    if (text_index >= static_cast<int>(text.length())) {
        return false;
    }

    int start = text_index;
    while (text_index < static_cast<int>(text.length()) && !std::isspace(static_cast<unsigned char>(text[text_index]))) {
        ++text_index;
    }

    token = text.substr(start, text_index - start);
    return true;
}

template <typename T>
bool read_only_stream<T>::skip_string_token() {
    std::string token;
    return read_string_token(token);
}

template <typename T>
void read_only_stream<T>::require_open() const {
    if (!opened) {
        throw std::logic_error("Stream is not open");
    }
    if (kind == sequence_source_kind && enumerator == nullptr) {
        throw std::logic_error("Stream is not open");
    }
}
