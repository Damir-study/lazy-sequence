#ifndef MUTABLE_LIST_SEQUENCE_H
#define MUTABLE_LIST_SEQUENCE_H

#include "list_sequence.h"

template <typename T>
class mutable_list_sequence : public list_sequence<T> {
public:
    mutable_list_sequence (T* items, int count) : list_sequence<T>(items, count) {};
    mutable_list_sequence() : list_sequence<T>() {};
    mutable_list_sequence(const list_sequence<T> &list_seq) : list_sequence<T>(list_seq) {};

    virtual list_sequence<T>* create_empty() const override;
protected:
    virtual list_sequence<T>* instance() override;
    virtual list_sequence<T>* clone() const override;
};

#include "mutable_list_sequence.tpp"

#endif //MUTABLE_ARRAY_SEQUENCE_H