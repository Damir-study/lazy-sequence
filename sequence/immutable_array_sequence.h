#ifndef IMMUTABLE_ARRAY_SEQUENCE_H
#define IMMUTABLE_ARRAY_SEQUENCE_H

#include "array_sequence.h"

template <typename T>
class immutable_array_sequence : public array_sequence<T> {
public:
    immutable_array_sequence (T* items, int count) : array_sequence<T>(items, count) {};
    immutable_array_sequence() : array_sequence<T>() {};
    immutable_array_sequence(const array_sequence<T> &array_seq) : array_sequence<T>(array_seq) {};

    virtual array_sequence<T>* create_empty() const override;
protected:
    virtual array_sequence<T>* instance() override;
    virtual array_sequence<T>* clone() const override;
};

#include "immutable_array_sequence.tpp"

#endif // IMMUTABLE_ARRAY_SEQUENCE_H