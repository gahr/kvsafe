///
// Copyright (C) 2014-2015 Pietro Cerutti <gahr@gahr.ch>
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

#pragma once

#include <string>
#include <ostream>

struct IsSameName
{
    const std::string& name;
    IsSameName(const std::string& name) : name{name} {}
    template<typename T>
        bool operator()(const T& obj)
        {
            return name == obj.name();
        }
};

template<typename Container>
struct FindNameableResult
{
    typedef typename Container::iterator Iterator;
    Iterator iter;
    bool found;

    operator bool() const
    {
        return found;
    }

    Iterator operator->()
    {
        return iter;
    }
};


template<typename Container>
struct FindNameableResult<const Container>
{
    typedef typename Container::const_iterator Iterator;
    Iterator iter;
    bool found;

    operator bool() const
    {
        return found;
    }

    Iterator operator->() const
    {
        return iter;
    }
};

template<typename Container>
FindNameableResult<Container>
findNameable(Container& c, const std::string& name)
{
    FindNameableResult<Container> r;
    r.iter = std::find_if(begin(c), end(c), IsSameName(name));
    r.found = r.iter != end(c);
    return r;
}

template<typename Container>
FindNameableResult<const Container>
findNameable(const Container& c, const std::string& name)
{
    FindNameableResult<const Container> r;
    r.iter = std::find_if(begin(c), end(c), IsSameName(name));
    r.found = r.iter != end(c);
    return r;
}

class Nameable
{
    std::string d_name;

public:
    Nameable(const std::string& name) : d_name{name} {}
    const std::string& name() const { return d_name; }
    void name(const std::string& name) { d_name = name; }
};

inline std::ostream& operator<<(std::ostream& os, const Nameable& n)
{
    os << n.name();
    return os;
}
