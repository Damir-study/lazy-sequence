#include "immutable_list_sequence.h"

template <typename T>
list_sequence<T>* immutable_list_sequence<T>::instance() {
    return clone();
}

template <typename T>
list_sequence<T>* immutable_list_sequence<T>::clone() const{
    return new immutable_list_sequence<T>(*this);
}

template <typename T>
list_sequence<T>* immutable_list_sequence<T>::create_empty() const{
    return new immutable_list_sequence<T>;
}
