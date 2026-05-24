#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <functional>

#include "sequence/sequence.h"

template <typename T>
class lazy_sequence;

template <typename T>
class generator {
public:
    generator();
    generator(lazy_sequence<T>* owner, std::function<T(sequence<T>*)> rule);
    generator(const generator<T>& other);

    T get_next();
    bool has_next() const;

    void set_owner(lazy_sequence<T>* owner);

private:
    lazy_sequence<T>* owner;
    std::function<T(sequence<T>*)> rule;
};

#include "generator.tpp"

#endif // GENERATOR_HPP
