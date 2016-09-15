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

#include "entity.h"
#include "logger.h"
#include "util.h"

#include <string>
#include <vector>

template<typename Filer, typename Consoler>
class Store
{
    typedef std::vector<Entity> Entities;

    Entities    d_entities;
    bool        d_modified;
    bool        d_loaded;

    Filer       d_filer;
    Consoler    d_consoler;

public:
    Store(int& argc, char ** argv);
    ~Store();

    void setFilename(const std::string& filename);

    bool load();
    bool save() const;

    auto entities() const -> decltype(d_entities);

    bool changePassword();

    void emitEntities() const;
    void emitProps(const std::string& entity = std::string()) const;
    void emitValues(const std::string& entity = std::string(), const std::string& prop = std::string()) const;

    void set(const std::string& entity, 
             const std::string& key,
             const std::string& value);

    void removeEntity(const std::string& entity);
    void removeProp(const std::string& entity, const std::string& prop);

private:
    bool modified() const;
    void modified(bool);

    bool loaded() const;
    void loaded(bool);

    void setPassword(const std::string& msg);
};

#include "store.hh"
