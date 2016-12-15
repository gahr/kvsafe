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

class PasswordPrompter
{
private:
    std::string d_prompt;

public:
    PasswordPrompter(const std::string& prompt)
        : d_prompt(prompt)
    {}

    std::string operator()() const
    {
        std::string password;
        while (password.empty())
        {
            password = Util::readPassword(d_prompt);
        }
        return password;
    }
};

class PasswordCacher
{
private:
    std::string d_password;

public:
    PasswordCacher(const std::string& password)
        : d_password(password)
    {}

    std::string operator()() const
    {
        return d_password;
    }
};


struct PasswordSetter
{
    template<typename PasswordProvider, typename FilerImpl>
    typename std::enable_if<!FilerImpl::IS_PASSWORD_BASED, bool>::type
    operator()(const PasswordProvider&, FilerImpl&)
    {
        return false;
    }

    template<typename PasswordProvider, typename FilerImpl>
    typename std::enable_if<FilerImpl::IS_PASSWORD_BASED, bool>::type
    operator()(const PasswordProvider& provider, FilerImpl& filer)
    {
        filer.setPassword(provider());
        return true;
    }
};
