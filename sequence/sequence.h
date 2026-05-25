#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "IEnumerator.h"

template <typename T>
class sequence : public IEnumerable<T> {
public:
    virtual ~sequence() {};

    virtual const T& get_first() const = 0;
    virtual const T& get_last() const = 0;

    virtual sequence<T>* get_subsequence(int start_index, int end_index) const = 0;
    virtual int get_length() const = 0;

    virtual sequence<T>* append(const T& item) = 0;
    virtual sequence<T>* prepend(const T& item) = 0;
    virtual sequence<T>* insert_at(const T& item, int index) = 0;
    virtual sequence<T>* concat(const sequence <T> *list) = 0;

    virtual IEnumerator<T>* get_enumerator() const override = 0;

    sequence<T>* slice(int index, int count, const sequence<T>* seq) const;
    sequence<T>* where(bool (*where_func)(const T&)) const;
    int find(bool (*find_func)(const T&)) const;

    virtual sequence<T>* create_empty() const = 0;
};

#include "sequence.tpp"

#endif //SEQUENCE_H