#ifndef LIST_SEQUENCE_H
#define LIST_SEQUENCE_H

#include "linked_list.h"
#include "sequence.h"

template <typename T>
class list_sequence : public sequence<T> {
public:
    list_sequence (const T* items, int count);
    list_sequence();
    list_sequence(const list_sequence<T> &list_seq);
    ~list_sequence();

    virtual const T& get_first() const override;
    virtual const T& get_last() const override;
    virtual int get_length() const override;
    virtual sequence<T>* get_subsequence(int start_index, int end_index) const override;
    
    virtual sequence<T>* append(const T& item) override;
    virtual sequence<T>* prepend(const T& item) override;
    virtual sequence<T>* insert_at(const T& item, int index) override;

    virtual sequence <T>* concat(const sequence <T> *list) override;

    IEnumerator<T>* get_enumerator() const;

    virtual list_sequence<T>* create_empty() const override = 0;
protected:
    linked_list<T>* items;

    virtual list_sequence<T>* instance() = 0;
    virtual list_sequence<T>* clone() const = 0;
};

#include "list_sequence.tpp"

#endif //LIST_SEQUENCE_H