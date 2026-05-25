#ifndef IENUMERATOR_H
#define IENUMERATOR_H


template <typename T>
class IEnumerator {
public:
    virtual bool move_next() = 0;
    virtual const T& get_current() const = 0;
    virtual ~IEnumerator() {}
};

template <typename T>
class IEnumerable {
public:
    virtual IEnumerator<T>* get_enumerator() const = 0;
    virtual ~IEnumerable() {}
};

#endif //IENUMERATOR_H