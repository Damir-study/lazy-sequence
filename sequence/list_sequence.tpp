#include "list_sequence.h"
#include <stdexcept>

template <typename T>
list_sequence<T>::list_sequence(const T* items, int count) {
    this->items = new linked_list<T>(items, count);
}

template <typename T>
list_sequence<T>::list_sequence() {
    this->items = new linked_list<T>;
}

template <typename T>
list_sequence<T>::list_sequence(const list_sequence<T> &list_seq) {
    this->items = new linked_list<T>(*(list_seq.items));
}

template <typename T>
list_sequence<T>::~list_sequence() {
    delete items;
}



template <typename T>
const T& list_sequence<T>::get_first() const{
    if (get_length() == 0) throw std::out_of_range("IndexOutOfRange");
    return items->get_first();
}

template <typename T>
const T& list_sequence<T>::get_last() const{
    if (get_length() == 0) throw std::out_of_range("IndexOutOfRange");
    return items->get_last();
}

template <typename T>
int list_sequence<T>::get_length() const{
    return items->get_size();
}

template <typename T>
sequence<T>* list_sequence<T>::get_subsequence(int start_index, int end_index) const{
    if (start_index < 0 || end_index < start_index || end_index >= get_length()) throw std::out_of_range("IndexOutOfRange");
    list_sequence<T>* new_sequence = create_empty();
    delete new_sequence->items;
    new_sequence->items = items->get_sub_list(start_index, end_index);
    return new_sequence;
}



template <typename T>
sequence<T>* list_sequence<T>::append(const T& item) {
    list_sequence<T> *new_sequence = instance();
    new_sequence->items->append(item);
    return new_sequence;
}

template <typename T>
sequence<T>* list_sequence<T>::prepend(const T& item) {
    list_sequence<T> *new_sequence = instance();
    new_sequence->items->prepend(item);
    return new_sequence;
}

template <typename T>
sequence<T>* list_sequence<T>::insert_at(const T& item, int index) {
    if (index > get_length() || index < 0) throw std::out_of_range("IndexOutOfRange");
    list_sequence<T> *new_sequence = instance();
    new_sequence->items->insert_at(item, index);
    return new_sequence;
}

template <typename T>
sequence<T>* list_sequence<T>::concat(const sequence<T>* list) {
    list_sequence<T>* new_seq = this->instance();
    
    IEnumerator<T>* it = list->get_enumerator();
    
    while (it->move_next()) {
        new_seq->items->append(it->get_current());
    }
    
    delete it;
    return new_seq;
}

template <typename T>
IEnumerator<T>* list_sequence<T>::get_enumerator() const {
    return this->items->get_enumerator();
}
