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

#include "consoler/interface.h"

#include <functional>
#include <regex>

namespace {

    struct PasswordSetter
    {
        template<typename FilerImpl>
        typename std::enable_if<!FilerImpl::IS_PASSWORD_BASED, bool>::type
        operator()(FilerImpl&, const std::string& msg)
        {
            return false;
        }

        template<typename FilerImpl>
        typename std::enable_if<FilerImpl::IS_PASSWORD_BASED, bool>::type
        operator()(FilerImpl& filer, const std::string& msg)
        {
            std::string password;
            while (password.empty())
            {
                password = Util::readPassword(msg);
            }

            filer.setPassword(password);
            return true;
        }
    };
}

template<typename FilerImpl, typename ConsolerImpl>
Store<FilerImpl, ConsolerImpl>::Store(int& argc, char ** argv)
    : d_modified(false)
    , d_loaded(false)
    , d_filer(argc, argv)
    , d_consoler(argc, argv)
{
    logger << "Store::Store\n";
}

template<typename FilerImpl, typename ConsolerImpl>
Store<FilerImpl, ConsolerImpl>::~Store()
{
    logger << "Store::~Store";
}

template<typename FilerImpl, typename ConsolerImpl>
bool
Store<FilerImpl, ConsolerImpl>::modified() const
{
    return d_modified;
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::modified(bool m)
{
    logger << "modified(" << m << ")\n";
    d_modified = m;
}

template<typename FilerImpl, typename ConsolerImpl>
bool
Store<FilerImpl, ConsolerImpl>::loaded() const
{
    return d_loaded;
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::loaded(bool l)
{
    logger << "loaded(" << l << ")\n";
    d_loaded = l;
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::setFilename(const std::string& filename)
{
    logger << "Store::setFilename(" << filename << ")\n";
    d_filer.setFilename(filename);
}

template<typename FilerImpl, typename ConsolerImpl>
auto
Store<FilerImpl, ConsolerImpl>::entities() const
    -> decltype(d_entities)
{
    return d_entities;
}

template<typename FilerImpl, typename ConsolerImpl>
bool
Store<FilerImpl, ConsolerImpl>::load()
{
    logger << "Store::load()\n";
    if (loaded())
    {
        logger << "already loaded\n";
        return true;
    }

    PasswordSetter()(d_filer, "Password");

    bool ret = d_filer.load(*this);
    modified(false);
    loaded(ret);
    return ret;
}

template<typename FilerImpl, typename ConsolerImpl>
bool
Store<FilerImpl, ConsolerImpl>::save() const
{
    logger << "Store::save()\n";
    if (!modified())
    {
        logger << "not modified\n";
        return true;
    }

    return d_filer.save(*this);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitEntities(const std::string& match) const
{
    std::regex r{match};
    std::vector<Consoler::Interface::StringRef> entityList;

    if (!loaded())
    {
        return;
    }

    for (const auto& e : d_entities)
    {
        if (e.skip())
        {
            continue;
        }

        if (!match.empty() && !std::regex_match(e.name(), r))
        {
            continue;
        }

        entityList.push_back(e.name());
    }

    return d_consoler.emitEntities(entityList);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitProps(const std::string& entity) const
{
    if (!loaded())
    {
        return;
    }

    auto e = findNameable(d_entities, entity);
    if (!e || e->skip())
    {
        return;
    }

    std::vector<Consoler::Interface::StringRef> props;
    for (const auto& p : e->props())
    {
        if (p.skip())
        {
            continue;
        }

        props.push_back(p.name());
    }
    return d_consoler.emitProps(entity, props);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitValue(const std::string& entity, const std::string& prop) const
{
    if (!loaded())
    {
        return;
    }

    const auto e = findNameable(d_entities, entity);
    if (!e || e->skip())
    {
        return;
    }

    const auto p = findNameable(e.iter->props(), prop);
    if (!p || p->skip())
    {
        return;
    }

    return d_consoler.emitValue(entity, prop, p->value());
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::set(const std::string& entity, 
        const std::string& key,
        const std::string& value)
{
    logger << "Store::set(" << entity << ", " << key << ", " << value << ")\n";

    auto er = findNameable(d_entities, entity);
    if (!er)
    {
        logger << "Entity not found, adding\n";
        // new entity
        d_entities.push_back({entity, key, value});
        modified(true);
    }
    else
    {
        // existing entity
        logger << "Entity found\n";
        auto pr = findNameable(er->props(), key);
        if (!pr)
        {
            logger << "Prop not found, adding\n";
            // new prop
            er->props().push_back({key, value});
            modified(true);
        }
        else
        {
            // existing prop
            logger << "Prop found\n";
            if (pr->value() != value)
            {
                logger << "Setting new value\n";
                // value has changed
                pr->value(value);
                modified(true);
            }
            else
            {
                logger << "Value hasn't changed\n";
            }
        }
    }
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::removeEntity(const std::string& entity)
{
    logger << "Store::removeEntity(" << entity << ")\n";

    if (!loaded())
    {
        return;
    }

    auto er = findNameable(d_entities, entity);
    if (!er)
    {
        logger << "entity not found\n";
        return;
    }

    logger << "entity removed\n";
    er->skip(true);
    modified(true);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::removeProp(const std::string& entity, const std::string& prop)
{
    logger << "Store::removeProp(" << entity << ", " << prop << ")\n";

    if (!loaded())
    {
        return;
    }

    auto er = findNameable(d_entities, entity);
    if (!er)
    {
        logger << "entity not found\n";
        return;
    }

    auto pr = findNameable(er->props(), prop);
    if (!pr)
    {
        logger << "prop not found\n";
        return;
    }

    logger << "prop removed\n";
    pr->skip(true);
    modified(true);
}

template<typename FilerImpl, typename ConsolerImpl>
bool
Store<FilerImpl, ConsolerImpl>::changePassword()
{
    if (!loaded())
    {
        return false;
    }

    if (PasswordSetter()(d_filer, "New password"))
    {
        modified(true);
    }
    return true;
}
