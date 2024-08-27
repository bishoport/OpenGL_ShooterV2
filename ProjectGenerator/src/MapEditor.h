#pragma once
#include <tools/ModelLoader.h>
#include <Timestep.h>


class MapEditor
{
public:
    MapEditor() = default;
    void Init();

private:

    int screenWidth = 1080;
    int screenHeight = 720;
};