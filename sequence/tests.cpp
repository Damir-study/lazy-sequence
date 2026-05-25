#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

#include "mutable_array_sequence.h"
#include "mutable_list_sequence.h"
#include "immutable_array_sequence.h"
#include "immutable_list_sequence.h"
#include "sequence_functions.h"

// Тестирование Array Sequence
TEST(mutable_array_sequence_test, empty_state) {
    mutable_array_sequence<int> seq;
    
    EXPECT_EQ(seq.get_length(), 0);
    EXPECT_THROW(seq.get_first(), std::out_of_range);
    EXPECT_THROW(seq.get_last(), std::out_of_range);

    IEnumerator<int>* it = seq.get_enumerator();
    EXPECT_FALSE(it->move_next());
    delete it;
}

TEST(mutable_array_sequence_test, sequence_test) {
    mutable_array_sequence<int> seq;
    
    seq.append(10);  // [10]
    seq.prepend(20); // [20, 10]
    seq.append(30);  // [20, 10, 30]

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_first(), 20);
    EXPECT_EQ(seq.get_last(), 30);

    IEnumerator<int>* it = seq.get_enumerator();
    
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 20);
    
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 10);
    
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 30);
    
    EXPECT_FALSE(it->move_next());
    delete it;
}

// Тестирование List Sequence
TEST(mutable_list_sequence_test, insert_at_mutable) {
    mutable_list_sequence<int> seq;
    
    seq.append(10); // [10]
    seq.insert_at(30, 0); // [30, 10]
    seq.insert_at(20, 1); // [30, 20, 10]

    EXPECT_EQ(seq.get_length(), 3);

    IEnumerator<int>* it = seq.get_enumerator();
    
    it->move_next();
    EXPECT_EQ(it->get_current(), 30);
    
    it->move_next();
    EXPECT_EQ(it->get_current(), 20);
    
    it->move_next();
    EXPECT_EQ(it->get_current(), 10);
    
    EXPECT_FALSE(it->move_next());
    delete it;
}

// Проверка на выживаемость при большом кол-ве элементов
TEST(mutable_list_sequence_test, large_scale_append) {
    mutable_list_sequence<int> seq;
    const int count = 1000;

    
    for (int i = 0; i < count; ++i) {
        seq.append(i);
    }

    EXPECT_EQ(seq.get_length(), count);
    EXPECT_EQ(seq.get_first(), 0);
    EXPECT_EQ(seq.get_last(), count - 1);

    IEnumerator<int>* it = seq.get_enumerator();
    for (int i = 0; i <= 500; ++i) {
        it->move_next();
    }
    EXPECT_EQ(it->get_current(), 500);
    delete it;
}

// Тест на то, что Array и List ведут себя одинаково
TEST(sequence_comparison, different_structures_same_data) {
    mutable_array_sequence<int> seq_arr;
    mutable_list_sequence<int> seq_list;

    for(int i = 0; i < 5; ++i) {
        seq_arr.append(i * 10);
        seq_list.append(i * 10);
    }

    IEnumerator<int>* it_arr = seq_arr.get_enumerator();
    IEnumerator<int>* it_list = seq_list.get_enumerator();

    while(it_arr->move_next() && it_list->move_next()) {
        EXPECT_EQ(it_arr->get_current(), it_list->get_current());
    }
    
    EXPECT_FALSE(it_arr->move_next());
    EXPECT_FALSE(it_list->move_next());

    delete it_arr;
    delete it_list;
}

// append test
TEST(mutable_transformations_test, append) {
    mutable_array_sequence<int> seq;
    
    seq.append(10);
    seq.append(20);
    seq.append(30);
    
    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_first(), 10);
    EXPECT_EQ(seq.get_last(), 30);
    
    IEnumerator<int>* it = seq.get_enumerator();
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 10);
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 20);
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 30);
    EXPECT_FALSE(it->move_next());
    delete it;
}


//prepend test
TEST(mutable_transformations_test, prepend) {
    mutable_list_sequence<int> seq;
    
    seq.prepend(30); // [30]
    seq.prepend(20); // [20, 30]
    seq.prepend(10); // [10, 20, 30]
    
    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_first(), 10);
    EXPECT_EQ(seq.get_last(), 30);
    
    IEnumerator<int>* it = seq.get_enumerator();
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 10);
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 20);
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 30);
    EXPECT_FALSE(it->move_next());
    delete it;
}

// insert at test
TEST(mutable_transformations_test, insert_at) {
    mutable_array_sequence<int> seq;
    
    seq.insert_at(20, 0); // [20]
    seq.insert_at(10, 0); // [10, 20]
    seq.insert_at(40, 2); // [10, 20, 40]
    seq.insert_at(30, 3); // [10, 20, 40, 30]
    
    EXPECT_EQ(seq.get_length(), 4);
    EXPECT_EQ(seq.get_first(), 10);
    EXPECT_EQ(seq.get_last(), 30);
    
    IEnumerator<int>* it = seq.get_enumerator();
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 10);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 20);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 40);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 30);
    EXPECT_FALSE(it->move_next());
    delete it;
}

// concat list + array
TEST(mutable_transformations_test, concat) {
    mutable_array_sequence<int> seq1;
    seq1.append(1);
    seq1.append(2);
    
    mutable_array_sequence<int> seq2;
    seq2.append(3);
    seq2.append(4);
    
    seq1.concat(&seq2);
    
    EXPECT_EQ(seq1.get_length(), 4);
    EXPECT_EQ(seq1.get_first(), 1);
    EXPECT_EQ(seq1.get_last(), 4);
    
    EXPECT_EQ(seq2.get_length(), 2);
    EXPECT_EQ(seq2.get_last(), 4);
    
    IEnumerator<int>* it = seq1.get_enumerator();
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 1);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 2);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 3);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 4);
    EXPECT_FALSE(it->move_next());
    delete it;
}

// concat + empty
TEST(mutable_transformations_test, concat_with_empty_sequence) {
    mutable_array_sequence<int> seq1;
    seq1.append(100);
    
    mutable_list_sequence<int> empty_seq;
    
    seq1.concat(&empty_seq);
    EXPECT_EQ(seq1.get_length(), 1);
    EXPECT_EQ(seq1.get_first(), 100);
    
    empty_seq.concat(&seq1);
    EXPECT_EQ(empty_seq.get_length(), 1);
    EXPECT_EQ(empty_seq.get_first(), 100);
}

// immutable append
TEST(immutable_transformations_test, append_immutable) {
    auto* s1 = new immutable_array_sequence<int>();
    
    sequence<int>* s2 = s1->append(10);
    sequence<int>* s3 = s2->append(20);
    
    EXPECT_EQ(s1->get_length(), 0);
    EXPECT_THROW(s1->get_first(), std::out_of_range);
    
    EXPECT_EQ(s2->get_length(), 1);
    EXPECT_EQ(s2->get_first(), 10);
    EXPECT_EQ(s2->get_last(), 10);
    
    EXPECT_EQ(s3->get_length(), 2);
    EXPECT_EQ(s3->get_first(), 10);
    EXPECT_EQ(s3->get_last(), 20);
    
    delete s1;
    delete s2;
    delete s3;
}

// immutable prepend
TEST(immutable_transformations_test, prepend_immutable) {
    auto* s1 = new immutable_list_sequence<int>();
    auto* s2 = s1->append(20);
    
    sequence<int>* s3 = s2->prepend(10);
    
    EXPECT_EQ(s2->get_length(), 1);
    EXPECT_EQ(s2->get_first(), 20);
    
    EXPECT_EQ(s3->get_length(), 2);
    EXPECT_EQ(s3->get_first(), 10);
    EXPECT_EQ(s3->get_last(), 20);
    
    IEnumerator<int>* it = s3->get_enumerator();
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 10);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 20);
    EXPECT_FALSE(it->move_next());
    delete it;
    
    delete s1; delete s2; delete s3;
}

// immutable insert at
TEST(immutable_transformations_test, insert_at_immutable) {
    auto* s1 = new immutable_array_sequence<int>();
    auto* s2 = s1->append(10);
    auto* s3 = s2->append(30); // [10, 30]
    
    sequence<int>* s4 = s3->insert_at(20, 1);
    
    EXPECT_EQ(s3->get_length(), 2);
    EXPECT_EQ(s3->get_first(), 10);
    EXPECT_EQ(s3->get_last(), 30);
    
    EXPECT_EQ(s4->get_length(), 3);
    EXPECT_EQ(s4->get_first(), 10);
    EXPECT_EQ(s4->get_last(), 30);
    
    IEnumerator<int>* it = s4->get_enumerator();
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 10);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 20);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 30);
    EXPECT_FALSE(it->move_next());
    delete it;
    
    delete s1; delete s2; delete s3; delete s4;
}

// immutable concat
TEST(immutable_transformations_test, concat_immutable) {
    auto* a1 = new immutable_list_sequence<int>();
    auto* a2 = a1->append(1);
    auto* seq_A = a2->append(2);
    
    auto* b1 = new immutable_array_sequence<int>();
    auto* b2 = b1->append(3);
    auto* seq_B = b2->append(4);
    
    sequence<int>* result = seq_A->concat(seq_B);
    
    EXPECT_EQ(seq_A->get_length(), 2);
    EXPECT_EQ(seq_A->get_last(), 2);
    EXPECT_EQ(seq_B->get_length(), 2);
    EXPECT_EQ(seq_B->get_last(), 4);
    
    EXPECT_EQ(result->get_length(), 4);
    EXPECT_EQ(result->get_first(), 1);
    EXPECT_EQ(result->get_last(), 4);
    
    IEnumerator<int>* it = result->get_enumerator();
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 1);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 2);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 3);
    ASSERT_TRUE(it->move_next()); EXPECT_EQ(it->get_current(), 4);
    EXPECT_FALSE(it->move_next());
    delete it;
    
    delete a1; delete a2; delete seq_A;
    delete b1; delete b2; delete seq_B;
    delete result;
}


// Предикаты для тестов
bool is_even(const int& x) { return x % 2 == 0; }
bool is_negative(const int& x) { return x < 0; }
bool is_greater10(const int& x) { return x > 10; }

TEST(where_find_test, where) {
    mutable_array_sequence<int> seq;
    seq.append(1);
    seq.append(2);
    seq.append(3);
    seq.append(4);
    seq.append(5); // [1, 2, 3, 4, 5]

    sequence<int>* filtered = seq.where(is_even); //[2, 4]

    ASSERT_NE(filtered, nullptr);
    EXPECT_EQ(filtered->get_length(), 2);
    
    IEnumerator<int>* it = filtered->get_enumerator();
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 2);
    ASSERT_TRUE(it->move_next());
    EXPECT_EQ(it->get_current(), 4);
    EXPECT_FALSE(it->move_next());

    delete it;
    delete filtered;
}

TEST(where_find_test, where_no_matches) {
    mutable_list_sequence<int> seq;
    seq.append(1);
    seq.append(3);
    seq.append(5);

    sequence<int>* filtered = seq.where(is_even);

    EXPECT_EQ(filtered->get_length(), 0);
    
    IEnumerator<int>* it = filtered->get_enumerator();
    EXPECT_FALSE(it->move_next());

    delete it;
    delete filtered;
}

TEST(where_find_test, find) {
    mutable_array_sequence<int> seq;
    seq.append(5);
    seq.append(15);
    seq.append(7);
    seq.append(20);

    int index = seq.find(is_greater10);
    EXPECT_EQ(index, 1);

    int not_found = seq.find(is_negative);
    EXPECT_EQ(not_found, -1);
}

TEST(where_find_test, find_at_bound) {
    mutable_list_sequence<int> seq;
    seq.append(-5);
    seq.append(10);
    seq.append(20);

    EXPECT_EQ(seq.find(is_negative), 0);

    EXPECT_EQ(seq.find(is_greater10), 2);
}

TEST(where_find_test, where_on_immutable) {
    auto* s1 = new immutable_array_sequence<int>();
    auto* s2 = s1->append(2);
    auto* s3 = s2->append(3);
    auto* s4 = s3->append(4); // [2, 3, 4]

    sequence<int>* filtered = s4->where(is_even); // [2, 4]

    EXPECT_EQ(filtered->get_length(), 2);
    EXPECT_EQ(filtered->get_first(), 2);
    EXPECT_EQ(filtered->get_last(), 4);

    delete s1; delete s2; delete s3; delete s4;
    delete filtered;
}


TEST(complex_transformations_test, get_sub_sequence) {
    mutable_array_sequence<int> seq;
    for(int i = 0; i < 5; ++i) seq.append(i * 10); // [0, 10, 20, 30, 40]

    sequence<int>* sub = seq.get_subsequence(1, 4);

    ASSERT_NE(sub, nullptr);
    EXPECT_EQ(sub->get_length(), 3);
    EXPECT_EQ(sub->get_first(), 10);
    EXPECT_EQ(sub->get_last(), 30);

    IEnumerator<int>* it = sub->get_enumerator();
    it->move_next(); EXPECT_EQ(it->get_current(), 10);
    it->move_next(); EXPECT_EQ(it->get_current(), 20);
    it->move_next(); EXPECT_EQ(it->get_current(), 30);
    
    delete it;
    delete sub;
}

TEST(complex_transformations_test, get_sub_sequence_invalid_indices) {
    mutable_list_sequence<int> seq;
    seq.append(1);
    
    EXPECT_THROW(seq.get_subsequence(2, 1), std::out_of_range);
    EXPECT_THROW(seq.get_subsequence(-1, 0), std::out_of_range);
}

TEST(complex_transformations_test, split_by_even_numbers) {
    mutable_array_sequence<int> seq;
    for (int i = 1; i <= 5; ++i) seq.append(i); // [1, 2, 3, 4, 5]

    sequence<sequence<int>*>* fragments = split(&seq, is_even);

    ASSERT_EQ(fragments->get_length(), 3);

    IEnumerator<sequence<int>*>* it = fragments->get_enumerator();
    
    it->move_next();
    EXPECT_EQ(it->get_current()->get_length(), 1);
    EXPECT_EQ(it->get_current()->get_first(), 1);

    it->move_next();
    EXPECT_EQ(it->get_current()->get_length(), 1);
    EXPECT_EQ(it->get_current()->get_first(), 3);

    it->move_next();
    EXPECT_EQ(it->get_current()->get_length(), 1);
    EXPECT_EQ(it->get_current()->get_first(), 5);

    IEnumerator<sequence<int>*>* cleanup_it = fragments->get_enumerator();
    while (cleanup_it->move_next()) {
        delete cleanup_it->get_current();
    }
    delete cleanup_it;
    delete it;
    delete fragments;
}

TEST(complex_transformations_test, zip_n) {
    mutable_array_sequence<int>* row1 = new mutable_array_sequence<int>();
    row1->append(1); row1->append(3);

    mutable_array_sequence<int>* row2 = new mutable_array_sequence<int>();
    row2->append(2); row2->append(4);

    mutable_list_sequence<sequence<int>*> source; //[[1,3] [2,4]]
    source.append(row1);
    source.append(row2);

    sequence<sequence<int>*>* result = zipN(&source); // [[1,2] [3,4]]

    ASSERT_EQ(result->get_length(), 2);

    IEnumerator<sequence<int>*>* it = result->get_enumerator();
    
    it->move_next();
    EXPECT_EQ(it->get_current()->get_length(), 2);
    EXPECT_EQ(it->get_current()->get_first(), 1);
    EXPECT_EQ(it->get_current()->get_last(), 2);

    it->move_next();
    EXPECT_EQ(it->get_current()->get_length(), 2);
    EXPECT_EQ(it->get_current()->get_first(), 3);
    EXPECT_EQ(it->get_current()->get_last(), 4);

    IEnumerator<sequence<int>*>* cleanup_it = result->get_enumerator();
    while (cleanup_it->move_next()) delete cleanup_it->get_current();
    delete cleanup_it;
    delete it;
    delete result;
    delete row1; delete row2;
}


TEST(complex_transformations_test, unzip_n) {
    mutable_array_sequence<int>* row1 = new mutable_array_sequence<int>();
    row1->append(10); row1->append(20);
    
    mutable_array_sequence<int>* row2 = new mutable_array_sequence<int>();
    row2->append(30); row2->append(40);

    mutable_list_sequence<sequence<int>*> source; // [[10, 20], [30, 40]]
    source.append(row1);
    source.append(row2);

    sequence<sequence<int>*>* result = unzipN(&source); // [[10, 30], [20, 40]]

    ASSERT_EQ(result->get_length(), 2);

    IEnumerator<sequence<int>*>* it = result->get_enumerator();
    
    it->move_next();
    EXPECT_EQ(it->get_current()->get_first(), 10);
    EXPECT_EQ(it->get_current()->get_last(), 30);

    it->move_next();
    EXPECT_EQ(it->get_current()->get_first(), 20);
    EXPECT_EQ(it->get_current()->get_last(), 40);

    IEnumerator<sequence<int>*>* cleanup_it = result->get_enumerator();
    while (cleanup_it->move_next()) delete cleanup_it->get_current();
    delete cleanup_it;
    delete it;
    delete result;
    delete row1; delete row2;
}


TEST(slice_operation_test, mutable_slice) {
    mutable_array_sequence<int> seq;
    for(int i = 1; i <= 5; ++i) seq.append(i * 10); // [10, 20, 30, 40, 50]

    mutable_list_sequence<int> replacement;
    replacement.append(99);

    sequence<int>* result = seq.slice(1, 2, &replacement); // [10, 99, 40, 50]

    EXPECT_EQ(result->get_length(), 4);
    
    IEnumerator<int>* it = result->get_enumerator();
    it->move_next(); EXPECT_EQ(it->get_current(), 10);
    it->move_next(); EXPECT_EQ(it->get_current(), 99);
    it->move_next(); EXPECT_EQ(it->get_current(), 40);
    it->move_next(); EXPECT_EQ(it->get_current(), 50);
    
    delete it;
    delete result;
}

TEST(slice_operation_test, mutable_slice_insertion) {
    mutable_list_sequence<int> seq;
    seq.append(1);
    seq.append(2); // [1, 2]

    mutable_array_sequence<int> to_insert;
    to_insert.append(100);

    
    sequence<int>* result = seq.slice(1, 0, &to_insert); // [1, 100, 2]

    EXPECT_EQ(result->get_length(), 3);
    EXPECT_EQ(result->get_first(), 1);
    
    IEnumerator<int>* it = result->get_enumerator();
    it->move_next();
    it->move_next(); EXPECT_EQ(it->get_current(), 100);
    
    delete it;
    delete result;
}

TEST(slice_operation_test, immutable_slice) {
    auto* s1 = new immutable_array_sequence<int>();
    auto* s2 = s1->append(10);
    auto* s3 = s2->append(20);
    auto* s4 = s3->append(30); // [10, 20, 30]

    mutable_list_sequence<int> to_insert;
    to_insert.append(99);

    sequence<int>* result = s4->slice(1, 1, &to_insert); //[10, 99, 30]

    EXPECT_EQ(s4->get_length(), 3);
    EXPECT_EQ(s4->get_last(), 30);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->get_length(), 3);
    
    IEnumerator<int>* it = result->get_enumerator();
    it->move_next(); EXPECT_EQ(it->get_current(), 10);
    it->move_next(); EXPECT_EQ(it->get_current(), 99);
    it->move_next(); EXPECT_EQ(it->get_current(), 30);

    delete it;
    delete result;
    delete s1; delete s2; delete s3; delete s4;
}

TEST(slice_operation_test, slice_out_of_bounds_throws) {
    mutable_array_sequence<int> seq;
    seq.append(10);
    
    mutable_list_sequence<int> empty_ins;
    
    EXPECT_THROW(seq.slice(5, 1, &empty_ins), std::out_of_range);
    

    EXPECT_THROW(seq.slice(-12, 10, &empty_ins), std::out_of_range);
}

// Проверка работы с разными объектами (std::string)
TEST(micro_cases_test, test_string_sequence) {
    mutable_list_sequence<std::string> seq;
    seq.append("alpha");
    seq.append("beta");
    seq.append("gamma");

    EXPECT_EQ(seq.get_length(), 3);
    EXPECT_EQ(seq.get_first(), "alpha");
    EXPECT_EQ(seq.get_last(), "gamma");
    
    seq.insert_at("delta", 1);
    EXPECT_EQ(seq.get_length(), 4);
}

// Проверка ZIP при пустом входном контейнере
TEST(micro_cases_test, test_zip_n_empty_input) {
    mutable_list_sequence<sequence<int>*> empty_container;
    
    sequence<sequence<int>*>* result = zipN(&empty_container);
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->get_length(), 0);
    
    delete result;
}

// Проверка SPLIT, когда абсолютно все элементы — разделители
TEST(micro_edge_cases_test, test_split_all_elements_are_delimiters) {
    mutable_array_sequence<int> seq;
    seq.append(-1);
    seq.append(-2);
    seq.append(-3);

    auto* fragments = split(&seq, is_negative);

    EXPECT_EQ(fragments->get_length(), 4);
    
    IEnumerator<sequence<int>*>* it = fragments->get_enumerator();
    while(it->move_next()) {
        EXPECT_EQ(it->get_current()->get_length(), 0);
        delete it->get_current();
    }
    
    delete it;
    delete fragments;
}