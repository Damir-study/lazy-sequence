#include "lazy_sequence.hpp"

#include <stdexcept>

template <typename T>
lazy_sequence<T>::lazy_sequence()
    : items(new mutable_array_sequence<T>()),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(ordinal::finite(0)),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {}

template <typename T>
lazy_sequence<T>::lazy_sequence(const T* source_items, int count)
    : items(nullptr),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(ordinal::finite(0)),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }
    if (source_items == nullptr && count > 0) {
        throw std::invalid_argument("Items pointer cannot be null");
    }
    size = ordinal::finite(count);
    items = new mutable_array_sequence<T>(source_items, count);
}

template <typename T>
lazy_sequence<T>::lazy_sequence(const sequence<T>* seq)
    : items(new mutable_array_sequence<T>()),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(ordinal::finite(0)),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {
    if (seq == nullptr) {
        return;
    }

    const lazy_sequence<T>* lazy = dynamic_cast<const lazy_sequence<T>*>(seq);
    if (lazy != nullptr) {
        *this = *lazy;
        return;
    }

    size = ordinal::finite(seq->get_length());
    IEnumerator<T>* it = seq->get_enumerator();
    while (it->move_next()) {
        items->append(it->get_current());
    }
    delete it;
}

template <typename T>
lazy_sequence<T>::lazy_sequence(std::function<T(sequence<T>*)> rule, sequence<T>* start_items)
    : lazy_sequence(rule, start_items, ordinal::omega()) {}

template <typename T>
lazy_sequence<T>::lazy_sequence(std::function<T(sequence<T>*)> rule,
                                sequence<T>* start_items,
                                const ordinal& length)
    : items(new mutable_array_sequence<T>()),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(length),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {
    if (!rule) {
        throw std::invalid_argument("Rule is empty");
    }

    if (start_items != nullptr) {
        int start_count = start_items->get_length();
        if (length.is_finite() && ordinal::finite(start_count) > length) {
            throw std::invalid_argument("Start items count is greater than sequence length");
        }

        IEnumerator<T>* it = start_items->get_enumerator();
        while (it->move_next()) {
            items->append(it->get_current());
        }
        delete it;
    }

    gen = new generator<T>(this, rule);
}

template <typename T>
lazy_sequence<T>::lazy_sequence(sequence<T>* source, typename generator<T>::insert_item_operation operation)
    : items(new mutable_array_sequence<T>()),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(ordinal::finite(0)),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {
    if (source == nullptr) {
        throw std::invalid_argument("Source sequence is null");
    }

    lazy_sequence<T>* source_copy = new lazy_sequence<T>(source);
    generator<T>* new_generator = nullptr;

    try {
        ordinal source_size = source_copy->get_ordinal_length();
        if (operation.index > source_size) {
            throw std::out_of_range("IndexOutOfRange");
        }

        if (source_size.is_finite() ||
            operation.index.get_omega_coefficient() == source_size.get_omega_coefficient()) {
            size = source_size + 1;
        } else {
            size = source_size;
        }

        new_generator = new generator<T>(this, source_copy, operation);
    } catch (...) {
        delete new_generator;
        delete source_copy;
        throw;
    }

    this->source = source_copy;
    gen = new_generator;
}

template <typename T>
lazy_sequence<T>::lazy_sequence(sequence<T>* source, typename generator<T>::remove_operation operation)
    : items(new mutable_array_sequence<T>()),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(ordinal::finite(0)),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {
    if (source == nullptr) {
        throw std::invalid_argument("Source sequence is null");
    }

    lazy_sequence<T>* source_copy = new lazy_sequence<T>(source);
    generator<T>* new_generator = nullptr;

    try {
        ordinal source_size = source_copy->get_ordinal_length();
        if (operation.count == 0) {
            if (operation.index > source_size) {
                throw std::out_of_range("IndexOutOfRange");
            }
            size = source_size;
        } else if (operation.index >= source_size) {
            throw std::out_of_range("IndexOutOfRange");
        } else if (source_size.is_finite()) {
            int source_length = source_size.get_count();
            int index = operation.index.get_count();
            int available = source_length - index;
            int removed = operation.count > available ? available : operation.count;
            size = ordinal::finite(source_length - removed);
        } else if (operation.index.get_omega_coefficient() == source_size.get_omega_coefficient()) {
            int available = source_size.get_finite_part() - operation.index.get_finite_part();
            int removed = operation.count > available ? available : operation.count;
            size = source_size - removed;
        } else {
            size = source_size;
        }

        new_generator = new generator<T>(this, source_copy, operation);
    } catch (...) {
        delete new_generator;
        delete source_copy;
        throw;
    }

    this->source = source_copy;
    gen = new_generator;
}

template <typename T>
lazy_sequence<T>::lazy_sequence(const sequence<T>* source, int start_index, int count)
    : lazy_sequence(source, ordinal::finite(start_index), ordinal::finite(count)) {
    if (start_index < 0 || count < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
}

template <typename T>
lazy_sequence<T>::lazy_sequence(const sequence<T>* source, const ordinal& start_index, const ordinal& count)
    : items(new mutable_array_sequence<T>()),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(count),
      slice_start(start_index),
      slice_enabled(true) {
    if (source == nullptr) {
        throw std::invalid_argument("Source sequence is null");
    }

    lazy_sequence<T>* source_copy = new lazy_sequence<T>(source);

    try {
        ordinal source_size = source_copy->get_ordinal_length();
        if (start_index > source_size ||
            (!count.is_zero() && start_index >= source_size) ||
            start_index + count > source_size) {
            throw std::out_of_range("IndexOutOfRange");
        }
    } catch (...) {
        delete source_copy;
        throw;
    }

    this->source = source_copy;
}

template <typename T>
lazy_sequence<T>::lazy_sequence(const lazy_sequence<T>& list)
    : items(nullptr),
      source(nullptr),
      suffix(nullptr),
      gen(nullptr),
      size(ordinal::finite(0)),
      slice_start(ordinal::finite(0)),
      slice_enabled(false) {
    *this = list;
}

template <typename T>
lazy_sequence<T>& lazy_sequence<T>::operator=(const lazy_sequence<T>& list) {
    if (this == &list) {
        return *this;
    }

    mutable_array_sequence<T>* new_items = new mutable_array_sequence<T>(*list.items);
    lazy_sequence<T>* new_source = nullptr;
    lazy_sequence<T>* new_suffix = nullptr;
    generator<T>* new_gen = nullptr;

    try {
        new_source = list.source == nullptr ? nullptr : new lazy_sequence<T>(*list.source);
        new_suffix = list.suffix == nullptr ? nullptr : new lazy_sequence<T>(*list.suffix);
        new_gen = list.gen == nullptr ? nullptr : new generator<T>(*list.gen);
    } catch (...) {
        delete new_gen;
        delete new_suffix;
        delete new_source;
        delete new_items;
        throw;
    }

    delete gen;
    delete suffix;
    delete source;
    delete items;

    items = new_items;
    source = new_source;
    suffix = new_suffix;
    gen = new_gen;
    size = list.size;
    slice_start = list.slice_start;
    slice_enabled = list.slice_enabled;

    if (gen != nullptr) {
        gen->set_owner(this);
        if (source != nullptr) {
            gen->set_source(source);
        }
    }

    return *this;
}

template <typename T>
lazy_sequence<T>::~lazy_sequence() {
    delete gen;
    delete suffix;
    delete source;
    delete items;
}

template <typename T>
const T& lazy_sequence<T>::get_first() const {
    return get(0);
}

template <typename T>
const T& lazy_sequence<T>::get_last() const {
    if (size.is_finite()) {
        if (size.get_count() == 0) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return get(size.get_count() - 1);
    }

    if (size.get_finite_part() > 0) {
        return get(size - 1);
    }

    if (items->get_length() != 0) {
        return (*items)[items->get_length() - 1];
    }

    throw std::out_of_range("IndexOutOfRange");
}

template <typename T>
const T& lazy_sequence<T>::get(int index) const {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    return get(ordinal::finite(index));
}

template <typename T>
const T& lazy_sequence<T>::get(const ordinal& index) const {
    if (index >= size) {
        throw std::out_of_range("IndexOutOfRange");
    }

    if (suffix != nullptr) {
        ordinal source_size = source == nullptr ? ordinal::finite(0) : source->get_ordinal_length();
        if (index < source_size) {
            return source->get(index);
        }
        return suffix->get(index - source_size);
    }

    if (slice_enabled) {
        if (source == nullptr) {
            throw std::out_of_range("IndexOutOfRange");
        }
        return source->get(slice_start + index);
    }

    if (index.is_infinite() && source != nullptr) {
        return source->get(index);
    }

    if (index.is_infinite()) {
        throw std::out_of_range("IndexOutOfRange");
    }

    int finite_index = index.get_count();
    materialize_to(finite_index);
    return (*items)[finite_index];
}

template <typename T>
sequence<T>* lazy_sequence<T>::get_subsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index < start_index) {
        throw std::out_of_range("IndexOutOfRange");
    }
    if (size.is_finite() && end_index > size.get_count()) {
        throw std::out_of_range("IndexOutOfRange");
    }

    int count = end_index - start_index;
    if (count == 0) {
        return new lazy_sequence<T>();
    }

    return new lazy_sequence<T>(this, start_index, count);
}

template <typename T>
int lazy_sequence<T>::get_length() const {
    ensure_finite();
    return size.get_count();
}

template <typename T>
ordinal lazy_sequence<T>::get_ordinal_length() const {
    return size;
}

template <typename T>
bool lazy_sequence<T>::is_infinite() const {
    return size.is_infinite();
}

template <typename T>
int lazy_sequence<T>::get_materialized_count() const {
    return items->get_length();
}

template <typename T>
sequence<T>* lazy_sequence<T>::get_materialized_items() const {
    return items;
}

template <typename T>
sequence<T>* lazy_sequence<T>::append(const T& item) {
    T buffer[1] = {item};
    lazy_sequence<T> tail(buffer, 1);
    return concat(&tail);
}

template <typename T>
sequence<T>* lazy_sequence<T>::prepend(const T& item) {
    typename generator<T>::insert_item_operation operation{ordinal::finite(0), item};
    return new lazy_sequence<T>(this, operation);
}

template <typename T>
sequence<T>* lazy_sequence<T>::insert_at(const T& item, int index) {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }

    typename generator<T>::insert_item_operation operation{
        ordinal::finite(index),
        item
    };
    return new lazy_sequence<T>(this, operation);
}

template <typename T>
sequence<T>* lazy_sequence<T>::insert_at(const sequence<T>* insert_items, int index) {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    if (insert_items == nullptr) {
        return new lazy_sequence<T>(*this);
    }

    ordinal insert_index = ordinal::finite(index);
    if (insert_index > size) {
        throw std::out_of_range("IndexOutOfRange");
    }

    lazy_sequence<T>* prefix = nullptr;
    lazy_sequence<T>* middle = nullptr;
    lazy_sequence<T>* suffix_part = nullptr;
    sequence<T>* left = nullptr;
    sequence<T>* result = nullptr;

    try {
        prefix = new lazy_sequence<T>(this, ordinal::finite(0), insert_index);
        middle = new lazy_sequence<T>(insert_items);
        ordinal suffix_size = size.is_finite() ? ordinal::finite(size.get_count() - index) : size;
        suffix_part = new lazy_sequence<T>(this, insert_index, suffix_size);

        left = prefix->concat(middle);
        lazy_sequence<T>* left_lazy = dynamic_cast<lazy_sequence<T>*>(left);
        if (left_lazy == nullptr) {
            throw std::logic_error("Concat result is not lazy_sequence");
        }

        result = left_lazy->concat(suffix_part);
    } catch (...) {
        delete result;
        delete left;
        delete suffix_part;
        delete middle;
        delete prefix;
        throw;
    }

    delete left;
    delete suffix_part;
    delete middle;
    delete prefix;
    return result;
}

template <typename T>
sequence<T>* lazy_sequence<T>::concat(const sequence<T>* list) {
    if (list == nullptr) {
        return new lazy_sequence<T>(*this);
    }

    lazy_sequence<T>* result = new lazy_sequence<T>();
    result->source = new lazy_sequence<T>(this);
    result->suffix = new lazy_sequence<T>(list);
    result->size = result->source->get_ordinal_length() + result->suffix->get_ordinal_length();
    return result;
}

template <typename T>
sequence<T>* lazy_sequence<T>::remove_at(int index) {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    return remove_range(index, 1);
}

template <typename T>
sequence<T>* lazy_sequence<T>::remove_range(int index, int count) {
    if (index < 0 || count < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }

    typename generator<T>::remove_operation operation{
        ordinal::finite(index),
        count
    };
    return new lazy_sequence<T>(this, operation);
}

template <typename T>
IEnumerator<T>* lazy_sequence<T>::get_enumerator() const {
    return new lazy_enumerator(this);
}

template <typename T>
lazy_sequence<T>* lazy_sequence<T>::create_empty() const {
    return new lazy_sequence<T>();
}

template <typename T>
void lazy_sequence<T>::materialize_to(int index) const {
    if (index < 0 || (size.is_finite() && index >=size.get_count())) {
        throw std::out_of_range("IndexOutOfRange");
    }

    while (items->get_length() <= index) {
        if (gen == nullptr || !gen->has_next()) {
            throw std::out_of_range("IndexOutOfRange");
        }
        items->append(gen->get_next());
    }
}

template <typename T>
void lazy_sequence<T>::ensure_finite() const {
    if (size.is_infinite()) {
        throw std::logic_error("Sequence is infinite");
    }
}

template <typename T>
class lazy_sequence<T>::lazy_enumerator : public IEnumerator<T> {
public:
    lazy_enumerator(const lazy_sequence<T>* owner)
        : owner(owner), index(-1) {}

    bool move_next() override {
        if (owner->size.is_finite() &&
            index + 1 >= owner->size.get_count()) {
            return false;
        }

        ++index;
        owner->get(index);
        return true;
    }

    const T& get_current() const override {
        if (index < 0) {
            throw std::out_of_range("Enumerator is out of bounds");
        }
        return owner->get(index);
    }

private:
    const lazy_sequence<T>* owner;
    int index;
};
