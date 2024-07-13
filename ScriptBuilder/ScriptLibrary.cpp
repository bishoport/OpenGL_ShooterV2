#include "ScriptLibrary.h"

extern "C" SCRIPT_LIBRARY_API libCore::Script * CreateExampleScript() {
    return new libCore::ExampleScript();
}
