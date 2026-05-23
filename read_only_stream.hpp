#ifndef READ_ONLY_STREAM_HPP
#define READ_ONLY_STREAM_HPP

#include <functional>
#include <string>

#include "lazy_sequence.hpp"
#include "sequence/sequence.h"

template <typename T>
using deserializer = std::function<T(const std::string&)>;

template <typename T>
class read_only_stream {
public:
    read_only_stream();
    read_only_stream(sequence<T>* source);
    read_only_stream(lazy_sequence<T>* source);
    read_only_stream(const std::string& data, deserializer<T> convert);
    read_only_stream(read_only_stream<T>* source);
    read_only_stream(const read_only_stream<T>& other);
    ~read_only_stream();

    bool is_end_of_stream() const;
    T read();
    int get_position() const;

    bool is_can_seek() const;
    int seek(int index);
    bool is_can_go_back() const;

    void open();
    void close();

private:
    enum source_kind {
        no_source_kind,
        sequence_source_kind,
        string_source_kind,
        stream_source_kind
    };

    source_kind kind;
    sequence<T>* source;
    IEnumerator<T>* enumerator;
    read_only_stream<T>* source_stream;
    std::string text;
    int text_index;
    deserializer<T> convert;
    int position;
    bool opened;
    bool end_reached;

    void reset_enumerator();
    bool read_string_token(std::string& token);
    bool skip_string_token();
    void require_open() const;
};

#include "read_only_stream.tpp"

#endif // READ_ONLY_STREAM_HPP
