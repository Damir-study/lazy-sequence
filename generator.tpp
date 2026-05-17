#include "generator.hpp"
#include "sequence/mutable_array_sequence.h"
#include <stdexcept>

template <typename T>
generator<T>::generator()
    : owner(nullptr),
      source(nullptr),
      source_iterator(nullptr),
      rule(nullptr),
      operation(no_operation_kind),
      operation_index(ordinal::finite(0)),
      operation_items(nullptr),
      operation_iterator(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_count(0),
      remove_count(0) {}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, std::function<T(sequence<T>*)> rule)
    : owner(owner),
      source(nullptr),
      source_iterator(nullptr),
      rule(rule),
      operation(no_operation_kind),
      operation_index(ordinal::finite(0)),
      operation_items(nullptr),
      operation_iterator(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_count(0),
      remove_count(0) {}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, sequence<T>* source, insert_item_operation operation_data)
    : owner(owner),
      source(source),
      source_iterator(nullptr),
      rule(nullptr),
      operation(insert_operation_kind),
      operation_index(operation_data.index),
      operation_items(nullptr),
      operation_iterator(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_count(0),
      remove_count(0) {
    if (source == nullptr) {
        throw std::invalid_argument("Generator source is null");
    }

    T buffer[1] = {operation_data.item};
    operation_items = new mutable_array_sequence<T>(buffer, 1);

    source_iterator = source->get_enumerator();
    operation_iterator = operation_items->get_enumerator();
}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, sequence<T>* source, insert_sequence_operation operation_data)
    : owner(owner),
      source(source),
      source_iterator(nullptr),
      rule(nullptr),
      operation(insert_operation_kind),
      operation_index(operation_data.index),
      operation_items(nullptr),
      operation_iterator(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_count(0),
      remove_count(0) {
    if (source == nullptr) {
        throw std::invalid_argument("Generator source is null");
    }

    copy_operation_items(operation_data.items);

    source_iterator = source->get_enumerator();
    operation_iterator = operation_items == nullptr ? nullptr : operation_items->get_enumerator();
}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, sequence<T>* source, remove_operation operation_data)
    : owner(owner),
      source(source),
      source_iterator(nullptr),
      rule(nullptr),
      operation(remove_operation_kind),
      operation_index(operation_data.index),
      operation_items(nullptr),
      operation_iterator(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_count(0),
      remove_count(operation_data.count) {
    if (source == nullptr) {
        throw std::invalid_argument("Generator source is null");
    }

    source_iterator = source->get_enumerator();
}

template <typename T>
generator<T>::generator(const generator<T>& other)
    : owner(other.owner),
      source(other.source),
      source_iterator(nullptr),
      rule(other.rule),
      operation(other.operation),
      operation_index(other.operation_index),
      operation_items(nullptr),
      operation_iterator(nullptr),
      current_index(other.current_index),
      source_index(other.source_index),
      inserted_count(other.inserted_count),
      remove_count(other.remove_count) {
    copy_operation_items(other.operation_items);

    source_iterator = source == nullptr ? nullptr : source->get_enumerator();
    operation_iterator = operation_items == nullptr ? nullptr : operation_items->get_enumerator();

    for (std::size_t i = 0; i < source_index; ++i) {
        if (source_iterator == nullptr || !source_iterator->move_next()) {
            throw std::out_of_range("IndexOutOfRange");
        }
    }

    for (std::size_t i = 0; i < inserted_count; ++i) {
        if (operation_iterator == nullptr || !operation_iterator->move_next()) {
            throw std::out_of_range("IndexOutOfRange");
        }
    }
}

template <typename T>
generator<T>::~generator() {
    delete source_iterator;
    delete operation_iterator;
    delete operation_items;
}

template <typename T>
T generator<T>::get_next() {
    if (operation == no_operation_kind) {
        if (!owner || !rule) {
            throw std::invalid_argument("nullptr owner or rule");
        }

        T item = rule(owner);
        current_index += 1;
        return item;
    }

    if (!has_next()) {
        throw std::out_of_range("IndexOutOfRange");
    }

    bool should_insert = operation == insert_operation_kind &&
                         current_index >= operation_index &&
                         operation_items != nullptr &&
                         inserted_count < static_cast<std::size_t>(operation_items->get_length());

    if (should_insert) {
        operation_iterator->move_next();
        ++inserted_count;
        current_index += 1;
        return operation_iterator->get_current();
    }

    bool should_remove = operation == remove_operation_kind &&
                         current_index == operation_index &&
                         remove_count > 0;

    if (should_remove) {
        for (std::size_t i = 0; i < remove_count; ++i) {
            source_iterator->move_next();
            ++source_index;
        }
    }

    source_iterator->move_next();

    ++source_index;
    current_index += 1;
    return source_iterator->get_current();
}

template <typename T>
bool generator<T>::has_next() const {
    if (operation == no_operation_kind) {
        return owner && rule;
    }

    if (source == nullptr) {
        return false;
    }

    ordinal length = get_base_ordinal_length();
    if (operation == insert_operation_kind && operation_items != nullptr) {
        ordinal inserted_length = ordinal::finite(static_cast<std::size_t>(operation_items->get_length()));
        if (length.is_finite() || operation_index >= length ||
            operation_index.get_omega_coefficient() == length.get_omega_coefficient()) {
            length += inserted_length;
        }
    }
    if (operation == remove_operation_kind && operation_index < length) {
        if (length.is_finite() && operation_index.is_finite()) {
            std::size_t available = length.get_count() - operation_index.get_count();
            std::size_t removed = remove_count > available ? available : remove_count;
            length -= ordinal::finite(removed);
        } else if (length.is_infinite() &&
                   operation_index.is_infinite() &&
                   operation_index.get_omega_coefficient() == length.get_omega_coefficient()) {
            std::size_t available = length.get_finite_part() - operation_index.get_finite_part();
            std::size_t removed = remove_count > available ? available : remove_count;
            length -= removed;
        }
    }

    return current_index < length;
}

template <typename T>
generator<T>* generator<T>::append(const T& item) const {
    return insert(item, get_base_ordinal_length());
}

template <typename T>
generator<T>* generator<T>::append(const sequence<T>* items) const {
    return insert(items, get_base_ordinal_length());
}

template <typename T>
generator<T>* generator<T>::prepend(const T& item) const {
    return insert(item, ordinal::finite(0));
}

template <typename T>
generator<T>* generator<T>::prepend(const sequence<T>* items) const {
    return insert(items, ordinal::finite(0));
}

template <typename T>
generator<T>* generator<T>::insert(const T& item, std::size_t index) const {
    return insert(item, ordinal::finite(index));
}

template <typename T>
generator<T>* generator<T>::insert(const T& item, const ordinal& index) const {
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::insert_item_operation operation_data{index, item};
    return new generator<T>(owner, base, operation_data);
}

template <typename T>
generator<T>* generator<T>::insert(const sequence<T>* items, std::size_t index) const {
    return insert(items, ordinal::finite(index));
}

template <typename T>
generator<T>* generator<T>::insert(const sequence<T>* items, const ordinal& index) const {
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::insert_sequence_operation operation_data{index, items};
    return new generator<T>(owner, base, operation_data);
}

template <typename T>
generator<T>* generator<T>::remove(std::size_t index) const {
    return remove(ordinal::finite(index), 1);
}

template <typename T>
generator<T>* generator<T>::remove(const ordinal& index) const {
    return remove(index, 1);
}

template <typename T>
generator<T>* generator<T>::remove(std::size_t index, std::size_t count) const {
    return remove(ordinal::finite(index), count);
}

template <typename T>
generator<T>* generator<T>::remove(const ordinal& index, std::size_t count) const {
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::remove_operation operation_data{index, count};
    return new generator<T>(owner, base, operation_data);
}

template <typename T>
void generator<T>::set_owner(lazy_sequence<T>* owner) {
    this->owner = owner;
}

template <typename T>
void generator<T>::set_source(sequence<T>* source) {
    this->source = source;

    delete source_iterator;
    source_iterator = nullptr;

    if (this->source != nullptr && operation != no_operation_kind) {
        source_iterator = this->source->get_enumerator();
        for (std::size_t i = 0; i < source_index; ++i) {
            if (!source_iterator->move_next()) {
                throw std::out_of_range("IndexOutOfRange");
            }
        }
    }
}

template <typename T>
void generator<T>::copy_operation_items(const sequence<T>* items) {
    delete operation_items;
    operation_items = nullptr;

    if (items == nullptr) {
        return;
    }

    operation_items = new mutable_array_sequence<T>();
    IEnumerator<T>* it = items->get_enumerator();

    while (it->move_next()) {
        sequence<T>* next = operation_items->append(it->get_current());
        if (next != operation_items) {
            delete operation_items;
            operation_items = next;
        }
    }

    delete it;
}

template <typename T>
sequence<T>* generator<T>::get_base_source() const {
    if (source != nullptr) {
        return source;
    }
    if (owner != nullptr) {
        return owner;
    }
    return nullptr;
}

template <typename T>
ordinal generator<T>::get_base_ordinal_length() const {
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }

    lazy_sequence<T>* lazy = dynamic_cast<lazy_sequence<T>*>(base);
    if (lazy != nullptr) {
        return lazy->get_ordinal_length();
    }

    return ordinal::finite(static_cast<std::size_t>(base->get_length()));
}
