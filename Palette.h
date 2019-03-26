#ifndef _PALETTE_H_
#define _PALETTE_H_

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

class Palette
{
public:
    // idea inspired by this https://iquilezles.org/www/articles/palettes/palettes.htm
    template<typename T, unsigned int DIMENSIONS>
    static void inigoQuilez(const T t, const T palette[4][DIMENSIONS], T result[DIMENSIONS])
    {
        constexpr T twopi = T(6.28318530718);
        for(unsigned int i=0 ; i < DIMENSIONS ; i++)
        {
			const float a = palette[0][i];
			const float b = palette[1][i];
			const float c = palette[2][i];
			const float d = palette[3][i];
			T val = a + (b-a) * cos(twopi * ((d-c) * t +  c));
            result[i] = val;
        }
    };
    
};

#endif
