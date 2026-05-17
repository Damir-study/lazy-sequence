#ifndef ORDINAL_HPP
#define ORDINAL_HPP

#include <cstddef>

class ordinal {
public:
    ordinal();
    ordinal(std::size_t finite_part);
    ordinal(std::size_t omega_coefficient, std::size_t finite_part);

    static ordinal finite(std::size_t value);
    static ordinal omega();
    static ordinal omega_times(std::size_t coefficient);
    static ordinal omega_plus(std::size_t finite_part);

    bool is_zero() const;
    bool is_finite() const;
    bool is_infinite() const;

    std::size_t get_omega_coefficient() const;
    std::size_t get_finite_part() const;
    std::size_t get_count() const;

    ordinal operator+(const ordinal& other) const;
    ordinal operator+(std::size_t value) const;
    ordinal operator-(const ordinal& other) const;
    ordinal operator-(std::size_t value) const;
    ordinal operator*(std::size_t value) const;

    ordinal& operator+=(const ordinal& other);
    ordinal& operator+=(std::size_t value);
    ordinal& operator-=(const ordinal& other);
    ordinal& operator-=(std::size_t value);
    ordinal& operator*=(std::size_t value);

    bool operator==(const ordinal& other) const;
    bool operator!=(const ordinal& other) const;
    bool operator<(const ordinal& other) const;
    bool operator<=(const ordinal& other) const;
    bool operator>(const ordinal& other) const;
    bool operator>=(const ordinal& other) const;

private:
    std::size_t omega_coefficient;
    std::size_t finite_part;
};


#endif // ORDINAL_HPP
