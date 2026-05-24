#include "generator.hpp"

#include <stdexcept>

template <typename T>
generator<T>::generator()
    : owner(nullptr),
      rule(nullptr) {}

template <typename T>
generator<T>::generator(lazy_sequence<T>* owner, std::function<T(sequence<T>*)> rule)
    : owner(owner),
      rule(rule) {}

template <typename T>
generator<T>::generator(const generator<T>& other)
    : owner(other.owner),
      rule(other.rule) {}

template <typename T>
T generator<T>::get_next() {
    if (!owner || !rule) {
        throw std::invalid_argument("nullptr owner or rule");
    }

    return rule(owner->get_materialized_items());
}

template <typename T>
bool generator<T>::has_next() const {
    return owner && rule;
}

template <typename T>
void generator<T>::set_owner(lazy_sequence<T>* owner) {
    this->owner = owner;
}
