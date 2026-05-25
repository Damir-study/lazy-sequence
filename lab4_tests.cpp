#include <gtest/gtest.h>

#include <stdexcept>

#include "lazy_sequence.hpp"
#include "ordinal.hpp"
#include "read_only_stream.hpp"
#include "substring_counter.hpp"
#include "sequence/mutable_array_sequence.h"

int lazy_next_natural(sequence<int>* items) {
    return items->get_length();
}

int lazy_next_square(sequence<int>* items) {
    int index = items->get_length();
    return index * index;
}

int lazy_next_fibonacci(sequence<int>* items) {
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

    return previous + last;
}

char first_char_token(const char* token) {
    if (token == nullptr || token[0] == '\0') {
        throw std::invalid_argument("Empty token");
    }
    return token[0];
}

int int_token(const char* token) {
    int result = 0;
    int sign = 1;
    int index = 0;

    if (token[0] == '-') {
        sign = -1;
        index = 1;
    }

    while (token[index] != '\0') {
        result = result * 10 + token[index] - '0';
        ++index;
    }

    return result * sign;
}

TEST(ordinal_test, finite_and_omega_creation) {
    ordinal finite = ordinal::finite(7);
    ordinal omega = ordinal::omega();
    ordinal omega_plus = ordinal::omega_plus(3);

    EXPECT_TRUE(finite.is_finite());
    EXPECT_FALSE(finite.is_infinite());
    EXPECT_EQ(finite.get_count(), 7);

    EXPECT_TRUE(omega.is_infinite());
    EXPECT_EQ(omega.get_omega_coefficient(), 1);
    EXPECT_EQ(omega.get_finite_part(), 0);

    EXPECT_TRUE(omega_plus.is_infinite());
    EXPECT_EQ(omega_plus.get_omega_coefficient(), 1);
    EXPECT_EQ(omega_plus.get_finite_part(), 3);
}

TEST(ordinal_test, ordinal_arithmetic) {
    ordinal left = ordinal::omega_plus(3);
    ordinal finite = ordinal::finite(5);
    ordinal right = ordinal::omega_plus(2);

    ordinal left_plus_finite = left + finite;
    EXPECT_EQ(left_plus_finite.get_omega_coefficient(), 1);
    EXPECT_EQ(left_plus_finite.get_finite_part(), 8);

    ordinal left_plus_right = left + right;
    EXPECT_EQ(left_plus_right.get_omega_coefficient(), 2);
    EXPECT_EQ(left_plus_right.get_finite_part(), 2);

    ordinal difference = ordinal::omega_plus(5) - ordinal::omega_plus(2);
    EXPECT_TRUE(difference.is_finite());
    EXPECT_EQ(difference.get_count(), 3);
}

TEST(lazy_sequence_test, finite_array_constructor_and_getters) {
    int items[] = {10, 20, 30};
    lazy_sequence<int> seq(items, 3);

    EXPECT_FALSE(seq.is_infinite());
    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_ordinal_length(), ordinal::finite(3));
    EXPECT_EQ(seq.get_first(), 10);
    EXPECT_EQ(seq.get_last(), 30);
    EXPECT_EQ(seq.get(1), 20);
    EXPECT_EQ(seq.get_materialized_count(), 3);
}

TEST(lazy_sequence_test, finite_rule_materializes_on_demand) {
    lazy_sequence<int> squares(lazy_next_square, nullptr, ordinal::finite(5));

    EXPECT_EQ(squares.get_materialized_count(), 0);
    EXPECT_EQ(squares.get(0), 0);
    EXPECT_EQ(squares.get(3), 9);
    EXPECT_EQ(squares.get_materialized_count(), 4);
    EXPECT_EQ(squares.get_last(), 16);
    EXPECT_EQ(squares.get_materialized_count(), 5);
}

TEST(lazy_sequence_test, infinite_rule_has_omega_length) {
    lazy_sequence<int> naturals(lazy_next_natural, nullptr);

    EXPECT_TRUE(naturals.is_infinite());
    EXPECT_EQ(naturals.get_ordinal_length(), ordinal::omega());
    EXPECT_THROW(naturals.get_length(), std::logic_error);

    EXPECT_EQ(naturals.get(0), 0);
    EXPECT_EQ(naturals.get(5), 5);
    EXPECT_EQ(naturals.get_materialized_count(), 6);
}

TEST(lazy_sequence_test, fibonacci_rule_uses_previous_items) {
    int start_items[] = {0, 1};
    mutable_array_sequence<int> start(start_items, 2);
    lazy_sequence<int> fibonacci(lazy_next_fibonacci, &start, ordinal::finite(7));

    EXPECT_EQ(fibonacci.get(0), 0);
    EXPECT_EQ(fibonacci.get(1), 1);
    EXPECT_EQ(fibonacci.get(2), 1);
    EXPECT_EQ(fibonacci.get(3), 2);
    EXPECT_EQ(fibonacci.get(4), 3);
    EXPECT_EQ(fibonacci.get(5), 5);
    EXPECT_EQ(fibonacci.get(6), 8);
}

TEST(lazy_sequence_test, get_subsequence_is_lazy_slice) {
    lazy_sequence<int> naturals(lazy_next_natural, nullptr);

    sequence<int>* sub = naturals.get_subsequence(3, 7);
    lazy_sequence<int>* lazy_sub = dynamic_cast<lazy_sequence<int>*>(sub);

    ASSERT_NE(lazy_sub, nullptr);
    EXPECT_FALSE(lazy_sub->is_infinite());
    EXPECT_EQ(lazy_sub->get_length(), 4);
    EXPECT_EQ(lazy_sub->get(0), 3);
    EXPECT_EQ(lazy_sub->get(3), 6);

    delete sub;
}

TEST(lazy_sequence_test, copy_and_assignment_keep_rule_owner_correct) {
    lazy_sequence<int> naturals(lazy_next_natural, nullptr);

    lazy_sequence<int> copied(naturals);
    lazy_sequence<int> assigned;
    assigned = naturals;

    EXPECT_EQ(copied.get(4), 4);
    EXPECT_EQ(assigned.get(6), 6);
    EXPECT_EQ(naturals.get_materialized_count(), 0);
    EXPECT_EQ(copied.get_materialized_count(), 5);
    EXPECT_EQ(assigned.get_materialized_count(), 7);
}

TEST(lazy_sequence_operations_test, append_prepend_insert_remove_on_finite_sequence) {
    int items[] = {1, 2, 3};
    lazy_sequence<int> seq(items, 3);

    sequence<int>* changed = seq.append(4);
    lazy_sequence<int>* lazy_changed = dynamic_cast<lazy_sequence<int>*>(changed);
    ASSERT_NE(lazy_changed, nullptr);
    EXPECT_EQ(lazy_changed->get_length(), 4);
    EXPECT_EQ(lazy_changed->get(3), 4);
    delete changed;

    changed = seq.prepend(0);
    lazy_changed = dynamic_cast<lazy_sequence<int>*>(changed);
    ASSERT_NE(lazy_changed, nullptr);
    EXPECT_EQ(lazy_changed->get(0), 0);
    EXPECT_EQ(lazy_changed->get(1), 1);
    delete changed;

    changed = seq.insert_at(99, 1);
    lazy_changed = dynamic_cast<lazy_sequence<int>*>(changed);
    ASSERT_NE(lazy_changed, nullptr);
    EXPECT_EQ(lazy_changed->get_length(), 4);
    EXPECT_EQ(lazy_changed->get(0), 1);
    EXPECT_EQ(lazy_changed->get(1), 99);
    EXPECT_EQ(lazy_changed->get(2), 2);
    delete changed;

    changed = seq.remove_range(1, 2);
    lazy_changed = dynamic_cast<lazy_sequence<int>*>(changed);
    ASSERT_NE(lazy_changed, nullptr);
    EXPECT_EQ(lazy_changed->get_length(), 1);
    EXPECT_EQ(lazy_changed->get(0), 1);
    delete changed;
}

TEST(lazy_sequence_operations_test, insert_sequence_by_ordinal_index) {
    int items[] = {1, 2, 3};
    int insert_items[] = {7, 8};
    lazy_sequence<int> seq(items, 3);
    mutable_array_sequence<int> inserted(insert_items, 2);

    sequence<int>* changed = seq.insert_at(&inserted, ordinal::finite(2));
    lazy_sequence<int>* lazy_changed = dynamic_cast<lazy_sequence<int>*>(changed);

    ASSERT_NE(lazy_changed, nullptr);
    EXPECT_EQ(lazy_changed->get_length(), 5);
    EXPECT_EQ(lazy_changed->get(0), 1);
    EXPECT_EQ(lazy_changed->get(1), 2);
    EXPECT_EQ(lazy_changed->get(2), 7);
    EXPECT_EQ(lazy_changed->get(3), 8);
    EXPECT_EQ(lazy_changed->get(4), 3);

    delete changed;
}

TEST(lazy_sequence_operations_test, concat_infinite_sequence_with_finite_tail) {
    int tail_items[] = {10, 20, 30};
    lazy_sequence<int> naturals(lazy_next_natural, nullptr);
    lazy_sequence<int> tail(tail_items, 3);

    sequence<int>* joined = naturals.concat(&tail);
    lazy_sequence<int>* lazy_joined = dynamic_cast<lazy_sequence<int>*>(joined);

    ASSERT_NE(lazy_joined, nullptr);
    EXPECT_EQ(lazy_joined->get_ordinal_length(), ordinal::omega_plus(3));
    EXPECT_EQ(lazy_joined->get(0), 0);
    EXPECT_EQ(lazy_joined->get(5), 5);
    EXPECT_EQ(lazy_joined->get(ordinal::omega()), 10);
    EXPECT_EQ(lazy_joined->get(ordinal::omega_plus(1)), 20);
    EXPECT_EQ(lazy_joined->get(ordinal::omega_plus(2)), 30);

    delete joined;
}

TEST(lazy_sequence_operations_test, concat_two_infinite_sequences) {
    lazy_sequence<int> left(lazy_next_natural, nullptr);
    lazy_sequence<int> right(lazy_next_square, nullptr);

    sequence<int>* joined = left.concat(&right);
    lazy_sequence<int>* lazy_joined = dynamic_cast<lazy_sequence<int>*>(joined);

    ASSERT_NE(lazy_joined, nullptr);
    EXPECT_EQ(lazy_joined->get_ordinal_length(), ordinal::omega_times(2));
    EXPECT_EQ(lazy_joined->get(0), 0);
    EXPECT_EQ(lazy_joined->get(4), 4);
    EXPECT_EQ(lazy_joined->get(ordinal::omega()), 0);
    EXPECT_EQ(lazy_joined->get(ordinal::omega_plus(3)), 9);

    delete joined;
}

TEST(lazy_sequence_operations_test, insert_infinite_sequence_inside_finite_sequence) {
    int items[] = {100, 200, 300};
    lazy_sequence<int> finite(items, 3);
    lazy_sequence<int> naturals(lazy_next_natural, nullptr);

    sequence<int>* changed = finite.insert_at(&naturals, 1);
    lazy_sequence<int>* lazy_changed = dynamic_cast<lazy_sequence<int>*>(changed);

    ASSERT_NE(lazy_changed, nullptr);
    EXPECT_EQ(lazy_changed->get_ordinal_length(), ordinal::omega_plus(2));
    EXPECT_EQ(lazy_changed->get(0), 100);
    EXPECT_EQ(lazy_changed->get(1), 0);
    EXPECT_EQ(lazy_changed->get(2), 1);
    EXPECT_EQ(lazy_changed->get(ordinal::omega()), 200);
    EXPECT_EQ(lazy_changed->get(ordinal::omega_plus(1)), 300);

    delete changed;
}

TEST(lazy_sequence_operations_test, remove_ordinal_range_from_infinite_with_tail) {
    int tail_items[] = {10, 20, 30};
    lazy_sequence<int> naturals(lazy_next_natural, nullptr);
    lazy_sequence<int> tail(tail_items, 3);

    sequence<int>* joined = naturals.concat(&tail);
    lazy_sequence<int>* lazy_joined = dynamic_cast<lazy_sequence<int>*>(joined);
    ASSERT_NE(lazy_joined, nullptr);

    sequence<int>* removed = lazy_joined->remove_range(ordinal::omega(), 2);
    lazy_sequence<int>* lazy_removed = dynamic_cast<lazy_sequence<int>*>(removed);

    ASSERT_NE(lazy_removed, nullptr);
    EXPECT_EQ(lazy_removed->get_ordinal_length(), ordinal::omega_plus(1));
    EXPECT_EQ(lazy_removed->get(ordinal::omega()), 30);

    delete removed;
    delete joined;
}

TEST(read_only_stream_test, text_stream_reads_chars_one_by_one) {
    read_only_stream<char> stream("ab cd", first_char_token);

    stream.open();
    EXPECT_EQ(stream.read(), 'a');
    EXPECT_EQ(stream.read(), 'b');
    EXPECT_EQ(stream.read(), ' ');
    EXPECT_EQ(stream.read(), 'c');
    EXPECT_EQ(stream.read(), 'd');
    EXPECT_TRUE(stream.is_end_of_stream());
    stream.close();
}

TEST(read_only_stream_test, text_stream_reads_int_tokens) {
    read_only_stream<int> stream("10 -20 30", int_token);

    stream.open();
    EXPECT_EQ(stream.read(), 10);
    EXPECT_EQ(stream.read(), -20);
    EXPECT_EQ(stream.seek(1), 1);
    EXPECT_EQ(stream.read(), -20);
    EXPECT_EQ(stream.read(), 30);
    EXPECT_TRUE(stream.is_end_of_stream());
    stream.close();
}

TEST(read_only_stream_test, copy_constructor_preserves_seek_position_for_text_stream) {
    read_only_stream<int> stream("5 10 15", int_token);

    stream.open();
    EXPECT_EQ(stream.read(), 5);

    read_only_stream<int> copied(stream);
    EXPECT_EQ(copied.get_position(), 1);
    EXPECT_EQ(copied.read(), 10);
    EXPECT_EQ(copied.read(), 15);

    stream.close();
}

TEST(substring_counter_test, counts_overlapping_occurrences_in_text_stream) {
    read_only_stream<char> stream("ababa", first_char_token);
    substring_counter counter("aba");

    stream.open();
    EXPECT_EQ(counter.count(&stream), 2);
    stream.close();
}

TEST(substring_counter_test, counts_occurrences_in_sequence_stream) {
    char text[] = {'a', 'a', 'a', 'a'};
    mutable_array_sequence<char> seq(text, 4);
    read_only_stream<char> stream(&seq);
    substring_counter counter("aa");

    stream.open();
    EXPECT_EQ(counter.count(&stream), 3);
    stream.close();
}

TEST(substring_counter_test, count_with_limit_supports_infinite_streams) {
    char start_items[] = {'a'};
    mutable_array_sequence<char> start(start_items, 1);
    lazy_sequence<char> lazy_chars(
        [](sequence<char>* items) {
            return items->get_length() % 2 == 0 ? 'a' : 'b';
        },
        &start
    );
    read_only_stream<char> stream(&lazy_chars);
    substring_counter counter("aba");

    stream.open();
    EXPECT_EQ(counter.count(&stream, 5), 2);
    stream.close();
}

TEST(substring_counter_test, handles_no_matches_and_invalid_arguments) {
    read_only_stream<char> stream("abcdef", first_char_token);
    substring_counter counter("zzz");

    stream.open();
    EXPECT_EQ(counter.count(&stream), 0);
    stream.close();

    EXPECT_THROW(substring_counter(""), std::invalid_argument);
    EXPECT_THROW(counter.count(nullptr), std::invalid_argument);
    EXPECT_THROW(counter.count(&stream, -2), std::out_of_range);
}

TEST(substring_counter_test, complex_pattern_uses_kmp_fallbacks) {
    read_only_stream<char> stream("abababacababaca", first_char_token);
    substring_counter counter("ababaca");

    stream.open();
    EXPECT_EQ(counter.count(&stream), 2);
    stream.close();
}
