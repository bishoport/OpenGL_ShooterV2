#pragma once
#include "../LibCoreHeaders.h"
#include "../Core/Texture.h"
#include "../tools/AssetsManager.h"  // Asegúrate de incluir el encabezado

namespace libCore
{
	class TextureManager
	{
	public:
		static Ref<Texture> LoadTexture(const char* imagePath, TEXTURE_TYPES type, GLuint slot);
		static GLuint LoadImagesForCubemap(std::vector<const char*> faces);
	};
}