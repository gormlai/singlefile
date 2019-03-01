#ifndef _RANDOM_H_
#define _RANDOM_H_

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

#include <random>

template<typename T>
class Random
{
public:
	Random(T lowerLimit, T upperLimit)
		: _seed{}
		, _numberGenerator(_seed())
	{

	}

protected:
	std::random_device _seed;
	std::mt19937 _numberGenerator;
};

template<typename T>
class UniformDistribution : public Random<T>
{
public:
	UniformDistribution(T lowerLimit, T upperLimit)
		:Random<T>(lowerLimit, upperLimit)
		,_distribution(float(lowerLimit), float(upperLimit))
	{
	}

	T generate()
	{
		const float t = _distribution(Random<T>::_numberGenerator);
		return T(t);
	}

private:
	std::uniform_real_distribution<float> _distribution;
};


#endif

