#pragma once

#include "../LibCoreHeaders.h"
#include <unordered_map>
#include <functional>
#include "Script.h"

namespace libCore
{
    class ScriptFactory {
    public:
        using ScriptCreator = std::function<Ref<Script>()>;

        static ScriptFactory& GetInstance();

        template<typename ScriptType>
        void RegisterScript(const std::string& scriptName);

        Ref<Script> CreateScript(const std::string& scriptName);

        const std::unordered_map<std::string, ScriptCreator>& GetCreators() const;

    private:
        std::unordered_map<std::string, ScriptCreator> creators;

        ScriptFactory();
        ScriptFactory(const ScriptFactory&) = delete;
        ScriptFactory& operator=(const ScriptFactory&) = delete;
    };
}

#include "ScriptFactory.inl"
