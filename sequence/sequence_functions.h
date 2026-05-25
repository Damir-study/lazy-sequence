#ifndef SEQUENCE_FUNCTIONS_H
#define SEQUENCE_FUNCTIONS_H

#include "mutable_list_sequence.h"

template <typename T>
sequence<sequence<T>*>* split(const sequence<T>* seq, bool (*split_func)(const T&));
template <typename T>
sequence<sequence<T>*>* zipN(const sequence<sequence<T>*>* sequences);
template <typename T>
sequence<sequence<T>*>* unzipN(const sequence<sequence<T>*>* sequences);

#include "sequence_functions.tpp"

#endif // SEQUENCE_FUNCTIONS_H