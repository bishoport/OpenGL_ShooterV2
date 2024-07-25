#pragma once
#include "../LibCoreHeaders.h"
#include "../Core/Texture.h"

namespace libCore
{
	class TextureManager
	{
	public:

		static TextureManager& getInstance() {
			static TextureManager instance;
			return instance;
		}

		Ref<Texture> LoadTexture(const char* directoryPath, const char* fileName, TEXTURE_TYPES type, GLuint slot);
		GLuint LoadImagesForCubemap(std::vector<const char*> faces);
		GLuint loadHDR(const char* filepath);

	private:
		std::unordered_map<std::string, Ref<Texture>> loadedTextures;
		TextureManager() {}
	};
}