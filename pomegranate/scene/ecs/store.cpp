#include "pch.hpp"

#include "store.hpp"

namespace pom {
    Store::Store()
    {
        archetypes.push_back(new Archetype(this, {}));
    }

    Store::~Store()
    {
        for (Archetype* a : archetypes)
            delete a;
    }

    Entity Store::createEntity()
    {
        // TODO: replace this with a free list to reuse component ids.
        static Entity counter = 0;

        Entity ret = counter++; // NOTE: 0 is reserved?
        records[ret] = archetypes[0]->addEntity(ret);

        return ret;
    }

    void Store::destroyEntity(Entity entity)
    {
        auto recordPair = records.find(entity);
        POM_ASSERT(recordPair != records.end(), "Cannot destroy nonexistent entity ", entity);
        recordPair->second.archetype->removeEntity(recordPair->second.idx);
    }

    [[nodiscard]] bool Store::exists(Entity entity) const
    {
        return records.contains(entity);
    }

    [[nodiscard]] const Type& Store::getType(Entity entity) const
    {
        auto recordPair = records.find(entity);
        POM_ASSERT(recordPair != records.end(), "Cannot get type of nonexistent entity ", entity);
        return recordPair->second.archetype->getType();
    }

    Archetype* Store::createChildArchetype(Archetype* archetype, const ComponentMetadata& added)
    {
        Type newType = archetype->type.add(added);
        auto* newArchetype = new Archetype(this, newType);
        archetypes.push_back(newArchetype);
        return archetype->addEdges.add(added.id, newArchetype);
    }

    Archetype* Store::findOrCreateArchetype(const Type& type)
    {
        Archetype* cur = getNullArchetype();
        for (const ComponentMetadata& component : type) {
            Archetype* next = cur->addEdges.get(component.id);
            if (next == nullptr) {
                cur = createChildArchetype(cur, component);
            } else {
                cur = next;
            }
        }

        return cur;
    }
} // namespace pom