#ifndef MUTABLE_ARRAY_SEQUENCE_H
#define MUTABLE_ARRAY_SEQUENCE_H

#include "array_sequence.h"

template <typename T>
class mutable_array_sequence : public array_sequence<T> {
public:
    mutable_array_sequence (const T* items, int count) : array_sequence<T>(items, count) {};
    mutable_array_sequence() : array_sequence<T>() {};
    mutable_array_sequence(const array_sequence<T> &array_seq) : array_sequence<T>(array_seq) {};

    virtual array_sequence<T>* create_empty() const override;
protected:
    virtual array_sequence<T>* instance() override;
    virtual array_sequence<T>* clone() const override;
};

#include "mutable_array_sequence.tpp"

#endif //MUTABLE_ARRAY_SEQUENCE_H