#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include "Scripts/ScriptMoveTest.h"

using namespace libCore;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Código de inicialización, si es necesario
        DisableThreadLibraryCalls(hModule); // Opcional: Para evitar llamadas adicionales
        break;
    case DLL_PROCESS_DETACH:
        // Código de limpieza, si es necesario
        break;
    }
    return TRUE;
}

// Función de registro de scripts en ScriptFactory
extern "C" __declspec(dllexport) void RegisterScripts(ScriptFactory & factory)
{
    if (&factory != nullptr) {
        factory.RegisterScript<libCore::ScriptMoveTest>("ScriptMoveTest");
    }
    else {
        // Manejar el caso en que factory no esté inicializado
        std::cerr << "Error: ScriptFactory no está inicializado." << std::endl;
    }
}

