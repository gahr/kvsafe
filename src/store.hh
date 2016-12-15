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

#include <algorithm>
#include <functional>
#include <regex>

namespace {

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

    template<typename List>
    struct EntityIterator
    {
        List& d_list;
        std::regex d_entityRex;
        std::regex d_propRex;
        EntityIterator(List& list)
            : d_list(list)
            , d_entityRex(".*")
            , d_propRex(".*")
        {}

        EntityIterator(List& list, const std::string& e)
            : d_list(list)
            , d_entityRex(std::string(".*").append(e).append(".*"))
            , d_propRex(".*")
        {}

        EntityIterator(List& list, const std::string& e, const std::string& p)
            : d_list(list)
            , d_entityRex(std::string(".*").append(e).append(".*"))
            , d_propRex(std::string(".*").append(p).append(".*"))
        {}

        void operator()(const Entity& e)
        {
            if (!e.skip() && std::regex_match(e.name(), d_entityRex))
            {
                for (auto&& p : e.props())
                {
                    if (!p.skip() && std::regex_match(p.name(), d_propRex))
                    {
                        add(e, p);
                    }
                }
            }
        }

        void add(const Entity& e, const Prop& p);
    };

    template<>
    void EntityIterator<Consoler::Interface::EntityList>::add(
            const Entity& e, const Prop&)
    {
        d_list.insert({e.name()});
    }

    template<>
    void EntityIterator<Consoler::Interface::EntityPropList>::add(
            const Entity& e, const Prop& p)
    {
        d_list.push_back({e.name(), p.name()});
    }

    template<>
    void EntityIterator<Consoler::Interface::EntityPropValueList>::add(
            const Entity& e, const Prop& p)
    {
        d_list.push_back({e.name(), p.name(), p.value()});
    }

    template<typename List>
    EntityIterator<List> makeEntityIterator(List& list)
    {
        return EntityIterator<List>(list);
    }

    template<typename List>
    EntityIterator<List> makeEntityIterator(List& list,
            const std::string& entity)
    {
        return EntityIterator<List>(list, entity);
    }

    template<typename List>
    EntityIterator<List> makeEntityIterator(List& list,
            const std::string& entity, const std::string& prop)
    {
        return EntityIterator<List>(list, entity, prop);
    }
}

template<typename FilerImpl, typename ConsolerImpl>
Store<FilerImpl, ConsolerImpl>::Store(int& argc, char ** argv)
    : d_modified(false)
    , d_loaded(false)
    , d_explicitPassword(false)
    , d_filer(argc, argv)
    , d_consoler(argc, argv)
{
    logger << "Store::Store\n";
}

template<typename FilerImpl, typename ConsolerImpl>
Store<FilerImpl, ConsolerImpl>::~Store()
{
    logger << "Store::~Store\n";
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
void
Store<FilerImpl, ConsolerImpl>::setPassword(const std::string& password)
{
    logger << "Store::setPassword\n";
    PasswordSetter()(PasswordCacher(password), d_filer);
    d_explicitPassword = true;
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

    if (!d_explicitPassword)
    {
        PasswordSetter()(PasswordPrompter("Password"), d_filer);
    }

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

    return d_filer.save(entities());
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitEntities(const std::string& entity) const
{
    std::vector<Consoler::Interface::StringRef> entityList;

    if (!loaded())
    {
        return;
    }

    Consoler::Interface::EntityList e;
    auto adder = makeEntityIterator(e, entity);
    std::for_each(d_entities.begin(), d_entities.end(), adder);
    return d_consoler.emitEntities(e);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitProps(const std::string& entity, const std::string& prop) const
{
    if (!loaded())
    {
        return;
    }

    Consoler::Interface::EntityPropList ep;
    auto adder = makeEntityIterator(ep, entity, prop);
    std::for_each(d_entities.begin(), d_entities.end(), adder);
    return d_consoler.emitProps(ep);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitValues(const std::string& entity, const std::string& prop) const
{
    if (!loaded())
    {
        return;
    }

    Consoler::Interface::EntityPropValueList epv;
    auto adder = makeEntityIterator(epv, entity, prop);
    std::for_each(d_entities.begin(), d_entities.end(), adder);
    return d_consoler.emitValues(epv);
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

    if (PasswordSetter()(PasswordPrompter("New password"), d_filer))
    {
        modified(true);
    }
    return true;
}
