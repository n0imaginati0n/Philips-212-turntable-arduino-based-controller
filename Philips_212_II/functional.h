#ifndef _FUNCTIONAL_H
#define _FUNCTIONAL_H

/** this function calculates correct duration value. 
  result of micros() overloads ~ every 70 min. i.e. can 
  happen, that the current micros() value is lower, than 
  that in the past. real duration in this situation will be 
  sum of the duration until maximal ulong value and a new value.

  Of course this function will not properly handle long delays. 
  but the skew can happen even in a very small interval.
*/
unsigned long duration(unsigned long start, unsigned long end);

#endif // _FUNCTIONAL_H

template<typename T, unsigned size>
constexpr unsigned arr_size(T(&)[size]) noexcept {
  return size;
}
