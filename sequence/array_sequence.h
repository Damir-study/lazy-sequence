#ifndef ARRAY_SEQUENCE_H
#define ARRAY_SEQUENCE_H

#include "dynamic_array.h"
#include "sequence.h"

template <typename T>
class array_sequence : public sequence<T> {
public:
    array_sequence (const T* items, int count);
    array_sequence();
    array_sequence(int count);
    array_sequence(const array_sequence<T> &array_seq);
    ~array_sequence();

    virtual const T& get_first() const override;
    virtual const T& get_last() const override;
    virtual const T& operator[](int index) const;
    virtual int get_length() const override;
    virtual sequence<T>* get_subsequence(int start_index, int end_index) const override;
    
    virtual sequence<T>* append(const T& item) override;
    virtual sequence<T>* prepend(const T& item) override;
    virtual sequence<T>* insert_at(const T& item, int index) override;
    virtual sequence <T>* concat(const sequence <T> *arr) override;

    IEnumerator<T>*get_enumerator() const;

    virtual array_sequence<T>* create_empty() const override = 0;
protected:
    dynamic_array<T>* items;

    virtual array_sequence<T>* instance() = 0;
    virtual array_sequence<T>* clone() const = 0;
};

#include "array_sequence.tpp"

#endif //ARRAY_SEQUENCE_H