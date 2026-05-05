#ifndef _MVANG_H
#define _MVANG_H

#include <cstdint>

template <typename T, uint8_t sz>
class MvAverage
{
public:
    using value_type = T;
    using size_type = uint8_t;

    constexpr size_type size() const
    {
        return sz;
    }

    value_type get_average() const
    {
        // safe, if initialization of the class correct
        return m_sum / sz;
    }

    void add_value(value_type value)
    {
        // at first do a subtraction, as it is definitely safe:
        // if value_type is unsigned, then sum contains already
        // the outdated value
        m_sum = m_sum - m_values[m_current_position] + value;

        m_values[m_current_position++] = value;

        if (m_current_position >= sz)
        {
            m_current_position = 0;
        }
    }

private:
    value_type m_values[sz] = {0};    // initialize everything with 0
    value_type m_sum = 0;             // keep sum to make avg faster
    size_type m_current_position = 0; // current writing position 0
};

#endif // _MVANG_H