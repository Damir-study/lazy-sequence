#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "IEnumerator.h"

template <typename T>
class linked_list {
public:
    linked_list(const T* items, int count); // Копировать элементы из переданного массива
    linked_list(); // Создать пустой список
    linked_list(const linked_list<T> &list); // Копирующий конструктор
    ~linked_list();

    const T& get_first() const; // Получить первый элемент в списке
    const T& get_last() const; // Получить последний элемент в списке
    linked_list<T>* get_sub_list(int start_index, int end_index) const; // Получить список из всех элементов, начиная с startIndex и заканчивая endIndex
    int get_size() const; // Получить длину списка

    void append(const T& item); // Добавляет элемент в конец списка
    void prepend(const T& item); // Добавляет элемент в начало списка
    void insert_at(const T& item, int index); // Вставляет элемент в заданную позицию

    linked_list<T>* concat(const linked_list<T> &list) const; // Сцепляет два списка

    IEnumerator<T>* get_enumerator() const;
private:
    struct node {
        T data;
        node* next;
    };
    node *head, *tail;
    int size;

    class list_enumerator : public IEnumerator<T> {
        public:
            list_enumerator(node* start_node) : current(start_node), started(false) {}
            bool move_next() override;
            const T& get_current() const override;
        private:
            node* current;
            bool started;
    };
};

#include "linked_list.tpp"

#endif //LINKED_LIST_H