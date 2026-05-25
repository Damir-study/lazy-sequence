#include "array_sequence.h"
#include <stdexcept>

template <typename T>
array_sequence<T>::array_sequence(const T* items, int count) {
    this->items = new dynamic_array<T>(items, count);
}

template <typename T>
array_sequence<T>::array_sequence() {
    items = new dynamic_array<T>(0);
}

template <typename T>
array_sequence<T>::array_sequence(int count) {
    items = new dynamic_array<T>(count);
}

template <typename T>
array_sequence<T>::array_sequence(const array_sequence<T> &array_seq) {
    items = new dynamic_array<T>((*array_seq.items));
}

template <typename T>
array_sequence<T>::~array_sequence() {
    delete items;
}


template <typename T>
const T& array_sequence<T>::get_first() const {
    if (get_length() == 0) throw std::out_of_range("IndexOutOfRange");
    return (*items)[0];
}

template <typename T>
const T& array_sequence<T>::get_last() const {
    if (get_length() == 0) throw std::out_of_range("IndexOutOfRange");
    return (*items)[items->get_size()-1];
}

template <typename T>
const T& array_sequence<T>::operator[](int index) const {
    if (index >= get_length() || index < 0) throw std::out_of_range("IndexOutOfRange");
    return (*items)[index];
}

template <typename T>
int array_sequence<T>::get_length() const {
    return items->get_size();
}

template <typename T>
sequence<T>* array_sequence<T>::get_subsequence(int start_index, int end_index) const {
    if (start_index < 0 || end_index < start_index || end_index >= get_length()) 
        throw std::out_of_range("IndexOutOfRange");

    sequence<T> *result = create_empty();

    for (int i = start_index; i < end_index; ++i) {
        sequence<T> *old_ptr = result;
        result = result->append((*this->items)[i]);
        if (result != old_ptr) {
            delete old_ptr;
        }
    }

    return result;
}


template <typename T>
sequence<T>* array_sequence<T>::append(const T& item) {
    array_sequence<T> *new_sequence = instance();

    int new_size = new_sequence->items->get_size()+1;
    new_sequence->items->resize(new_size);

    new_sequence->items->set(new_sequence->items->get_size()-1, item);

    return new_sequence;
}

template <typename T>
sequence<T>* array_sequence<T>::prepend(const T& item) {
    array_sequence<T> *new_sequence = instance();

    int new_size = new_sequence->items->get_size()+1;
    new_sequence->items->resize(new_size);

    for (int i=new_size-1; i>0;--i) {
        new_sequence->items->set(i, (*new_sequence->items)[i-1]);
    }
    new_sequence->items->set(0, item);

    return new_sequence;
}

template <typename T>
sequence<T>* array_sequence<T>::insert_at(const T& item, int index) {
    if (index > get_length() || index < 0) throw std::out_of_range("IndexOutOfRange");

    array_sequence<T> *new_sequence = instance();

    int new_size = new_sequence->items->get_size()+1;
    new_sequence->items->resize(new_size);

    for (int i=new_size-1; i>index;--i) {
        new_sequence->items->set(i, (*new_sequence->items)[i-1]);
    }
    new_sequence->items->set(index, item);

    return new_sequence;
}

template <typename T>
sequence<T>* array_sequence<T>::concat(const sequence<T>* arr) {
    array_sequence<T>* new_sequence = instance();

    if (!arr || arr->get_length() == 0) {
        return new_sequence;
    }

    int old_size = new_sequence->get_length();
    int additional_size = arr->get_length();
    new_sequence->items->resize(old_size + additional_size);

    IEnumerator<T>* it = arr->get_enumerator();
    int i = old_size;
    
    while (it->move_next()) {
        new_sequence->items->set(i, it->get_current());
        i++;
    }

    delete it;
    return new_sequence;
}



template <typename T>
IEnumerator<T>* array_sequence<T>::get_enumerator() const {
    return this->items->get_enumerator();
}
