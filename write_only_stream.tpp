#include "write_only_stream.hpp"

#include <stdexcept>

template <typename T>
write_only_stream<T>::write_only_stream()
    : kind(no_target_kind),
      target(nullptr),
      target_stream(nullptr),
      position(0),
      opened(false) {}

template <typename T>
write_only_stream<T>::write_only_stream(sequence<T>* target)
    : kind(sequence_target_kind),
      target(target),
      target_stream(nullptr),
      position(0),
      opened(false) {}

template <typename T>
write_only_stream<T>::write_only_stream(write_only_stream<T>* target)
    : kind(stream_target_kind),
      target(nullptr),
      target_stream(target),
      position(0),
      opened(false) {}

template <typename T>
write_only_stream<T>::write_only_stream(const write_only_stream<T>& other)
    : kind(other.kind),
      target(other.target),
      target_stream(other.target_stream),
      position(other.position),
      opened(false) {}

template <typename T>
write_only_stream<T>::~write_only_stream() {
    close();
}

template <typename T>
int write_only_stream<T>::get_position() const {
    return position;
}

template <typename T>
sequence<T>* write_only_stream<T>::get_sequence() const {
    return target;
}

template <typename T>
int write_only_stream<T>::write(const T& item) {
    require_open();

    if (kind == sequence_target_kind) {
        target = target->append(item);
    } else if (kind == stream_target_kind) {
        target_stream->write(item);
    }

    ++position;
    return position;
}

template <typename T>
void write_only_stream<T>::open() {
    close();

    if (kind == sequence_target_kind) {
        if (target == nullptr) {
            throw std::invalid_argument("Stream target is null");
        }
        position = 0;
    } else if (kind == stream_target_kind) {
        if (target_stream == nullptr) {
            throw std::invalid_argument("Stream target is null");
        }
        position = 0;
    } else {
        throw std::invalid_argument("Stream target is null");
    }

    opened = true;
}

template <typename T>
void write_only_stream<T>::close() {
    opened = false;
}

template <typename T>
void write_only_stream<T>::require_open() const {
    if (!opened) {
        throw std::logic_error("Stream is not open");
    }
    if (kind == sequence_target_kind && target == nullptr) {
        throw std::logic_error("Stream is not open");
    }
    if (kind == stream_target_kind && target_stream == nullptr) {
        throw std::logic_error("Stream is not open");
    }
}
