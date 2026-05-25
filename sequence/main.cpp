#include <iostream>
#include <limits>
#include <string>

#include "mutable_array_sequence.h"
#include "immutable_array_sequence.h"
#include "mutable_list_sequence.h"
#include "immutable_list_sequence.h"
#include "sequence_functions.h"

// Безопасный ввод целого числа
int get_int() {
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Enter a number: ";
    }
    return val;
}

// Красивый вывод последовательности
void print_seq(const std::string& label, const sequence<int>* seq) {
    if (!seq) {
        std::cout << label << ": [ NULL ]" << std::endl;
        return;
    }
    std::cout << label << ": [ ";
    IEnumerator<int>* it = seq->get_enumerator();
    while (it->move_next()) {
        std::cout << it->get_current() << " ";
    }
    std::cout << "]" << std::endl;
    delete it;
}

// Предикаты для функций where, find, split
bool is_even(const int& x) { return x % 2 == 0; }
bool is_positive(const int& x) { return x > 0; }
bool is_negative(const int& x) { return x < 0; }


int main() {
    sequence<int>* current = nullptr;
    bool is_mutable = true;

    // Выбор базовой структуры
    std::cout << "Select base data structure:\n"
              << "1) Array Sequence\n"
              << "2) Linked List Sequence\n"
              << "Choice: ";
    int struct_choice = get_int();

    // Выбор мутабельности
    std::cout << "\nSelect mutability:\n"
              << "1) Mutable\n"
              << "2) Immutable\n"
              << "Choice: ";
    int mut_choice = get_int();
    is_mutable = mut_choice == 1;

    // Инициализация полиморфного указателя
    if (struct_choice == 1 && mut_choice == 1) {
        current = new mutable_array_sequence<int>();
    } else if (struct_choice == 1 && mut_choice == 2) {
        current = new immutable_array_sequence<int>();
    } else if (struct_choice == 2 && mut_choice == 1) {
        current = new mutable_list_sequence<int>();
    } else if (struct_choice == 2 && mut_choice == 2) {
        current = new immutable_list_sequence<int>();
    } else {
        std::cout << "Invalid configuration. Exiting.\n";
        return 1;
    }

    std::cout << "\nHow many elements do you want to add initially? ";
    int initial_count = get_int();

    for (int i = 0; i < initial_count; ++i) {
        std::cout << "Enter element " << i << ": ";
        int val = get_int();

        if (is_mutable) {
            current->append(val);
        } else {
            sequence<int>* next_version = current->append(val);
            delete current;
            current = next_version;
        }
    }

    // Главный цикл UI
    while (true) {
        std::cout << "\n========================================\n";
        print_seq("Current Sequence", current);
        std::cout << "----------------------------------------\n";
        std::cout << "CATEGORIES:\n";
        std::cout << "1) Elementary (GetFirst, GetLast, GetLength)\n";
        std::cout << "2) Transformations (Append, Prepend, InsertAt, Concat)\n";
        std::cout << "3) Search/Filter (Where, Find)\n";
        std::cout << "4) Complex (Slice, Split, ZipN)\n";
        std::cout << "0) Exit\n";
        std::cout << "Choose a category: ";
        
        int category = get_int();

        if (category == 0) {
            delete current;
            break;
        }

        switch (category) {
            case 1: {
                std::cout << "\n--- Elementary Operations ---\n";
                std::cout << "1) Get First item\n";
                std::cout << "2) Get Last item\n";
                std::cout << "3) Get Length\n";
                std::cout << "0) Back to main menu\n";
                std::cout << "Choose an action: ";
                
                int action = get_int();

                try {
                    switch (action) {
                        case 1:
                            {
                            int getter = current->get_first();
                            std::cout << "Result: First item is " << getter << "\n";
                            break;
                            }
                        case 2:
                            {
                            int getter = current->get_last();
                            std::cout << "Result: Last item is " << getter << "\n";
                            break;
                            }
                        case 3:
                            {
                            int getter = current->get_length();
                            std::cout << "Result: Length is " << getter << "\n";
                            break;
                            }
                        case 0:
                            std::cout << "Returning to main menu...\n";
                            break;
                        default:
                            std::cout << "Invalid action choice.\n";
                            break;
                    }
                } 
                catch (const std::exception& e) {
                    std::cerr << "EXCEPTION CAUGHT: " << e.what() << "\n";
                }
                break;
            } // case 1

            case 2: {
                std::cout << "\n--- Transformations ---\n";
                std::cout << "1) Append (add to end)\n";
                std::cout << "2) Prepend (add to start)\n";
                std::cout << "3) InsertAt (add by index)\n";
                std::cout << "4) Concat (join with another sequence)\n";
                std::cout << "0) Back to main menu\n";
                std::cout << "Choose an action: ";
                
                int action = get_int();
                if (action == 0) break;

                try {
                    int val, index;
                    sequence<int>* result = nullptr;

                    switch (action) {
                        case 1: // APPEND
                            std::cout << "Enter value: ";
                            val = get_int();
                            if (is_mutable) {
                                current->append(val);
                                std::cout << "Success: Item appended to mutable sequence.\n";
                            } else {
                                result = current->append(val);
                            }
                            break;

                        case 2: // PREPEND
                            std::cout << "Enter value: ";
                            val = get_int();
                            if (is_mutable) {
                                current->prepend(val);
                                std::cout << "Success: Item prepended to mutable sequence.\n";
                            } else {
                                result = current->prepend(val);
                            }
                            break;

                        case 3: // INSERT_AT
                            std::cout << "Enter value: ";
                            val = get_int();
                            std::cout << "Enter index: ";
                            index = get_int();
                            if (is_mutable) {
                                current->insert_at(val, index);
                                std::cout << "Success: Item inserted at index " << index << ".\n";
                            } else {
                                result = current->insert_at(val, index);
                            }
                            break;

                        case 4: { // CONCAT
                            std::cout << "How many elements in the SECOND sequence? ";
                            int count = get_int();
                            
                            sequence<int>* second = current->create_empty();
                            for(int i = 0; i < count; ++i) {
                                std::cout << "Enter element " << i << ": ";
                                second = second->append(get_int());
                            }

                            if (is_mutable) {
                                current->concat(second);
                                std::cout << "Success: Sequences concatenated (mutable).\n";
                            } else {
                                result = current->concat(second);
                            }
                            delete second;
                            break;
                        }
                        default:
                            std::cout << "Unknown action.\n";
                            break;
                    }

                    // Обработка Immutable результата
                    if (!is_mutable && result != nullptr) {
                        std::cout << "\n>>> IMMUTABLE OPERATION RESULT <<<\n";
                        print_seq("New Resulting Sequence", result);
                        print_seq("Original Sequence (Unchanged)", current);
                        delete result;
                    }

                } catch (const std::exception& e) {
                    std::cerr << "\n ERROR: " << e.what() << std::endl;
                }
                break;
            } // case 2
            case 3: {
                std::cout << "\n--- Search & Filter ---\n";
                std::cout << "1) Where (Filter elements)\n";
                std::cout << "2) Find (Search for index)\n";
                std::cout << "0) Back to main menu\n";
                std::cout << "Choose an action: ";
                
                int action = get_int();
                if (action == 0) break;

                std::cout << "\nSelect condition:\n";
                std::cout << "1) Even numbers\n";
                std::cout << "2) Positive numbers\n";
                std::cout << "3) Negative numbers\n";
                std::cout << "Choice: ";
                int cond_choice = get_int();

                bool (*pred)(const int&) = nullptr;
                if (cond_choice == 1) pred = is_even;
                else if (cond_choice == 2) pred = is_positive;
                else if (cond_choice == 3) pred = is_negative;

                if (!pred) {
                    std::cout << "Invalid condition choice.\n";
                    break;
                }

                try {
                    if (action == 1) { // WHERE
                        sequence<int>* filtered = current->where(pred);
                        print_seq("Filtered version", filtered);
                        delete filtered; 
                    } 
                    else if (action == 2) { // FIND
                        int index = current->find(pred);
                        if (index != -1) {
                            std::cout << "First match found at index: " << index << "\n";
                        } else {
                            std::cout << "No elements match the condition.\n";
                        }
                    }
                } 
                catch (const std::exception& e) {
                    std::cerr << "SEARCH ERROR: " << e.what() << "\n";
                }
                break;
            } // case 3

            case 4: {
                std::cout << "\n--- Complex Operations ---\n";
                std::cout << "1) Slice (Extract & Replace)\n";
                std::cout << "2) Split (Divide by condition)\n";
                std::cout << "3) ZipN (Combine with another sequence)\n";
                std::cout << "0) Back to main menu\n";
                std::cout << "Choose an action: ";
                
                int action = get_int();
                if (action == 0) break;

                try {
                    switch (action) {
                        case 1: { // SLICE
                            std::cout << "Enter start index: ";
                            int index = get_int();
                            std::cout << "How many elements to remove? ";
                            int count = get_int();
                            
                            std::cout << "Enter how many elements to INSERT instead: ";
                            int ins_count = get_int();
                            sequence<int>* ins_seq = current->create_empty();
                            for(int i = 0; i < ins_count; ++i) {
                                std::cout << "  Insert element " << i << ": ";
                                ins_seq = ins_seq->append(get_int());
                            }

                            sequence<int>* result = current->slice(index, count, ins_seq);
                            std::cout << "\n>>> SLICE RESULT (Immutable) <<<\n";
                            print_seq("New Sequence", result);
                            print_seq("Original", current);
                            delete result;
                            
                            delete ins_seq;
                            break;
                        } 

                        case 2: { // SPLIT
                            std::cout << "Select splitting condition:\n";
                            std::cout << "1) By even numbers\n";
                            std::cout << "2) By positive numbers\n";
                            std::cout << "3) By negative numbers\n";
                            std::cout << "Choice: ";
                            
                            int split_choice = get_int();
                            bool (*pred)(const int&) = nullptr;

                            if (split_choice == 1) pred = is_even;
                            else if (split_choice == 2) pred = is_positive;
                            else if (split_choice == 3) pred = is_negative;

                            if (!pred) {
                                std::cout << "Invalid split condition.\n";
                                break;
                            }

                            auto* bundle = split(current, pred);

                            std::cout << "\n>>> SPLIT RESULT <<<\n";
                            IEnumerator<sequence<int>*>* bundle_it = bundle->get_enumerator();
                            int p = 1;
                            
                            while (bundle_it->move_next()) {
                                sequence<int>* part = bundle_it->get_current();
                                std::string label = "  Part " + std::to_string(p++);
                                
                                print_seq(label, part);
                            }

                            delete bundle_it;
                            delete bundle; 
                            break;
                        } // split

                        case 3: { // ZIPN
                            std::cout << "How many additionals sequences do you want to create for zipping? ";
                            int num_seqs = get_int();
                            if (num_seqs < 1) {
                                std::cout << "You need at least one more sequence to zip.\n";
                                break;
                            }

                            auto* pack = new mutable_list_sequence<sequence<int>*>();
                            
                            pack->append(current);

                            for (int i = 0; i < num_seqs; ++i) {
                                std::cout << "\nCreating sequence #" << i + 2 << ":\n";
                                std::cout << "How many elements? ";
                                int count = get_int();
                                
                                sequence<int>* new_s = current->create_empty();
                                for (int j = 0; j < count; ++j) {
                                    std::cout << "  Element " << j << ": ";
                                    new_s = new_s->append(get_int());
                                }
                                pack->append(new_s);
                            }
                            
                            try {
                                auto* zipped = zipN(pack);

                                std::cout << "\n>>> ZIPN RESULT <<<\n";
                                IEnumerator<sequence<int>*>* zip_it = zipped->get_enumerator();
                                int tuple_idx = 0;
                                
                                while (zip_it->move_next()) {
                                    std::string label = "  Tuple " + std::to_string(tuple_idx++);
                                    print_seq(label, zip_it->get_current());
                                }

                                delete zip_it;
                                delete zipped; 
                            } catch (const std::exception& e) {
                                std::cerr << "ZIPN ERROR: " << e.what() << "\n";
                            }

                            IEnumerator<sequence<int>*>* pack_it = pack->get_enumerator();
                            pack_it->move_next();
                            while (pack_it->move_next()) {
                                delete pack_it->get_current();
                            }
                            delete pack_it;
                            delete pack;
                            break;
                        } // zipN
                    } // switch (action)
                } // try
                catch (const std::exception& e) {
                    std::cerr << "COMPLEX OP ERROR: " << e.what() << "\n";
                }
                break;
            } // case 4

            default:
                std::cout << "Invalid category. Try again.\n";
                break;
        } // switch (category)
    } // while

    return 0;
} // main