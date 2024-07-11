#pragma once

#include "../LibCoreHeaders.h"


// Definir la constante M_PI_2
#ifndef M_PI_2
#define M_PI_2 1.5707963267948966 // π/2
#endif


namespace libCore
{
	class LUTGenerator
	{
	public:

		// Método para obtener la instancia de la clase
		static LUTGenerator& GetInstance() {
			static LUTGenerator instance;
			return instance;
		}


		

		GLuint createLTC1Texture(int size) {
			std::vector<float> data = calculateLTC1(size);

			GLuint ltc1Texture;
			glGenTextures(1, &ltc1Texture);
			glBindTexture(GL_TEXTURE_2D, ltc1Texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, data.data());
			glBindTexture(GL_TEXTURE_2D, 0);

			return ltc1Texture;
		}

		

		GLuint createLTC2Texture(int size) {
			std::vector<float> data = calculateLTC2(size);

			GLuint ltc2Texture;
			glGenTextures(1, &ltc2Texture);
			glBindTexture(GL_TEXTURE_2D, ltc2Texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size, size, 0, GL_RGBA, GL_FLOAT, data.data());
			glBindTexture(GL_TEXTURE_2D, 0);

			return ltc2Texture;
		}


		

	private:
		// Constructor privado
		LUTGenerator() {}


		// Función para calcular la matriz inversa M para LTC1
		std::vector<float> calculateLTC1(int size) {
			std::vector<float> data(size * size * 4); // RGBA values
			int index = 0;

			for (int y = 0; y < size; ++y) {
				for (int x = 0; x < size; ++x) {
					float roughness = (float)x / (size - 1); // Rugosidad
					float theta = (float)y / (size - 1) * M_PI_2; // Ángulo

					// Aquí debes calcular la matriz M basada en tus necesidades
					// Por simplicidad, inicializamos con una matriz de identidad
					glm::mat3 M = glm::mat3(1.0f);
					glm::mat3 Minv = glm::inverse(M);

					// Almacenar la matriz inversa M en formato RGBA
					data[index++] = Minv[0][0];
					data[index++] = Minv[1][0];
					data[index++] = Minv[2][0];
					data[index++] = 1.0f; // Alpha, no utilizado en este ejemplo
				}
			}

			return data;
		}


		// Función para calcular los valores de GGX norm, Fresnel y sphere clipping
		std::vector<float> calculateLTC2(int size) {
			std::vector<float> data(size * size * 4); // RGBA values
			int index = 0;

			for (int y = 0; y < size; ++y) {
				for (int x = 0; x < size; ++x) {
					float roughness = (float)x / (size - 1); // Rugosidad
					float theta = (float)y / (size - 1) * M_PI_2; // Ángulo

					// GGX Normalization
					float ggxNorm = 1.0f / (4.0f * roughness * roughness * std::max(std::cos(theta), 0.0f) + 1e-4f);

					// Fresnel Term (Schlick's approximation)
					float F0 = 0.04f; // Fresnel reflectance at normal incidence
					float fresnel = F0 + (1.0f - F0) * std::pow(1.0f - std::cos(theta), 5.0f);

					// Sphere Clipping
					float sphereClipping = std::max(0.0f, std::cos(theta));

					// Almacenar los valores en formato RGBA
					data[index++] = ggxNorm;
					data[index++] = fresnel;
					data[index++] = 0.0f; // unused
					data[index++] = sphereClipping;
				}
			}

			return data;
		}
		

		// Eliminar la posibilidad de copiar o asignar instancias
		LUTGenerator(const LUTGenerator&) = delete;
		LUTGenerator& operator=(const LUTGenerator&) = delete;
	};
}