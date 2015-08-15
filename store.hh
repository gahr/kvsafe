#include "consoler/interface.h"

#include <functional>
#include <regex>

template<typename FilerImpl, typename ConsolerImpl>
Store<FilerImpl, ConsolerImpl>::Store(int& argc, char ** argv)
    : d_modified(false)
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

    while(d_password.empty())
    {
        logger << "getting password\n";
        d_password = Util::readPassword("Password");
        logger << "got password: " << d_password << "\n";
    }

    bool ret = d_filer.load(*this, d_password);
    modified(false);
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

    return d_filer.save(*this, d_password);
}

template<typename FilerImpl, typename ConsolerImpl>
void
Store<FilerImpl, ConsolerImpl>::emitEntities(const std::string& match) const
{
    std::regex r{match};
    std::vector<Consoler::Interface::StringRef> entityList;

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
void
Store<FilerImpl, ConsolerImpl>::changePassword()
{

}
