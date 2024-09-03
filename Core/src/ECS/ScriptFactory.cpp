#include "ScriptFactory.h"

namespace libCore
{
    ScriptFactory::ScriptFactory() {
        std::cout << "Se crea un Script Factory " << std::endl;
    }

    ScriptFactory& ScriptFactory::GetInstance() {
        static ScriptFactory instance;
        return instance;
    }

    Ref<Script> ScriptFactory::CreateScript(const std::string& scriptName) {
        auto it = creators.find(scriptName);
        if (it != creators.end()) {
            std::cout << "Se crea Script  " << scriptName << std::endl;
            return it->second();
        }
        return nullptr;
    }

    const std::unordered_map<std::string, ScriptFactory::ScriptCreator>& ScriptFactory::GetCreators() const 
    {   
        //std::cout << "creators count  " << creators.size() << std::endl;
        return creators;
    }
}
