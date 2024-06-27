#include"Texture.h"
#include <stdexcept>
#include <iostream>

namespace libCore
{
    Texture::Texture(const char* image, TEXTURE_TYPES type, GLuint slot)
    {
        m_type = type;
        // Stores the width, height, and the number of color channels of the image
        int widthImg, heightImg, numColCh;
        // Flips the image so it appears right side up
        stbi_set_flip_vertically_on_load(true);

        // Reads the image from a file and stores it in bytes
        unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

        if (!bytes)
        {
            std::cerr << "Failed to load texture: " << image << std::endl;
            return;
        }

        // Generates an OpenGL texture object
        glGenTextures(1, &ID);
        // Assigns the texture to a Texture Unit
        glActiveTexture(GL_TEXTURE0 + slot);
        m_unit = slot;
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum format = GL_RGB;
        if (numColCh == 1)
            format = GL_RED;
        else if (numColCh == 3)
            format = GL_RGB;
        else if (numColCh == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(bytes);
    }

    void Texture::Bind(const std::string& shader)
    {
        GLenum error;

        // Active the texture unit first
        glActiveTexture(GL_TEXTURE0 + m_unit);
        error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error in glActiveTexture: " << error << std::endl;
        }

        // Bind the texture to the active unit
        glBindTexture(GL_TEXTURE_2D, ID);
        error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error in glBindTexture: " << error << std::endl;
        }

        // Set the appropriate uniform based on the texture type
        if (m_type == TEXTURE_TYPES::ALBEDO)
            libCore::ShaderManager::Get(shader)->setInt("albedoTexture", m_unit);

        if (m_type == TEXTURE_TYPES::NORMAL)
            libCore::ShaderManager::Get(shader)->setInt("normalTexture", m_unit);

        if (m_type == TEXTURE_TYPES::METALLIC)
            libCore::ShaderManager::Get(shader)->setInt("metallicTexture", m_unit);

        if (m_type == TEXTURE_TYPES::ROUGHNESS)
            libCore::ShaderManager::Get(shader)->setInt("roughnessTexture", m_unit);

        // Uncomment if using AO textures
        // if (m_type == TEXTURE_TYPES::AO)
        //     libCore::ShaderManager::Get(shader)->setInt("aoTexture", m_unit);
    }

	void Texture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::Delete()
	{
		glDeleteTextures(1, &ID);
	}
}
