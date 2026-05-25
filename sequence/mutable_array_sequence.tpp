#include "mutable_array_sequence.h"


template <typename T>
array_sequence<T>* mutable_array_sequence<T>::instance() {
    return this;
}

template <typename T>
array_sequence<T>* mutable_array_sequence<T>::clone() const {
    return new mutable_array_sequence<T>(*this);
}

template <typename T>
array_sequence<T>* mutable_array_sequence<T>::create_empty() const{
    return new mutable_array_sequence<T>;
}

