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

        ordinal suffix_index = source_size.is_finite() && index.is_infinite()
                                   ? index
                                   : index - source_size;
        return suffix->get(suffix_index);
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
    return insert_at(item, size);
}

template <typename T>
sequence<T>* lazy_sequence<T>::prepend(const T& item) {
    return insert_at(item, ordinal::finite(0));
}

template <typename T>
sequence<T>* lazy_sequence<T>::insert_at(const T& item, int index) {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }

    return insert_at(item, ordinal::finite(index));
}

template <typename T>
sequence<T>* lazy_sequence<T>::insert_at(const T& item, const ordinal& index) {
    T buffer[1] = {item};
    lazy_sequence<T> single_item(buffer, 1);
    return insert_at(&single_item, index);
}

template <typename T>
sequence<T>* lazy_sequence<T>::insert_at(const sequence<T>* insert_items, int index) {
    if (index < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }

    return insert_at(insert_items, ordinal::finite(index));
}

template <typename T>
sequence<T>* lazy_sequence<T>::insert_at(const sequence<T>* insert_items, const ordinal& index) {
    if (insert_items == nullptr) {
        return new lazy_sequence<T>(*this);
    }

    if (index > size) {
        throw std::out_of_range("IndexOutOfRange");
    }

    lazy_sequence<T>* prefix = nullptr;
    lazy_sequence<T>* middle = nullptr;
    lazy_sequence<T>* suffix_part = nullptr;
    sequence<T>* left = nullptr;
    sequence<T>* result = nullptr;

    try {
        prefix = new lazy_sequence<T>(this, ordinal::finite(0), index);
        middle = new lazy_sequence<T>(insert_items);
        suffix_part = new lazy_sequence<T>(this, index, get_tail_length_from(index));

        left = prefix->concat(middle);
        result = left->concat(suffix_part);
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
    return remove_at(ordinal::finite(index));
}

template <typename T>
sequence<T>* lazy_sequence<T>::remove_at(const ordinal& index) {
    return remove_range(index, 1);
}

template <typename T>
sequence<T>* lazy_sequence<T>::remove_range(int index, int count) {
    if (index < 0 || count < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }

    return remove_range(ordinal::finite(index), count);
}

template <typename T>
sequence<T>* lazy_sequence<T>::remove_range(const ordinal& index, int count) {
    if (count < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }
    if (index > size || (count > 0 && index >= size)) {
        throw std::out_of_range("IndexOutOfRange");
    }
    if (count == 0) {
        return new lazy_sequence<T>(*this);
    }

    ordinal end = index + count;
    if (end > size) {
        end = size;
    }

    lazy_sequence<T>* prefix = nullptr;
    lazy_sequence<T>* suffix_part = nullptr;
    sequence<T>* result = nullptr;

    try {
        prefix = new lazy_sequence<T>(this, ordinal::finite(0), index);
        suffix_part = new lazy_sequence<T>(this, end, get_tail_length_from(end));
        result = prefix->concat(suffix_part);
    } catch (...) {
        delete result;
        delete suffix_part;
        delete prefix;
        throw;
    }

    delete suffix_part;
    delete prefix;
    return result;
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
ordinal lazy_sequence<T>::get_tail_length_from(const ordinal& start) const {
    if (start > size) {
        throw std::out_of_range("IndexOutOfRange");
    }
    if (start == size) {
        return ordinal::finite(0);
    }
    if (size.is_finite()) {
        return ordinal::finite(size.get_count() - start.get_count());
    }
    if (start.is_finite()) {
        return size;
    }
    return size - start;
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
