#pragma once
#include "../LibCoreHeaders.h"


#include "ft2build.h"

#include FT_FREETYPE_H

#include <map> 
#include "ShaderManager.h"

namespace libCore
{
    class FreeTypeManager
    {
    public:

        FreeTypeManager()
        {
            // FreeType
            // --------
            FT_Library ft;
            // All functions return a value different than 0 whenever an error occurred
            if (FT_Init_FreeType(&ft))
            {
                std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
            }

            // find path to font
            std::string font_name = "C:/Users/pdortegon/Documents/PROPIOS/OpenGL_DEV/OpenGLSandbox/Core/assets/fonts/Antonio-Bold.ttf";
            if (font_name.empty())
            {
                std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
            }

            // load font as face
            FT_Face face;
            if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
                std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
            }
            else {
                // set size to load glyphs as
                FT_Set_Pixel_Sizes(face, 0, 48);

                // disable byte-alignment restriction
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

                // load first 128 characters of ASCII set
                for (unsigned char c = 0; c < 128; c++)
                {
                    // Load character glyph 
                    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                    {
                        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                        continue;
                    }
                    // generate texture
                    unsigned int texture;
                    glGenTextures(1, &texture);
                    glBindTexture(GL_TEXTURE_2D, texture);
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RED,
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        0,
                        GL_RED,
                        GL_UNSIGNED_BYTE,
                        face->glyph->bitmap.buffer
                    );
                    // set texture options
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    // now store character for later use
                    Character character = {
                        texture,
                        glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                        glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                        static_cast<unsigned int>(face->glyph->advance.x)
                    };
                    Characters.insert(std::pair<char, Character>(c, character));
                }
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            // destroy FreeType once we're finished
            FT_Done_Face(face);
            FT_Done_FreeType(ft);


            // configure VAO/VBO for texture quads
            // -----------------------------------
            // Configuraci�n del VAO/VBO para los quads de textura
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // Reserva memoria para el VBO con un tama�o suficiente para manejar los v�rtices con las coordenadas 3D + textura
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

            // Configuraci�n del atributo de posici�n (3 componentes)
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

            // Configuraci�n del atributo de coordenadas de textura (2 componentes)
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        void RenderText(std::string text, glm::vec3 position, float scale, glm::vec3 color)
        {
            // Activate corresponding render state    
            libCore::ShaderManager::Get("text")->use();
            libCore::ShaderManager::Get("text")->setVec3("textColor", color);
            libCore::ShaderManager::Get("text")->setInt("text", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindVertexArray(VAO);

            float xStart = position.x;

            // Iterate through all characters
            for (auto c = text.begin(); c != text.end(); c++)
            {
                Character ch = Characters[*c];

                float xpos = xStart + ch.Bearing.x * scale;
                float ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;
                float zpos = position.z; // Use the Z-coordinate from position

                float w = ch.Size.x * scale;
                float h = ch.Size.y * scale;
                // Update VBO for each character
                float vertices[6][5] = {
                    { xpos,     ypos + h,   zpos, 0.0f, 0.0f },
                    { xpos,     ypos,       zpos, 0.0f, 1.0f },
                    { xpos + w, ypos,       zpos, 1.0f, 1.0f },

                    { xpos,     ypos + h,   zpos, 0.0f, 0.0f },
                    { xpos + w, ypos,       zpos, 1.0f, 1.0f },
                    { xpos + w, ypos + h,   zpos, 1.0f, 0.0f }
                };
                // Render glyph texture over quad
                glBindTexture(GL_TEXTURE_2D, ch.TextureID);
                // Update content of VBO memory
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

                glBindBuffer(GL_ARRAY_BUFFER, 0);
                // Render quad
                glDrawArrays(GL_TRIANGLES, 0, 6);
                // Now advance cursors for next glyph
                xStart += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels
            }
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }



    private:/// Holds all state information relevant to a character as loaded using FreeType

        unsigned int VAO, VBO;

        struct Character {
            unsigned int TextureID; // ID handle of the glyph texture
            glm::vec2   Size;      // Size of glyph
            glm::vec2   Bearing;   // Offset from baseline to left/top of glyph
            unsigned int Advance;   // Horizontal offset to advance to next glyph
        };

        std::map<GLchar, Character> Characters;

    };
}