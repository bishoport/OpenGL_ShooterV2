#include <windows.h>
#include "Scripts/ScriptMoveTest.h"
#include "ECS/ECS.h"

using namespace libCore;

// DllMain: Punto de entrada de la DLL
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// Función de registro de scripts en ScriptFactory
extern "C" __declspec(dllexport) void RegisterScripts(ScriptFactory & factory)
{
    //ScriptFactory::GetInstance().RegisterScript<ScriptMoveTest>("ScriptMoveTest");
    factory.RegisterScript<libCore::ScriptMoveTest>("ScriptMoveTest");
}
