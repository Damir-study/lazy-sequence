#ifndef WRITE_ONLY_STREAM_HPP
#define WRITE_ONLY_STREAM_HPP

#include "sequence/sequence.h"

template <typename T>
class write_only_stream {
public:
    write_only_stream();
    write_only_stream(sequence<T>* target);
    write_only_stream(write_only_stream<T>* target);
    write_only_stream(const write_only_stream<T>& other);
    ~write_only_stream();

    int get_position() const;
    sequence<T>* get_sequence() const;

    int write(const T& item);

    void open();
    void close();

private:
    enum target_kind {
        no_target_kind,
        sequence_target_kind,
        stream_target_kind
    };

    target_kind kind;
    sequence<T>* target;
    write_only_stream<T>* target_stream;
    int position;
    bool opened;

    void require_open() const;
};

#include "write_only_stream.tpp"

#endif // WRITE_ONLY_STREAM_HPP
