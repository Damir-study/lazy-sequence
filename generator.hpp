#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <functional>

#include "ordinal.hpp"
#include "sequence/sequence.h"

template <typename T>
class lazy_sequence;

template <typename T>
class generator {
public:
    struct insert_item_operation {
        ordinal index;
        T item;
    };

    struct insert_sequence_operation {
        ordinal index;
        const sequence<T>* items;
    };

    struct remove_operation {
        ordinal index;
        int count;
    };

    generator();
    generator(lazy_sequence<T>* owner, std::function<T(sequence<T>*)> rule);
    generator(lazy_sequence<T>* owner, sequence<T>* source, insert_item_operation operation);
    generator(lazy_sequence<T>* owner, sequence<T>* source, insert_sequence_operation operation);
    generator(lazy_sequence<T>* owner, sequence<T>* source, remove_operation operation);
    generator(const generator<T>& other);

    ~generator();

    T get_next();
    bool has_next() const;

    generator<T>* append(const T& item) const;
    generator<T>* append(const sequence<T>* items) const;
    generator<T>* prepend(const T& item) const;
    generator<T>* prepend(const sequence<T>* items) const;
    generator<T>* insert(const T& item, int index) const;
    generator<T>* insert(const T& item, const ordinal& index) const;
    generator<T>* insert(const sequence<T>* items, int index) const;
    generator<T>* insert(const sequence<T>* items, const ordinal& index) const;
    generator<T>* remove(int index) const;
    generator<T>* remove(const ordinal& index) const;
    generator<T>* remove(int index, int count) const;
    generator<T>* remove(const ordinal& index, int count) const;
    void set_owner(lazy_sequence<T>* owner);
    void set_source(sequence<T>* source);
    
private:
    enum operation_kind {
        no_operation_kind,
        insert_operation_kind,
        remove_operation_kind
    };

    lazy_sequence<T>* owner;
    sequence<T>* source;
    IEnumerator<T>* source_iterator;

    std::function<T(sequence<T>*)> rule;

    operation_kind operation;
    ordinal operation_index;
    sequence<T>* operation_items;
    IEnumerator<T>* operation_iterator;

    ordinal current_index;
    int source_index;
    int inserted_count;
    int remove_count;

    void copy_operation_items(const sequence<T>* items);
    sequence<T>* get_base_source() const;
    ordinal get_base_ordinal_length() const;
};

#include "generator.tpp"

#endif // GENERATOR_HPP
