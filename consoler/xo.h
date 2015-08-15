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

    void emitEntities(const std::vector<Interface::StringRef>& entities) const
    {
        xo_open_list("entry");
        for (const auto& e : entities)
        {
            xo_open_instance("entry");
            xo_emit("{:entity/%s}\n", e.get().c_str());
            xo_close_instance("entry");
        }
        xo_close_list("entry");
    }

    void emitProps(const std::string& entity,
                   const std::vector<Interface::StringRef>& props) const
    {
        xo_open_list("entry");
        for (const auto& p : props)
        {
            xo_open_instance("entry");
            xo_emit("{k:entity/%s}.{:property/%s}\n",
                    entity.c_str(), p.get().c_str());
            xo_close_instance("entry");
        }
        xo_close_list("entry");
    }

    void emitValue(const std::string& entity,
                   const std::string& prop,
                   const std::string& value) const
    {
        xo_open_list("entry");
        xo_open_instance("entry");
        xo_emit("{k:entity/%s}.{:property/%s}: {:value/%s}\n",
                entity.c_str(),
                prop.c_str(),
                value.c_str());
        xo_close_instance("entry");
        xo_close_list("entry");
    }
};
}
