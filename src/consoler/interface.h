///
// Copyright (C) 2014-2016 Pietro Cerutti <gahr@gahr.ch>
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
#include <functional>
#include <set>

namespace std {

    bool operator<(const std::reference_wrapper<const std::string>& lhs,
                   const std::reference_wrapper<const std::string>& rhs)
    {
        return lhs.get() < rhs.get();
    }

    bool operator==(const std::reference_wrapper<const std::string>& lhs,
                    const std::reference_wrapper<const std::string>& rhs)
    {
        return lhs.get() == rhs.get();
    }
}

namespace Consoler {

    template<typename EV>
    bool compareEntityValue(const EV& lhs, const EV& rhs)
    {
        return lhs.entity < rhs.entity ||
               (lhs.entity == rhs.entity && lhs.prop < rhs.prop);
    }

struct Interface
{
    typedef std::reference_wrapper<const std::string> StringRef;
    typedef std::set<StringRef> EntityList;

    struct EntityPropValue
    {
        StringRef entity;
        StringRef prop;
        StringRef value;

        bool operator<(const EntityPropValue& other) const
        {
            return compareEntityValue(*this, other);
        }
    };
    typedef std::set<EntityPropValue> EntityPropValueList;

    struct EntityProp
    {
        StringRef entity;
        StringRef prop;

        bool operator<(const EntityProp& other) const
        {
            return compareEntityValue(*this, other);
        }
    };
    typedef std::set<EntityProp> EntityPropList;

    Interface(int& argc, char ** argv);
    void emitEntities(const EntityList& list) const;
    void emitProps(const EntityPropList& list) const;
    void emitValues(const EntityPropValueList& list) const;
};
}
