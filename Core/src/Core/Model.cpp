#include "Model.h"

namespace libCore
{
	void Model::Draw(const std::string& shader)
	{
		//Bind Textures & Values
		for (unsigned int i = 0; i < materials.size(); i++)
		{
			//VALUES
			libCore::ShaderManager::Get(shader)->setVec3("albedoColor",     materials[i]->albedoColor);
			libCore::ShaderManager::Get(shader)->setFloat("normalStrength", materials[i]->normalStrength);
			libCore::ShaderManager::Get(shader)->setFloat("metallicValue",  materials[i]->metallicValue);
			libCore::ShaderManager::Get(shader)->setFloat("roughnessValue", materials[i]->roughnessValue);

			//TEXTURES
			materials[i]->albedoMap->Bind(shader);
			materials[i]->normalMap->Bind(shader);
			materials[i]->metallicMap->Bind(shader);
			materials[i]->roughnessMap->Bind(shader);
		}
		

		if (childs.size() > 0)
		{
			for (unsigned int i = 0; i < childs.size(); i++)
			{
				childs[i]->Draw(shader);
			}
		}
		else
		{
			//Draw Meshes
			for (unsigned int i = 0; i < meshes.size(); i++)
			{
				libCore::ShaderManager::Get(shader)->setMat4("model", transform.getMatrix());
				meshes[i]->Draw();
			}
		}


		
	}
	void Model::DrawAABB(const std::string& shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			libCore::ShaderManager::Get(shader)->setVec4("u_Color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
			libCore::ShaderManager::Get(shader)->setMat4("model", transform.getMatrix());

			//meshes[i]->UpdateAABB(transform.getMatrix());
			meshes[i]->DrawAABB();
		}
	}


	void Model::UpdateAABB()
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->UpdateAABB(transform.getMatrix());
		}
	}
}
