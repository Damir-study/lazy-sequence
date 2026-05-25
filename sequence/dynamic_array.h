#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "IEnumerator.h"

template <typename T>
class dynamic_array {
public:
    dynamic_array(const T* items, int count); // Копировать элементы изпереданного массива
    dynamic_array(int count); // Создать массив заданной длины
    dynamic_array(const dynamic_array<T> &dyn_arr); // Копирующий конструктор
    ~dynamic_array();

    const T& operator[](int index) const; // Получить элемент по индексу
    int get_size() const; // Получить размер массива
    int get_capacity() const;

    void set(int index, const T& value); // Задать элемент по индексу
    void resize(int new_size); // Изменить размер массива

    IEnumerator<T>* get_enumerator() const;
private:
    T* data;
    int size;
    int capacity;

    void size_up(int min_capacity); 
    void size_down();

    class array_enumerator : public IEnumerator<T> {
        public:
            array_enumerator(T* items, int count) : data(items), size(count), current_index(-1) {}
            bool move_next() override;
            const T& get_current() const override;
        private:
            T* data;
            int size;
            int current_index;
    };
};

#include "dynamic_array.tpp"

#endif //DYNAMIC_ARRAY_H