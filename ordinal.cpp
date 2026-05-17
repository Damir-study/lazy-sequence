#include "ordinal.hpp"

#include <stdexcept>

ordinal::ordinal()
    : omega_coefficient(0), finite_part(0) {}

ordinal::ordinal(std::size_t finite_part)
    : omega_coefficient(0), finite_part(finite_part) {}

ordinal::ordinal(std::size_t omega_coefficient, std::size_t finite_part)
    : omega_coefficient(omega_coefficient), finite_part(finite_part) {}

ordinal ordinal::finite(std::size_t value) {
    return ordinal(0, value);
}

ordinal ordinal::omega() {
    return ordinal(1, 0);
}

ordinal ordinal::omega_times(std::size_t coefficient) {
    return ordinal(coefficient, 0);
}

ordinal ordinal::omega_plus(std::size_t finite_part) {
    return ordinal(1, finite_part);
}

bool ordinal::is_zero() const {
    return omega_coefficient == 0 && finite_part == 0;
}

bool ordinal::is_finite() const {
    return omega_coefficient == 0;
}

bool ordinal::is_infinite() const {
    return omega_coefficient != 0;
}

std::size_t ordinal::get_omega_coefficient() const {
    return omega_coefficient;
}

std::size_t ordinal::get_finite_part() const {
    return finite_part;
}

std::size_t ordinal::get_count() const {
    if (is_infinite()) {
        throw std::logic_error("Ordinal is infinite");
    }
    return finite_part;
}

ordinal ordinal::operator+(const ordinal& other) const {
    if (other.omega_coefficient == 0) {
        return ordinal(omega_coefficient, finite_part + other.finite_part);
    }
    return ordinal(omega_coefficient + other.omega_coefficient, other.finite_part);
}

ordinal ordinal::operator+(std::size_t value) const {
    return ordinal(omega_coefficient, finite_part + value);
}

ordinal ordinal::operator-(const ordinal& other) const {
    if (*this < other) {
        throw std::out_of_range("Ordinal subtraction is negative");
    }

    if (other.omega_coefficient == 0) {
        return *this - other.finite_part;
    }

    if (omega_coefficient == other.omega_coefficient) {
        return ordinal(0, finite_part - other.finite_part);
    }

    return ordinal(omega_coefficient - other.omega_coefficient, finite_part);
}

ordinal ordinal::operator-(std::size_t value) const {
    if (finite_part >= value) {
        return ordinal(omega_coefficient, finite_part - value);
    }

    return *this;
}

ordinal ordinal::operator*(std::size_t value) const {
    return ordinal(omega_coefficient * value, finite_part);
}

ordinal& ordinal::operator+=(const ordinal& other) {
    *this = *this + other;
    return *this;
}

ordinal& ordinal::operator+=(std::size_t value) {
    *this = *this + value;
    return *this;
}

ordinal& ordinal::operator-=(const ordinal& other) {
    *this = *this - other;
    return *this;
}

ordinal& ordinal::operator-=(std::size_t value) {
    *this = *this - value;
    return *this;
}

ordinal& ordinal::operator*=(std::size_t value) {
    *this = *this * value;
    return *this;
}

bool ordinal::operator==(const ordinal& other) const {
    return omega_coefficient == other.omega_coefficient &&
           finite_part == other.finite_part;
}

bool ordinal::operator!=(const ordinal& other) const {
    return !(*this == other);
}

bool ordinal::operator<(const ordinal& other) const {
    if (omega_coefficient != other.omega_coefficient) {
        return omega_coefficient < other.omega_coefficient;
    }
    return finite_part < other.finite_part;
}

bool ordinal::operator<=(const ordinal& other) const {
    return *this < other || *this == other;
}

bool ordinal::operator>(const ordinal& other) const {
    return other < *this;
}

bool ordinal::operator>=(const ordinal& other) const {
    return other <= *this;
}