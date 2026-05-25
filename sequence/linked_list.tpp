#include "linked_list.h"
#include <stdexcept>

template <typename T>
linked_list<T>::linked_list(const T* items, int count) : head(nullptr), tail(nullptr), size(0) {
    for (int i = 0; i < count; ++i) {
        append(items[i]);
    }
}

template <typename T>
linked_list<T>::linked_list() : head(nullptr), tail(nullptr), size(0) {}

template <typename T>
linked_list<T>::linked_list(const linked_list<T> &list) : head(nullptr), tail(nullptr), size(0) {
    node* current = list.head;
    while (current != nullptr) {
        this->append(current->data);
        current = current->next;
    }
}

template <typename T>
linked_list<T>::~linked_list() {
    node* current = head;
    while (current != nullptr) {
        node* next = current->next;
        delete current;
        current = next;
    }
}

template <typename T>
const T& linked_list<T>::get_first() const {
    if (head==nullptr) throw std::out_of_range("IndexOutOfRange");
    return head->data;
}

template <typename T>
const T& linked_list<T>::get_last() const {
    if (head==nullptr) throw std::out_of_range("IndexOutOfRange");
    return tail->data;
}

template <typename T>
int linked_list<T>::get_size() const {
    return size;
}

template <typename T>
void linked_list<T>::append(const T& item) {
    ++size;
    node *new_node = new node{item,nullptr};

    if (head == nullptr) {
        head = tail = new_node;
        return;
    }

    tail->next = new_node;
    tail = new_node;
}

template <typename T>
void linked_list<T>::prepend(const T& item) {
    node* new_node = new node{item, head};
    head = new_node;
    
    if (tail == nullptr) {
        tail = new_node;
    }
    
    ++size;
}

template <typename T>
void linked_list<T>::insert_at(const T& item, int index) {
    if (index<0) throw std::out_of_range("IndexOutOfRange");

    int count = 0;

    node **current = &this->head;
    while (*current && count < index) {
        current = &(*current)->next;
        ++count;
    }

    if (count < index) throw std::out_of_range("IndexOutOfRange");

    *current = new node{item, (*current)};
    if ((*current)->next == nullptr) {
        this->tail = *current;
    }
    ++size;
}


template <typename T>
linked_list<T>* linked_list<T>::concat(const linked_list<T> &list) const {
    linked_list<T> *result = new linked_list<T>(*this);
    
    node *current = list.head;
    while (current) {
        result->append(current->data);
        current = current->next;
    }
    
    return result;
}

template <typename T>
linked_list<T>* linked_list<T>::get_sub_list(int start_index, int end_index) const {
    if (start_index < 0 || end_index > size || start_index > end_index) {
        throw std::out_of_range("IndexOutOfRange");
    }

    linked_list<T>* new_list = new linked_list<T>();
    
    node* current = head;
    for (int i = 0; i < start_index; ++i) {
        current = current->next;
    }

    for (int i = start_index; i < end_index; ++i) {
        new_list->append(current->data);
        current = current->next;
    }

    return new_list;
}



template <typename T>
bool linked_list<T>::list_enumerator::move_next() {
    if (!started) {
        started = true;
    } else if (current != nullptr) {
        current = current->next;
    }
    return current != nullptr;
}

template <typename T>
const T& linked_list<T>::list_enumerator::get_current() const{
    if (current == nullptr) {
        throw std::out_of_range("Enumerator is out of bounds");
    }
    return current->data;
}

template <typename T>
IEnumerator<T>* linked_list<T>::get_enumerator() const {
    return new list_enumerator(head);
}