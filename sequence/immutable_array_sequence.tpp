#include "immutable_array_sequence.h"

template <typename T>
array_sequence<T>* immutable_array_sequence<T>::instance() {
    return clone();
}

template <typename T>
array_sequence<T>* immutable_array_sequence<T>::clone() const{
    return new immutable_array_sequence<T>(*this);
}

template <typename T>
array_sequence<T>* immutable_array_sequence<T>::create_empty() const{
    return new immutable_array_sequence<T>;
}
