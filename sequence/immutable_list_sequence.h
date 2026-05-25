#ifndef IMMUTABLE_LIST_SEQUENCE_H
#define IMMUTABLE_LIST_SEQUENCE_H

#include "list_sequence.h"

template <typename T>
class immutable_list_sequence : public list_sequence<T> {
public:
    immutable_list_sequence (T* items, int count) : list_sequence<T>(items, count) {};
    immutable_list_sequence() : list_sequence<T>() {};
    immutable_list_sequence(const list_sequence<T> &list_seq) : list_sequence<T>(list_seq) {};

    virtual list_sequence<T>* create_empty() const override;
protected:
    virtual list_sequence<T>* instance() override;
    virtual list_sequence<T>* clone() const override;
};

#include "immutable_list_sequence.tpp"

#endif // IMMUTABLE_list_SEQUENCE_H