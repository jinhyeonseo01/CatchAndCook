#pragma once


class IComponentFactory
{
public:
    using Creator = std::function<std::shared_ptr<Component>()>;

    static std::unordered_map<std::string, Creator>& GetRegistry() {
        static std::unordered_map<std::string, Creator> registry;
        return registry;
    }

    static void RegisterComponent(const std::string& name, const Creator& creator) {
        GetRegistry()[name] = creator;
    }

    static std::shared_ptr<Component> Create(const std::string& name) {
        auto& registers = GetRegistry();
        auto it = registers.find(name);
        if (it != registers.end()) {
            return it->second();
        }
        return nullptr;
    }
};

#define COMPONENT(CLASSNAME) \
namespace { \
    struct Register_##CLASSNAME { \
        Register_##CLASSNAME() { \
            IComponentFactory::RegisterComponent(#CLASSNAME, []() -> std::shared_ptr<Component> { return std::make_shared<CLASSNAME>(); }); \
        } \
    }; \
    static Register_##CLASSNAME global_Register_##CLASSNAME; \
}

