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

#include <array>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>

// NaCl
#include <crypto_secretbox.h>
#include <crypto_hash_sha256.h>
#include <randombytes.h>

namespace Filer {
class Simple
{
    std::string d_filename;
    std::string d_key;

public:
    enum : bool { IS_PASSWORD_BASED = true };

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

    void setPassword(const std::string& password)
    {
        d_key = hash(password);
    }

    template<typename Store>
    bool load(Store& store)
    {
        if (d_filename.empty())
        {
            return false;
        }

        std::ifstream ifs { d_filename.c_str(), std::ios::binary };
        ifs.seekg(0, std::ios_base::end);
        if (ifs.tellg() <= 0)
        {
            // empty or non-existant file, assume ok
            return true;
        }
        ifs.seekg(0);

        std::istringstream is;
        if (!decrypt(ifs, is))
        {
            return false;
        }

        std::array<std::string, 3> triple;
        size_t i;
        while (true)
        {
            i = readTriple(is, triple);

            if (!is && !is.eof())
            {
                return false;
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

    template<typename EntityList>
    bool save(const EntityList& entities) const
    {
        if (d_filename.empty())
        {
            return false;
        }

        std::ostringstream os;
        for (const auto& e : entities)
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

                os << e.name().length()  << ":" << e.name() 
                   << p.name().length()  << ":" << p.name()
                   << p.value().length() << ":" << p.value();

                if (!os.good())
                {
                    return false;
                }
            }
        }

        std::ofstream ofs { d_filename.c_str(), std::ios::binary };
        return encrypt(os, ofs);
    }

private:

    size_t readTriple(std::istream& is, std::array<std::string, 3>& triple)
    {
        size_t i;
        for (i = 0; i < 3; ++i)
        {
            size_t length = 0;
            is >> length;
            if (!is)
            {
                return i;
            }
            is.ignore();
            triple[i].resize(length, '\0');
            is.read(&triple[i][0], length);
            if (!is)
            {
                return i;
            }
        }

        return i;
    }

    std::string hash(const std::string& val) const
    {
        return crypto_hash_sha256(val);
    }

    bool encrypt(const std::ostringstream& in, std::ostream& out) const
    {
        std::array<unsigned char, 24> nonce;
        randombytes(nonce.data(), 24);
        std::string n { nonce.begin(), nonce.end() };
        out << n << crypto_secretbox(in.str(), n, d_key);
        return out.good();
    }

    bool decrypt(std::istream& in, std::istringstream& out) const
    {
        std::array<char, 24> nonce;
        in.read(nonce.data(), 24);
        if (!in.good())
        {
            return false;
        }
        std::stringstream ss;
        ss << in.rdbuf();
        try
        {
            out.str(crypto_secretbox_open(ss.str(), { nonce.begin(), nonce.end() }, d_key));
        }
        catch (...)
        {
            return false;
        }
        return in.good();
    }
};
}
