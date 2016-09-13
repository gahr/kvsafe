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

#include <iomanip>
#include <fstream>
#include <string>

namespace Filer {
class Simple
{
    std::string d_filename;

public:
    enum : bool { IS_PASSWORD_BASED = false };

    Simple(int& argc, char **)
    {
    }

    ~Simple()
    {
    }

    void setFilename(const std::string& filename)
    {
        d_filename = filename;
    }

    template<typename Store>
    bool load(Store& store)
    {
        if (d_filename.empty())
        {
            return false;
        }

        std::ifstream ifs{d_filename.c_str(), std::ios::binary};
        if (!ifs)
        {
            return false;
        }

        std::string triple[3];
        while (true)
        {
            size_t i = 0;
            for (i = 0; i < 3; ++i)
            {
                if (!std::getline(ifs, triple[i]))
                {
                    break;
                }
            }

            switch (i)
            {
                case 0:
                    // nothing read
                    return true;
                    break;
                case 1:
                case 2:
                    // partial triple
                    return false;
                    break;
                default:
                    store.set(triple[0], triple[1], triple[2]);
                    break;
            }
        }

        // not reached
        return true;
    }

    template<typename Store>
    bool save(const Store& store) const
    {
        if (d_filename.empty())
        {
            return false;
        }

        std::ofstream ofs{d_filename.c_str(), std::ios::binary};
        if (!ofs)
        {
            return false;
        }

        for (const auto& e : store.entities())
        {
            if (e.skip())
            {
                continue;
            }

            for (const auto& p : e.props())
            {
                if (p.skip())
                {
                    continue;
                }

                ofs << e.name() 
                    << "\n"
                    << p.name()
                    << "\n"
                    << p.value()
                    << "\n";

                if (!ofs.good())
                {
                    return false;
                }
            }
        }

        return true;
    }
};
}
