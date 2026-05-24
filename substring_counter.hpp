#ifndef SUBSTRING_COUNTER_HPP
#define SUBSTRING_COUNTER_HPP

template <typename T>
class read_only_stream;

class substring_counter {
public:
    substring_counter(const char* pattern);
    substring_counter(const substring_counter& other);
    substring_counter& operator=(const substring_counter& other);
    ~substring_counter();

    int count(read_only_stream<char>* stream) const;
    int count(read_only_stream<char>* stream, int max_read_count) const;

private:
    char* pattern;
    int* prefix;
    int pattern_length;

    void set_pattern(const char* source, int length);
    void build_prefix();
};

#endif // SUBSTRING_COUNTER_HPP
