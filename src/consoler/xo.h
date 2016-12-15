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

#include "consoler/interface.h"
#include "entity.h"
#include "prop.h"

extern "C" {
#include <stdio.h>
#include <libxo/xo.h>
}

namespace Consoler {
struct Xo
{
    Xo(int& argc, char ** argv)
    {
        argc = xo_parse_args(argc, argv);
    }

    ~Xo()
    {
        xo_finish();
    }

    void emitEntities(const Interface::EntityList& list) const
    {
        xo_open_list("entry");
        for (const auto& e : list)
        {
            xo_open_instance("entry");
            xo_emit("{:entity/%s}\n", e.get().c_str());
            xo_close_instance("entry");
        }
        xo_close_list("entry");
    }

    void emitProps(const Interface::EntityPropList& list) const
    {
        xo_open_list("entry");
        for (const auto& elem : list)
        {
            xo_open_instance("entry");
            xo_emit("{k:entity/%s}.{:property/%s}\n",
                    elem.entity.get().c_str(), elem.prop.get().c_str());
            xo_close_instance("entry");
        }
        xo_close_list("entry");
    }

    void emitValues(const Interface::EntityPropValueList& list) const
    {
        xo_open_list("entry");
        for (auto&& elem : list)
        {
            xo_open_instance("entry");
            xo_emit("{k:entity/%s}.{:property/%s}: {:value/%s}\n",
                    elem.entity.get().c_str(), 
                    elem.prop.get().c_str(),
                    elem.value.get().c_str());
            xo_close_instance("entry");
        }
        xo_close_list("entry");
    }
};
}
