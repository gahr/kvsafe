///
// Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
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

namespace Password
{
template <typename Provider>
class Confirmer
{
    const Provider& d_provider1;
    const Provider& d_provider2;
    std::string d_onFailure;

public:
    Confirmer(const Provider& provider)
        : d_provider1(provider)
        , d_provider2(provider)
    {
    }

    Confirmer(const Provider& provider1, const Provider& provider2)
        : d_provider1(provider1)
        , d_provider2(provider2)
    {
    }

    void setFailureMessage(std::string onFailure)
    {
        d_onFailure = std::move(onFailure);
    }

    std::string operator()() const
    {
        std::string password1, password2;
        while (true)
        {
            password1 = d_provider1();
            if (password1.empty())
            {
                return password1;
            }

            password2 = d_provider2();
            if (password2.empty())
            {
                return password2;
            }

            if (password1 == password2)
            {
                return password1;
            }

            if (!d_onFailure.empty())
            {
                std::cerr << d_onFailure << "\n";
            }
        }
    }
};
} // namespace Password
