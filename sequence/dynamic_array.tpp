#include "dynamic_array.h"
#include <stdexcept>

template <typename T>
dynamic_array<T>::dynamic_array(const T* items, int count) {

    this->data = new T[count*2]();
    this->size = count;

    for (int i=0; i<count;++i) {
        set(i,*(items+i));
    }

    capacity = count*2;
}

template <typename T>
dynamic_array<T>::dynamic_array(int count) {
    data = new T[count*2]();

    this->size = count;
    this->capacity = count*2;
}

template <typename T>
dynamic_array<T>::dynamic_array(const dynamic_array<T> &dyn_arr) {
    this->size = dyn_arr.get_size();
    this->capacity = dyn_arr.get_capacity();

    this->data = new T[capacity]();

    for (int i=0; i<this->size;++i) {
        set(i, dyn_arr[i]);
    }
}

template <typename T>
dynamic_array<T>::~dynamic_array() {
    delete[] data;
}


template <typename T>
const T& dynamic_array<T>::operator[](int index) const {
    if (index >= size || index < 0) throw std::out_of_range("IndexOutOfRange");
    return *(data+index);
}

template <typename T>
int dynamic_array<T>::get_size() const {
    return size;
}

template <typename T>
int dynamic_array<T>::get_capacity() const {
    return capacity;
}

template <typename T>
void dynamic_array<T>::set(int index, const T& value) {
    if (index > size || index < 0) throw std::out_of_range("IndexOutOfRange");
    data[index] = value;
}

template <typename T>
void dynamic_array<T>::size_up(int min_capacity) {
    int new_capacity = capacity > 0 ? capacity * 2 : 1;
    while (new_capacity < min_capacity) new_capacity *= 2;

    T *new_data = new T[new_capacity]();

    for (int i=0;i<size;++i) {
        *(new_data+i) = (*this)[i];
    }

    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

template <typename T>
void dynamic_array<T>::size_down() {
    int new_capacity = (int)(capacity*0.5);
    T *new_data = new T[new_capacity]();

    for (int i=0;i<size;++i) {
        *(new_data+i) = (*this)[i];
    }

    delete[] data;
    data = new_data;
    capacity = new_capacity;
}

template <typename T>
void dynamic_array<T>::resize(int new_size) {
    if (new_size < 0) throw std::out_of_range("IndexOutOfRange");

    if (new_size > capacity) {
        size_up(new_size);
    } else if (new_size < capacity / 4 && capacity > 10) { 
        size_down(); 
    }
    size = new_size;
}



template <typename T>
const T& dynamic_array<T>::array_enumerator::get_current() const{
    return data[current_index];
}

template <typename T>
bool dynamic_array<T>::array_enumerator::move_next() {
    if (current_index < size - 1) {
        ++current_index;
        return true;
    }
    return false;
}

template <typename T>
IEnumerator<T>* dynamic_array<T>::get_enumerator() const {
    return new array_enumerator(this->data, this->size);
}