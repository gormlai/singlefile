#ifndef _CIRCULAR_ARRAY_H_
#define _CIRCULAR_ARRAY_H_

/*
LICENSE - this file is public domain
 
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
 
 */

template<int SIZE, typename T>
class CircularArray
{
public:
    CircularArray();
    T average() const;
    T addValue(T frameTime);
    T sum() const { return _sum; }

private:
    T _sum;
    T _values[SIZE];
    int _index;
    
public:
    
    
};

template<int SIZE, typename T>
CircularArray<SIZE, T>::CircularArray()
:_sum(0)
,_index(0)
{
    memset(&_values[0], 0, sizeof(_values));
}

template<int SIZE, typename T>
T CircularArray<SIZE, T>::average() const
{
    return sum() / (T)SIZE;
}

template<int SIZE, typename T>
T CircularArray<SIZE, T>::addValue(T newValue)
{
    int index = _index % SIZE;
    _sum -= _values[index];
    _sum += newValue;
    _values[index] = newValue;
    _index++;
    _index = _index % SIZE;

    return average();
}


#endif
