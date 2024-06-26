#include "TextureManager.h"

namespace libCore
{
    Ref<Texture> TextureManager::LoadTexture(const char* imagePath, TEXTURE_TYPES type, GLuint slot)
    {
        // Intenta obtener la textura desde el AssetsManager
        auto textureFromAssets = AssetsManager::GetInstance().GetTexture(imagePath);
        if (textureFromAssets) 
        {
            //std::cout << "LA TEXTURA " << imagePath << " YA EXISTE EN ASSETS" << std::endl;
            return textureFromAssets;  // Retorna la textura si ya est� cargada
        }

        //Cargamos la textura si no existe en la biblioteca:
        //std::cout << "Loading Texture->" << imagePath << std::endl;
        auto texture = CreateRef<Texture>(imagePath, type, slot);
        AssetsManager::GetInstance().SetTexture(imagePath, texture);
        return texture;
    }

    GLuint TextureManager::LoadImagesForCubemap(std::vector<const char*> faces)
    {
        stbi_set_flip_vertically_on_load(false);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;


        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char* data = stbi_load(faces[i], &width, &height, &nrComponents, 0);

            if (data)
            {
                std::cout << "Cubemap TExture Loaded " << faces[i] << std::endl;
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }


    GLuint libCore::TextureManager::loadHDR(const char* filepath)
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* data = stbi_loadf(filepath, &width, &height, &nrComponents, 0);

        unsigned int hdrTexture{};

        if (data)
        {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);

            std::cout << "Image HDR loaded successfully" << std::endl;
        }
        else
        {
            std::cout << "Failed to load HDR image." << std::endl;
        }

        return hdrTexture;
    }
}
