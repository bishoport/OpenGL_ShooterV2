#pragma once

namespace libCore
{
    template<typename ScriptType>
    void ScriptFactory::RegisterScript(const std::string& scriptName) {
        creators[scriptName] = []() -> Ref<Script> {
            return CreateRef<ScriptType>();
        };
    }
}
