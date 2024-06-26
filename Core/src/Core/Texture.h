#pragma once

#include <stb_image/stb_image.h>
#include "../LibCoreHeaders.h"
#include "../tools/ShaderManager.h"

namespace libCore
{
    class Texture
    {
    public:
        GLuint ID = 0;
        TEXTURE_TYPES m_type;
        GLuint m_unit;
        std::string m_textureName = "";

        Texture(const char* image, TEXTURE_TYPES type, GLuint slot);

        // Binds a texture
        void Bind(const std::string& shader);
        // Unbinds a texture
        void Unbind();
        // Deletes a texture
        void Delete();

        // Get the texture ID
        GLuint GetTextureID() const { return ID; }
    };
}
