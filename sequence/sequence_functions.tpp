#include "sequence_functions.h"

template <typename T>
sequence<sequence<T>*>* split(const sequence<T>* seq, bool (*split_func)(const T&)) {
    auto* result = new mutable_list_sequence<sequence<T>*>; 
    auto* current_fragment = seq->create_empty();
    IEnumerator<T>* it = seq->get_enumerator();

    while (it->move_next()) {
        const T& data = it->get_current();
        if (split_func(data)) {
            result->append(current_fragment);
            current_fragment = seq->create_empty();
        }
        else {
            current_fragment = current_fragment->append(data);
        }
    }
    result->append(current_fragment);

    delete it;
    return result;
}


template <typename T>
sequence<sequence<T>*>* zipN(const sequence<sequence<T>*>* sequences) {
    if (!sequences) return nullptr;

    if (sequences->get_length() == 0) {
        return new mutable_list_sequence<sequence<T>*>;
    }

    int n = sequences->get_length();
    
    // Массив указателей на итераторы
    IEnumerator<T>** iterators = new IEnumerator<T>*[n];
    IEnumerator<sequence<T>*>* outer_it = sequences->get_enumerator();
    
    int idx = 0;
    sequence<T>* prototype = nullptr;

    // сами итераторы
    while (outer_it->move_next()) {
        sequence<T>* current_inner = outer_it->get_current();
        if (idx == 0) prototype = current_inner;  // Сохраняем первый для прототипа
        iterators[idx++] = current_inner->get_enumerator();
    }
    delete outer_it;

    // результирующий контейнер
    sequence<sequence<T>*>* result = new mutable_list_sequence<sequence<T>*>;

    // добавляем в итоговую последовательность
    while (true) {
        bool all_have_next = true;
        for (int i = 0; i < n; ++i) {
            if (!iterators[i]->move_next()) {
                all_have_next = false;
                break;
            }
        }

        if (!all_have_next) break;

        // Создаем новую "колонку"
        sequence<T>* current_column = prototype->create_empty();
        
        for (int i = 0; i < n; ++i) {
            current_column = current_column->append(iterators[i]->get_current());
        }
        
        result->append(current_column);
    }
    
    for (int i = 0; i < n; ++i) {
        delete iterators[i];
    }
    delete[] iterators;

    return result;
}

template <typename T>
sequence<sequence<T>*>* unzipN(const sequence<sequence<T>*>* sequences) {
    if (!sequences) return nullptr;
    if (sequences->get_length() == 0) {
        return new mutable_list_sequence<sequence<T>*>;
    }

    // Узнаем, сколько будет "столбцов" (длина первой "строки")
    IEnumerator<sequence<T>*>* outer_it = sequences->get_enumerator();
    
    sequence<T>* first_row = outer_it->get_current();
    int target_count = first_row->get_length();
    
    // Создаем результирующую структуру
    sequence<sequence<T>*>* result = new mutable_list_sequence<sequence<T>*>;
    for (int i = 0; i < target_count; ++i) {
        // Создаем пустые "столбцы" того же типа, что и строки
        result->append(new mutable_list_sequence<T>());
    }

    // Распределяем данные
    delete outer_it;
    outer_it = sequences->get_enumerator();

    while (outer_it->move_next()) {
        sequence<T>* current_row = outer_it->get_current();
        IEnumerator<T>* row_it = current_row->get_enumerator();
        IEnumerator<sequence<T>*>* res_it = result->get_enumerator();
        
        // Распихиваем элементы строки по столбцам результата
        while (row_it->move_next() && res_it->move_next()) {
            res_it->get_current()->append(row_it->get_current());
        }
        
        delete row_it;
        delete res_it;
    }

    delete outer_it;
    return result;
}