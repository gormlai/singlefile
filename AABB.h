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

#ifndef _AABB_H_
#define _AABB_H_

template<typename T, typename ElementType, unsigned int DIMENSIONS, long MIN, long MAX>
class AABB
{
public:
	AABB() 
	{
		for (unsigned int dim = 0; dim < DIMENSIONS; dim++)
		{
			_min[dim] = ElementType(MAX);
			_max[dim] = ElementType(MIN);
		}

	}

	AABB(const T & min, const T & max)
		:_min(min)
		,_max(max)
	{
	}

	AABB(const AABB & right)
	{
		_min = right._min;
		_max = right._max;
	}

	void extend(const T & p)
	{
		for (unsigned int dim = 0; dim < DIMENSIONS; dim++)
		{
			_min[dim] = std::min<ElementType>(p[dim], _min[dim]);
			_max[dim] = std::max<ElementType>(p[dim], _max[dim]);
		}
	}

	inline T getMin() const { return _min; }
	inline T getMax() const { return _max; }
    inline T getSize() const { return (getMax() - getMin()); }
    inline T getCenter() const { return (getMin() + getMax()) / ((ElementType)(2)); }
	inline void getMinMax(T & min, T & max) const { min = _min; max = _max; }


private:
	T _min;
	T _max;
};

#endif