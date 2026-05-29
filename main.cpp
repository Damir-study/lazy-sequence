#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "lazy_sequence.hpp"
#include "read_only_stream.hpp"
#include "substring_counter.hpp"
#include "sequence/mutable_array_sequence.h"

const int text_buffer_size = 1024;

lazy_sequence<int>* create_sequence_menu();

int read_int(const char* prompt = nullptr) {
    if (prompt != nullptr) {
        std::cout << prompt;
    }

    int value;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Enter integer: ";
    }
    return value;
}

void read_text(char* buffer, int size, const char* prompt) {
    std::cout << prompt;
    std::cin >> std::ws;
    std::cin.getline(buffer, size);

    if (!std::cin) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        buffer[0] = '\0';
    }
}

int text_length(const char* text) {
    if (text == nullptr) {
        return 0;
    }

    int length = 0;
    while (text[length] != '\0') {
        ++length;
    }
    return length;
}

char first_char(const char* token) {
    if (token == nullptr || token[0] == '\0') {
        throw std::invalid_argument("Empty char token");
    }
    return token[0];
}

void print_separator() {
    std::cout << "\n========================================\n";
}

void print_ordinal(const ordinal& value) {
    if (value.is_finite()) {
        std::cout << value.get_count();
        return;
    }

    int coefficient = value.get_omega_coefficient();
    if (coefficient == 1) {
        std::cout << "w0";
    } else {
        std::cout << "w0*" << coefficient;
    }

    if (value.get_finite_part() > 0) {
        std::cout << "+" << value.get_finite_part();
    }
}

ordinal read_ordinal() {
    std::cout << "Ordinal is w0 * omega_coefficient + finite_part.\n";
    int omega = read_int("omega coefficient: ");
    int finite = read_int("finite part: ");
    return ordinal(omega, finite);
}

int min_int(int left, int right) {
    return left < right ? left : right;
}

int next_natural(sequence<int>* items) {
    return items->get_length();
}

int next_square(sequence<int>* items) {
    int index = items->get_length();
    return index * index;
}

int next_fibonacci(sequence<int>* items) {
    int length = items->get_length();
    if (length == 0) {
        return 0;
    }
    if (length == 1) {
        return 1;
    }

    int previous = 0;
    int last = 0;
    IEnumerator<int>* it = items->get_enumerator();
    while (it->move_next()) {
        previous = last;
        last = it->get_current();
    }
    delete it;

    return previous * last - 2;
}

char next_alternating_char(sequence<char>* items) {
    return items->get_length() % 2 == 0 ? 'a' : 'b';
}

void print_finite_sequence(const char* label, const sequence<int>* seq) {
    std::cout << label << ": [ ";

    if (seq == nullptr) {
        std::cout << "null ]\n";
        return;
    }

    try {
        IEnumerator<int>* it = seq->get_enumerator();
        while (it->move_next()) {
            std::cout << it->get_current() << " ";
        }
        delete it;
    } catch (const std::exception& error) {
        std::cout << "error: " << error.what() << " ";
    }

    std::cout << "]\n";
}

void print_lazy_preview(const char* label, const lazy_sequence<int>* seq, int requested_count) {
    std::cout << label << ": [ ";

    if (seq == nullptr) {
        std::cout << "null ]\n";
        return;
    }

    int count = requested_count;
    try {
        ordinal length = seq->get_ordinal_length();
        if (length.is_finite()) {
            count = min_int(count, length.get_count());
        }

        for (int i = 0; i < count; ++i) {
            std::cout << seq->get(i) << " ";
        }

        if (length.is_infinite() || (length.is_finite() && count < length.get_count())) {
            std::cout << "... ";
        }
    } catch (const std::exception& error) {
        std::cout << "error: " << error.what() << " ";
    }

    std::cout << "]\n";
}

void print_lazy_state(const lazy_sequence<int>* seq) {
    if (seq == nullptr) {
        std::cout << "Current LazySequence: null\n";
        return;
    }

    print_lazy_preview("Current LazySequence preview", seq, 10);
    std::cout << "Ordinal length: ";
    print_ordinal(seq->get_ordinal_length());
    std::cout << "\nIs infinite: " << (seq->is_infinite() ? "true" : "false") << "\n";
    std::cout << "Materialized count: " << seq->get_materialized_count() << "\n";
}

mutable_array_sequence<int>* read_int_sequence(const char* title) {
    std::cout << title << "\n";
    int count = read_int("Element count: ");
    if (count < 0) {
        throw std::out_of_range("IndexOutOfRange");
    }

    mutable_array_sequence<int>* seq = new mutable_array_sequence<int>();
    for (int i = 0; i < count; ++i) {
        std::cout << "item[" << i << "]: ";
        seq->append(read_int());
    }
    return seq;
}

sequence<int>* read_sequence_argument(const char* title) {
    std::cout << title << "\n";
    std::cout << "1) Manual finite sequence\n";
    std::cout << "2) Lazy sequence\n";
    int choice = read_int("Choice: ");

    if (choice == 1) {
        return read_int_sequence("Manual finite source");
    }
    if (choice == 2) {
        return create_sequence_menu();
    }

    throw std::invalid_argument("Unknown sequence source");
}

lazy_sequence<int>* create_sequence_menu() {
    while (true) {
        print_separator();
        std::cout << "Create LazySequence<int>\n";
        std::cout << "1) Empty finite sequence\n";
        std::cout << "2) Manual finite sequence\n";
        std::cout << "3) Infinite natural numbers: 0, 1, 2, ...\n";
        std::cout << "4) Finite squares by rule: 0, 1, 4, 9, ...\n";
        std::cout << "5) Fibonacci by rule\n";
        std::cout << "6) Arithmetic progression by rule\n";
        std::cout << "Choice: ";

        int choice = read_int();

        try {
            if (choice == 1) {
                return new lazy_sequence<int>();
            }

            if (choice == 2) {
                mutable_array_sequence<int>* seq = read_int_sequence("Manual finite source");
                lazy_sequence<int>* result = new lazy_sequence<int>(seq);
                delete seq;
                return result;
            }

            if (choice == 3) {
                return new lazy_sequence<int>(next_natural, nullptr);
            }

            if (choice == 4) {
                int count = read_int("Length: ");
                return new lazy_sequence<int>(next_square, nullptr, ordinal::finite(count));
            }

            if (choice == 5) {
                int start_items[] = {0, 1};
                mutable_array_sequence<int> start(start_items, 2);
                std::cout << "1) Infinite\n";
                std::cout << "2) Finite\n";
                int length_choice = read_int("Choice: ");
                if (length_choice == 1) {
                    return new lazy_sequence<int>(next_fibonacci, &start);
                }

                int count = read_int("Length: ");
                return new lazy_sequence<int>(next_fibonacci, &start, ordinal::finite(count));
            }

            if (choice == 6) {
                int first = read_int("First item: ");
                int step = read_int("Step: ");
                int start_items[] = {first};
                mutable_array_sequence<int> start(start_items, 1);

                std::cout << "1) Infinite\n";
                std::cout << "2) Finite\n";
                int length_choice = read_int("Choice: ");

                if (length_choice == 1) {
                    return new lazy_sequence<int>([step](sequence<int>* items) {return items->get_last() + step;}, &start);
                }

                int count = read_int("Length: ");
                return new lazy_sequence<int>([step](sequence<int>* items) {return items->get_last() + step;},&start,ordinal::finite(count));
            }

            std::cout << "Unknown choice.\n";
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
}

void replace_current(lazy_sequence<int>*& current, sequence<int>* result) {
    if (result == nullptr) {
        return;
    }

    lazy_sequence<int>* lazy_result = dynamic_cast<lazy_sequence<int>*>(result);
    if (lazy_result == nullptr) {
        lazy_result = new lazy_sequence<int>(result);
        delete result;
    }

    delete current;
    current = lazy_result;
}

void inspect_menu(lazy_sequence<int>* current) {
    while (true) {
        print_separator();
        print_lazy_state(current);
        std::cout << "\nDecomposition / constructors\n";
        std::cout << "1) get_first\n";
        std::cout << "2) get_last\n";
        std::cout << "3) get(int index)\n";
        std::cout << "4) get(ordinal index)\n";
        std::cout << "5) get_subsequence(start, end exclusive)\n";
        std::cout << "6) get_length\n";
        std::cout << "7) get_ordinal_length\n";
        std::cout << "8) is_infinite\n";
        std::cout << "9) get_materialized_count\n";
        std::cout << "10) get_materialized_items\n";
        std::cout << "0) Back\n";
        std::cout << "Choice: ";

        int choice = read_int();
        if (choice == 0) {
            return;
        }

        try {
            if (choice == 1) {
                std::cout << "First item: " << current->get_first() << "\n";
            } else if (choice == 2) {
                std::cout << "Last item: " << current->get_last() << "\n";
            } else if (choice == 3) {
                int index = read_int("Index: ");
                std::cout << "Value: " << current->get(index) << "\n";
            } else if (choice == 4) {
                ordinal index = read_ordinal();
                std::cout << "Value: " << current->get(index) << "\n";
            } else if (choice == 5) {
                int start = read_int("Start index: ");
                int end = read_int("End index exclusive: ");
                sequence<int>* sub = current->get_subsequence(start, end);
                lazy_sequence<int>* lazy_sub = dynamic_cast<lazy_sequence<int>*>(sub);
                if (lazy_sub != nullptr) {
                    print_lazy_preview("Subsequence", lazy_sub, 15);
                } else {
                    print_finite_sequence("Subsequence", sub);
                }
                delete sub;
            } else if (choice == 6) {
                std::cout << "Length: " << current->get_length() << "\n";
            } else if (choice == 7) {
                std::cout << "Ordinal length: ";
                print_ordinal(current->get_ordinal_length());
                std::cout << "\n";
            } else if (choice == 8) {
                std::cout << "Is infinite: " << (current->is_infinite() ? "true" : "false") << "\n";
            } else if (choice == 9) {
                std::cout << "Materialized count: " << current->get_materialized_count() << "\n";
            } else if (choice == 10) {
                print_finite_sequence("Materialized items", current->get_materialized_items());
            } else {
                std::cout << "Unknown choice.\n";
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
}

void operations_menu(lazy_sequence<int>*& current) {
    while (true) {
        print_separator();
        print_lazy_state(current);
        std::cout << "\nOperations\n";
        std::cout << "1) append(item)\n";
        std::cout << "2) prepend(item)\n";
        std::cout << "3) insert_at(item, int index)\n";
        std::cout << "4) insert_at(sequence, int index)\n";
        std::cout << "5) insert_at(item, ordinal index)\n";
        std::cout << "6) insert_at(sequence, ordinal index)\n";
        std::cout << "7) concat(finite sequence)\n";
        std::cout << "8) concat(lazy sequence)\n";
        std::cout << "9) remove_at(int index)\n";
        std::cout << "10) remove_at(ordinal index)\n";
        std::cout << "11) remove_range(int index, int count)\n";
        std::cout << "12) remove_range(ordinal index, int count)\n";
        std::cout << "0) Back\n";
        std::cout << "Choice: ";

        int choice = read_int();
        if (choice == 0) {
            return;
        }

        try {
            sequence<int>* result = nullptr;

            if (choice == 1) {
                int item = read_int("Item: ");
                result = current->append(item);
            } else if (choice == 2) {
                int item = read_int("Item: ");
                result = current->prepend(item);
            } else if (choice == 3) {
                int item = read_int("Item: ");
                int index = read_int("Index: ");
                result = current->insert_at(item, index);
            } else if (choice == 4) {
                sequence<int>* seq = read_sequence_argument("Sequence to insert");
                int index = read_int("Index: ");
                result = current->insert_at(seq, index);
                delete seq;
            } else if (choice == 5) {
                int item = read_int("Item: ");
                ordinal index = read_ordinal();
                result = current->insert_at(item, index);
            } else if (choice == 6) {
                sequence<int>* seq = read_sequence_argument("Sequence to insert");
                ordinal index = read_ordinal();
                result = current->insert_at(seq, index);
                delete seq;
            } else if (choice == 7) {
                mutable_array_sequence<int>* seq = read_int_sequence("Finite sequence to concat");
                result = current->concat(seq);
                delete seq;
            } else if (choice == 8) {
                lazy_sequence<int>* seq = create_sequence_menu();
                result = current->concat(seq);
                delete seq;
            } else if (choice == 9) {
                int index = read_int("Index: ");
                result = current->remove_at(index);
            } else if (choice == 10) {
                ordinal index = read_ordinal();
                result = current->remove_at(index);
            } else if (choice == 11) {
                int index = read_int("Index: ");
                int count = read_int("Count: ");
                result = current->remove_range(index, count);
            } else if (choice == 12) {
                ordinal index = read_ordinal();
                int count = read_int("Count: ");
                result = current->remove_range(index, count);
            } else {
                std::cout << "Unknown choice.\n";
            }

            if (result != nullptr) {
                replace_current(current, result);
                std::cout << "Operation applied. Current sequence was replaced by new lazy sequence.\n";
                print_lazy_preview("Result preview", current, 15);
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
}

void substring_counter_menu() {
    while (true) {
        print_separator();
        std::cout << "Substring counter (KMP, one pass)\n";
        std::cout << "1) Count in char* text\n";
        std::cout << "2) Count in sequence<char>\n";
        std::cout << "3) Count in infinite lazy char sequence with limit\n";
        std::cout << "0) Back\n";
        std::cout << "Choice: ";

        int choice = read_int();
        if (choice == 0) {
            return;
        }

        char pattern[text_buffer_size];
        read_text(pattern, text_buffer_size, "Pattern: ");

        try {
            substring_counter counter(pattern);

            if (choice == 1) {
                char text[text_buffer_size];
                read_text(text, text_buffer_size, "Text: ");

                read_only_stream<char> stream(text, first_char, true);
                stream.open();
                int result = counter.count(&stream);
                stream.close();

                std::cout << "Occurrences: " << result << "\n";
            } else if (choice == 2) {
                char text[text_buffer_size];
                read_text(text, text_buffer_size, "Text for sequence<char>: ");

                mutable_array_sequence<char> chars(text, text_length(text));
                read_only_stream<char> stream(&chars);
                stream.open();
                int result = counter.count(&stream);
                stream.close();

                std::cout << "Occurrences: " << result << "\n";
            } else if (choice == 3) {
                char start_items[] = {'a'};
                mutable_array_sequence<char> start(start_items, 1);
                lazy_sequence<char> lazy_chars(next_alternating_char, &start);
                read_only_stream<char> stream(&lazy_chars);
                int max_count = read_int("How many chars to read: ");

                stream.open();
                int result = counter.count(&stream, max_count);
                stream.close();

                std::cout << "Occurrences: " << result << "\n";
            } else {
                std::cout << "Unknown choice.\n";
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
}

int main() {
    lazy_sequence<int>* current = nullptr;

    try {
        current = create_sequence_menu();
    } catch (const std::exception& error) {
        std::cout << "Cannot create initial sequence: " << error.what() << "\n";
        return 1;
    }

    while (true) {
        print_separator();
        print_lazy_state(current);
        std::cout << "1) Recreate current LazySequence\n";
        std::cout << "2) Test LazySequence decomposition / constructors\n";
        std::cout << "3) Test LazySequence operations\n";
        std::cout << "4) Test substring_counter\n";
        std::cout << "0) Exit\n";
        std::cout << "Choice: ";

        int choice = read_int();

        if (choice == 0) {
            delete current;
            return 0;
        }

        try {
            switch (choice) {
            case 1: {
                lazy_sequence<int>* next = create_sequence_menu();
                delete current;
                current = next;
                break;
            }
            case 2:
                inspect_menu(current);
                break;
            case 3:
                operations_menu(current);
                break;
            case 4:
                substring_counter_menu();
                break;
            default:
                std::cout << "Unknown choice.\n";
                break;
            }
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << "\n";
        }
    }
}
