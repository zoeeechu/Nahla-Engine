// written by: Zoe Somji
// ECS.h  2024

#ifndef ECS_H
#define ECS_H
#include <iostream>
#include <vector>
#include <memory>
#include <bitset>
#include <array>
#include <cstdint>
#include <string>

// union SDL_Event;

class ECSComponent;
class Entity;

using ComponentID = std::size_t;
/**
 * Returns a new ComponentID and increments the lastID for future calls.
 * @return the new ComponentID
 */
inline ComponentID getNewComponentID()
{
    static ComponentID lastID = 0;
    return lastID++;
}

/**
 * A function that returns the component ID for the given type.
 * @tparam T the type for which the component ID is retrieved
 * @return the component ID for the given type
 */
template <typename T>
inline ComponentID getTypeComponentID() noexcept
{
    static ComponentID typeID = getNewComponentID();
    return typeID;
}

constexpr std::size_t maxComponents = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<ECSComponent *, maxComponents>;

class ECSComponent
{
public:
    ECSComponent(ECSComponent *parent_) : parent(parent_) {}
    Entity *entity; // the entity that owns this ECSComponent
    virtual bool OnCreate() = 0;
    virtual void OnDestroy() = 0;
    virtual void Update(const float deltaTime) = 0;
    virtual void Render() const = 0;
    // virtual void HandleEvents(const SDL_Event &sdlEvent) = 0;
    virtual ~ECSComponent(){};

protected:
    ECSComponent *parent;
};

class Entity
{
private:
    std::vector<std::shared_ptr<ECSComponent>> components; // changed unique_ptr to shared_ptr
    ComponentArray componentArray;
    ComponentBitSet componentBitSet;

    std::string name;

    static inline std::size_t nextID = 0;
    std::size_t id;

public:
    bool active = true;
    Entity() : id(nextID++) {}
    std::size_t getID() const { return id; }
    void setName(std::string _name) { name = _name; }
    std::string getName() const { return name; }
    void Update(const float deltaTime)
    {
        // for (auto& c : components) c->HandleEvents(const SDL_Event& sdlEvent);
        for (auto &c : components)
            c->Update(deltaTime);
        for (auto &c : components)
            c->Render();
    }
    void Render() const {}
    bool isActive() const { return active; }

    // bool setActiveState(bool state) { active = state; return active; }
    void OnDestroy() { active = false; }

    template <typename T>
    bool hasComponent() const
    {
        return componentBitSet[getTypeComponentID<T>()];
    }

    template <typename T, typename... TArgs>
    T &addComponent(TArgs &&...mArgs)
    {
        T *c(new T(std::forward<TArgs>(mArgs)...));
        c->entity = this;
        std::shared_ptr<ECSComponent> uPtr{c}; // changed unique_ptr to shared_ptr
        components.emplace_back(std::move(uPtr));
        componentArray[getTypeComponentID<T>()] = c;
        componentBitSet[getTypeComponentID<T>()] = true;
        c->OnCreate();
        return *c;
    }

    template <typename T>
    T &getComponent() const
    {
        auto ptr(componentArray[getTypeComponentID<T>()]);
        return *static_cast<T *>(ptr);
    }
};

class Manager
{
private:
    std::vector<std::shared_ptr<Entity>> entities; // changed unique_ptr to shared_ptr
public:
    void Update(const float deltaTime)
    {
        for (auto &e : entities)
            e->Update(deltaTime);
    }
    void Render() const
    {
        for (auto &e : entities)
            e->Render();
    }

    void clearEntities()
    {
        for (auto &e : entities)
        {
            e->OnDestroy();
        }
        entities.clear();
        refresh();
    }
   
    void refresh()
    {
        entities.erase(std::remove_if(std::begin(entities), std::end(entities), [](const std::shared_ptr<Entity> &mEntity)
                                      {
                // changed unique_ptr to shared_ptr
                return !mEntity->isActive(); }),
                       std::end(entities));
    }

    Entity &addEntity(const std::string &name = "")
    {
        Entity *e = new Entity();
        if (!name.empty())
        {
            e->setName(name);
        }
        std::shared_ptr<Entity> uPtr{e}; // changed unique_ptr to shared_ptr
        entities.emplace_back(std::move(uPtr));
        return *e;
    }
    std::vector<std::shared_ptr<Entity>> getEntities() const { return entities; }
};

#endif