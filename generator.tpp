#include "generator.hpp"
#include <stdexcept>

template <typename T>
generator<T>::generator()
    : owner(nullptr),
      source(nullptr),
      source_iterator(nullptr),
      rule(nullptr),
      operation(no_operation_kind),
      operation_index(ordinal::finite(0)),
      inserted_item(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_item_used(false),
      remove_count(0) {}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, std::function<T(sequence<T>*)> rule)
    : owner(owner),
      source(nullptr),
      source_iterator(nullptr),
      rule(rule),
      operation(no_operation_kind),
      operation_index(ordinal::finite(0)),
      inserted_item(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_item_used(false),
      remove_count(0) {}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, sequence<T>* source, insert_item_operation operation_data)
    : owner(owner),
      source(source),
      source_iterator(nullptr),
      rule(nullptr),
      operation(insert_operation_kind),
      operation_index(operation_data.index),
      inserted_item(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_item_used(false),
      remove_count(0) {
    if (source == nullptr) {
        throw std::invalid_argument("Generator source is null");
    }

    inserted_item = new T(operation_data.item);
    source_iterator = source->get_enumerator();
}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, sequence<T>* source, remove_operation operation_data)
    : owner(owner),
      source(source),
      source_iterator(nullptr),
      rule(nullptr),
      operation(remove_operation_kind),
      operation_index(operation_data.index),
      inserted_item(nullptr),
      current_index(ordinal::finite(0)),
      source_index(0),
      inserted_item_used(false),
      remove_count(operation_data.count) {
    if (source == nullptr) {
        throw std::invalid_argument("Generator source is null");
    }
    if (operation_data.count < 0) {
        throw std::invalid_argument("Remove count cannot be negative");
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
      inserted_item(other.inserted_item == nullptr ? nullptr : new T(*other.inserted_item)),
      current_index(other.current_index),
      source_index(other.source_index),
      inserted_item_used(other.inserted_item_used),
      remove_count(other.remove_count) {
    source_iterator = source == nullptr ? nullptr : source->get_enumerator();

    for (int i = 0; i < source_index; ++i) {
        if (source_iterator == nullptr || !source_iterator->move_next()) {
            throw std::out_of_range("IndexOutOfRange");
        }
    }
}

template <typename T>
generator<T>::~generator() {
    delete source_iterator;
    delete inserted_item;
}

template <typename T>
T generator<T>::get_next() {
    if (operation == no_operation_kind) {
        if (!owner || !rule) {
            throw std::invalid_argument("nullptr owner or rule");
        }

        T item = rule(owner->get_materialized_items());
        current_index += 1;
        return item;
    }

    if (!has_next()) {
        throw std::out_of_range("IndexOutOfRange");
    }

    bool should_insert = operation == insert_operation_kind &&
                         current_index >= operation_index &&
                         inserted_item != nullptr &&
                         !inserted_item_used;

    if (should_insert) {
        inserted_item_used = true;
        current_index += 1;
        return *inserted_item;
    }

    bool should_remove = operation == remove_operation_kind &&
                         current_index == operation_index &&
                         remove_count > 0;

    if (should_remove) {
        for (int i = 0; i < remove_count; ++i) {
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
    if (operation == insert_operation_kind && inserted_item != nullptr) {
        ordinal inserted_length = ordinal::finite(1);
        if (length.is_finite() || operation_index >= length ||
            operation_index.get_omega_coefficient() == length.get_omega_coefficient()) {
            length += inserted_length;
        }
    }
    if (operation == remove_operation_kind && operation_index < length) {
        if (length.is_finite() && operation_index.is_finite()) {
            int available = length.get_count() - operation_index.get_count();
            int removed = remove_count > available ? available : remove_count;
            length -= ordinal::finite(removed);
        } else if (length.is_infinite() &&
                   operation_index.is_infinite() &&
                   operation_index.get_omega_coefficient() == length.get_omega_coefficient()) {
            int available = length.get_finite_part() - operation_index.get_finite_part();
            int removed = remove_count > available ? available : remove_count;
            length -= removed;
        }
    }

    return current_index < length;
}

template <typename T>
generator<T>* generator<T>::append(const T& item) const {
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::insert_item_operation operation_data{get_base_ordinal_length(), item};
    return new generator<T>(owner, base, operation_data);
}

template <typename T>
generator<T>* generator<T>::prepend(const T& item) const {
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::insert_item_operation operation_data{ordinal::finite(0), item};
    return new generator<T>(owner, base, operation_data);
}

template <typename T>
generator<T>* generator<T>::insert(const T& item, int index) const {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::insert_item_operation operation_data{ordinal::finite(index), item};
    return new generator<T>(owner, base, operation_data);
}

template <typename T>
generator<T>* generator<T>::remove(int index) const {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    return remove(index, 1);
}

template <typename T>
generator<T>* generator<T>::remove(int index, int count) const {
    if (index < 0 || count < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    sequence<T>* base = get_base_source();
    if (base == nullptr) {
        throw std::logic_error("Generator has no source");
    }
    generator<T>::remove_operation operation_data{ordinal::finite(index), count};
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
        for (int i = 0; i < source_index; ++i) {
            if (!source_iterator->move_next()) {
                throw std::out_of_range("IndexOutOfRange");
            }
        }
    }
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

    return ordinal::finite(base->get_length());
}
