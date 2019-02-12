#ifndef _SDL_FILE_STREAM_H_
#define _SDL_FILE_STREAM_H_

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

#include <SDL2/SDL.h>

#include <string>
#include <vector>

class SDL_FileStream
{
public:
    enum class OpenFlags
    {
        READ_WRITE = 1 << 0,
        READ_ONLY = 1 << 1,
        BINARY = 1 << 2,
        CONCATENATE = 1 << 3,
    };
    
    
    SDL_FileStream(const std::string & fullpath, unsigned int flags)
    :_ops(nullptr)
    ,_filepath(fullpath)
    {
        open(flags);
    }
    
    ~SDL_FileStream()
    {
        close();
    }
    
    void close()
    {
        if(_ops != nullptr)
            SDL_RWclose(_ops);
        
        _ops = nullptr;
    }
    
    bool isOpen() const
    {
        return _ops != nullptr;
    }
    
    bool open(unsigned int flags)
    {
        if(isOpen())
            return false;
        
        const std::string sFlags = unMaskFlags(flags);
        _ops = SDL_RWFromFile(_filepath.c_str(), sFlags.c_str());
        
        return isOpen();
    }
    
    unsigned int bytesLeft()
    {
        if(!isOpen())
            return 0;
        
        return static_cast<unsigned int>(SDL_RWsize(_ops) - SDL_RWtell(_ops));
    }
    
    unsigned int read(unsigned int bytes, void * data)
    {
        if(!isOpen())
            return 0;
        
        const unsigned int max = bytesLeft();
		size_t amountRead = SDL_RWread(_ops, data, bytes, 1);
        return static_cast<unsigned int>(amountRead);
    }
    
    template<typename T>
    unsigned int read(unsigned int itemCount, std::vector<T> & val)
    {
        const unsigned int numBytesToRead = sizeof(T) * itemCount;
        const unsigned int indexToReadFrom = (unsigned int)val.size();
        val.resize(val.size() + itemCount);
        return read(numBytesToRead, &val[indexToReadFrom]);
    }
    
    template<typename T>
    unsigned int read(std::vector<T> & val)
    {
        const unsigned int numBytesToRead = bytesLeft();
        const unsigned int itemsToRead = numBytesToRead / sizeof(T);
        return read(itemsToRead, val);
    }
    
    unsigned int read(std::string & val)
    {
        const unsigned int numBytesToRead = bytesLeft();
        const unsigned int indexToReadFrom = (unsigned int)val.size();
        val.resize(val.size() + numBytesToRead);
        return read(numBytesToRead, &val[indexToReadFrom]);
    }

    unsigned int size() const
    {
        if(!isOpen())
            return 0;
        
        return static_cast<unsigned int>(SDL_RWsize(_ops));
    }
    
    bool exists() const
    {
        if(isOpen())
            return true;
        
        SDL_FileStream file(_filepath, static_cast<unsigned int>(OpenFlags::READ_ONLY));
        const bool fileExists = file.isOpen();
        return fileExists;
    }

    
private:
    SDL_RWops * _ops;
    std::string _filepath;
    
    const std::string unMaskFlags(unsigned int flags)
    {
        std::string val;
        
        if(flags & static_cast<unsigned int>(OpenFlags::READ_ONLY))
            val += "r";
        else if(flags & (static_cast<unsigned int>(OpenFlags::READ_WRITE) | static_cast<unsigned int>(OpenFlags::CONCATENATE)))
            val += "w+";
        else if(flags & static_cast<unsigned int>(OpenFlags::READ_WRITE))
            val += "a+";

        if(flags & static_cast<unsigned int>(OpenFlags::BINARY))
            val += "b";
        
        return val;
    }
    
};

#endif
