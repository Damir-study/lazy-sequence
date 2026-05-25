#include "sequence.h"


template <typename T>
sequence<T>* sequence<T>::slice(int index, int count, const sequence<T>* seq) const {
    int len = this->get_length();
    
    int actual_index = (index < 0) ? (len + index) : index;
    if (actual_index < 0 || actual_index >= len) {
        throw std::out_of_range("Index out of range");
    }
    if (count < 0) {
        throw std::invalid_argument("Count cannot be negative");
    }

    sequence<T>* result = this->create_empty();

    IEnumerator<T>* it = this->get_enumerator();
    for (int i = 0; i < actual_index && it->move_next(); ++i) result = result->append(it->get_current());

    if (seq != nullptr) {
        IEnumerator<T>* it_s = seq->get_enumerator();
        while (it_s->move_next()) {
            result = result->append(it_s->get_current());
        }
        delete it_s;
    }

    for (int i = 0; i < count && it->move_next(); ++i) {}

    while (it->move_next()) {
        result = result->append(it->get_current());
    }

    delete it;
    return result;
}


template <typename T>
sequence<T>* sequence<T>::where(bool (*where_func)(const T&)) const {
    sequence<T>* result = this->create_empty();
    IEnumerator<T>* it = this->get_enumerator();

    while (it->move_next()) {
        const T& item = it->get_current();
        if (where_func(item)) {
            result = result->append(item);
        }
    }

    delete it;
    return result;
}

template <typename T>
int sequence<T>::find(bool (*find_func)(const T&)) const {
    IEnumerator<T>* it = this->get_enumerator();
    int index = 0;

    while (it->move_next()) {
        if (find_func(it->get_current())) {
            delete it;
            return index;
        }
        index++;
    }

    delete it;
    return -1;
}