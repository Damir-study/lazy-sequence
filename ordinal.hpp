#ifndef ORDINAL_HPP
#define ORDINAL_HPP

class ordinal {
public:
    ordinal();
    ordinal(int finite_part);
    ordinal(int omega_coefficient, int finite_part);

    static ordinal finite(int value);
    static ordinal omega();
    static ordinal omega_times(int coefficient);
    static ordinal omega_plus(int finite_part);

    bool is_zero() const;
    bool is_finite() const;
    bool is_infinite() const;

    int get_omega_coefficient() const;
    int get_finite_part() const;
    int get_count() const;

    ordinal operator+(const ordinal& other) const;
    ordinal operator+(int value) const;
    ordinal operator-(const ordinal& other) const;
    ordinal operator-(int value) const;
    ordinal operator*(int value) const;

    ordinal& operator+=(const ordinal& other);
    ordinal& operator+=(int value);
    ordinal& operator-=(const ordinal& other);
    ordinal& operator-=(int value);
    ordinal& operator*=(int value);

    bool operator==(const ordinal& other) const;
    bool operator!=(const ordinal& other) const;
    bool operator<(const ordinal& other) const;
    bool operator<=(const ordinal& other) const;
    bool operator>(const ordinal& other) const;
    bool operator>=(const ordinal& other) const;

private:
    int omega_coefficient;
    int finite_part;
};


#endif // ORDINAL_HPP
