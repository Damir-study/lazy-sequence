#include "mutable_list_sequence.h"


template <typename T>
list_sequence<T>* mutable_list_sequence<T>::instance() {
    return this;
}

template <typename T>
list_sequence<T>* mutable_list_sequence<T>::clone() const {
    return new mutable_list_sequence<T>(*this);
}

template <typename T>
list_sequence<T>* mutable_list_sequence<T>::create_empty() const{
    return new mutable_list_sequence<T>;
}

