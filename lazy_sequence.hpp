#ifndef LAZY_SEQUENCE_HPP
#define LAZY_SEQUENCE_HPP

#include <functional>

#include "ordinal.hpp"
#include "sequence/mutable_array_sequence.h"
#include "sequence/sequence.h"
#include "generator.hpp"

template <typename T>
class lazy_sequence : public sequence<T> {
public:
    lazy_sequence();
    lazy_sequence(const T* source_items, int count);
    lazy_sequence(const sequence<T>* seq);
    lazy_sequence(std::function<T(sequence<T>*)> rule, sequence<T>* start_items);
    lazy_sequence(std::function<T(sequence<T>*)> rule, sequence<T>* start_items, const ordinal& length);
    lazy_sequence(const lazy_sequence<T>& list);
    lazy_sequence<T>& operator=(const lazy_sequence<T>& list);
    ~lazy_sequence() override;

    const T& get_first() const override;
    const T& get_last() const override;
    const T& get(int index) const;
    const T& get(const ordinal& index) const;
    sequence<T>* get_subsequence(int start_index, int end_index) const override;
    int get_length() const override;
    ordinal get_ordinal_length() const;
    bool is_infinite() const;
    int get_materialized_count() const;
    sequence<T>* get_materialized_items() const;

    sequence<T>* append(const T& item) override;
    sequence<T>* prepend(const T& item) override;
    sequence<T>* insert_at(const T& item, int index) override;
    sequence<T>* insert_at(const sequence<T>* items, int index);
    sequence<T>* insert_at(const T& item, const ordinal& index);
    sequence<T>* insert_at(const sequence<T>* items, const ordinal& index);
    sequence<T>* concat(const sequence<T>* list) override;
    sequence<T>* remove_at(int index);
    sequence<T>* remove_at(const ordinal& index);
    sequence<T>* remove_range(int index, int count);
    sequence<T>* remove_range(const ordinal& index, int count);

    IEnumerator<T>* get_enumerator() const override;
    lazy_sequence<T>* create_empty() const override;

private:
    lazy_sequence(const sequence<T>* source, int start_index, int count);
    lazy_sequence(const sequence<T>* source, const ordinal& start_index, const ordinal& count);

    mutable mutable_array_sequence<T>* items;
    lazy_sequence<T>* source;
    lazy_sequence<T>* suffix;
    generator<T>* gen;
    ordinal size;
    ordinal slice_start;
    bool slice_enabled;

    void materialize_to(int index) const;
    void ensure_finite() const;
    ordinal get_tail_length_from(const ordinal& start) const;

    class lazy_enumerator;
};

#include "lazy_sequence.tpp"

#endif // LAZY_SEQUENCE_HPP
