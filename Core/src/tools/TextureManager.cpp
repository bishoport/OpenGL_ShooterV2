#include "TextureManager.h"
#include <filesystem> // Necesario para la navegación por directorios
#include <iostream>

namespace fs = std::filesystem;

namespace libCore
{
    Ref<Texture> TextureManager::LoadTexture(const char* directoryPath, const char* fileName, TEXTURE_TYPES type, GLuint slot)
    {
        // Función auxiliar para buscar la textura recursivamente en subcarpetas
        auto findTextureInSubfolders = [](const fs::path& baseDir, const std::string& imageName) -> fs::path
        {
            for (auto& p : fs::recursive_directory_iterator(baseDir))
            {
                if (fs::is_regular_file(p) && p.path().filename() == imageName)
                {
                    return p.path();
                }
            }
            return "";
        };

        // Ruta completa de la textura
        fs::path imagePathFS = fs::path(directoryPath) / fileName;
        std::string textureKey = imagePathFS.string();

        // Verificar si la textura ya está cargada
        auto it = loadedTextures.find(textureKey);
        if (it != loadedTextures.end()) {
            //std::cout << "LA TEXTURA " << textureKey << " YA EXISTE EN LOADEDTEXTURES" << std::endl;
            return it->second;  // Retorna la textura si ya está cargada
        }

        // Cargamos la textura si no existe en la biblioteca:
        if (!fs::exists(imagePathFS))
        {
            std::cout << "Texture not found at initial path. Searching in subfolders..." << std::endl;
            fs::path foundPath = findTextureInSubfolders(directoryPath, fileName);

            if (foundPath.empty())
            {
                std::cerr << "Texture not found in any subfolder." << std::endl;
                return nullptr;
            }

            imagePathFS = foundPath;
            textureKey = imagePathFS.string();
            std::cout << "Texture found at: " << imagePathFS << std::endl;
        }

        std::cout << "Loading Texture->" << imagePathFS << std::endl;
        auto texture = CreateRef<Texture>(imagePathFS.string().c_str(), type, slot);

        // Guardar la textura en loadedTextures
        loadedTextures[textureKey] = texture;

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

    void libCore::TextureManager::SaveImage(const std::string& filePath, int width, int height, int channels, unsigned char* data)
    {
        stbi_flip_vertically_on_write(false); // Voltear verticalmente la imagen para coincidir con las coordenadas de OpenGL
        stbi_write_png(filePath.c_str(), width, height, channels, data, width * channels);
    }


    // Carga una textura desde un archivo y la devuelve como GLuint
    GLuint TextureManager::LoadTextureFromFile(const char* filePath)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

        if (data)
        {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            GLenum format = GL_RGB;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
            return textureID;
        }
        else
        {
            std::cout << "Failed to load texture: " << filePath << std::endl;
            return 0;
        }
    }
}
