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
        TEXTURE_TYPES m_type = TEXTURE_TYPES::ALBEDO;

        GLuint m_unit = 0;
        std::string m_textureName = "";
        std::string texturePath = "";

        // Constructor predeterminado
        Texture() = default;

        Texture(const char* image, TEXTURE_TYPES type, GLuint slot);

        // Binds a texture
        void Bind(const std::string& shader);
        // Unbinds a texture
        void Unbind();
        // Deletes a texture
        void Delete();

        bool IsValid() const
        {
            return ID != 0;
        }

        // Get the texture ID
        GLuint GetTextureID() const { return ID; }
    };
}
