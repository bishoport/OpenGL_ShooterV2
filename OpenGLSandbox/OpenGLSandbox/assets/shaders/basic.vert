#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIds; 
layout (location = 6) in vec4 aWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 model;
uniform mat4 camMatrix; //view + projection

uniform bool use_skeletal;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoord;

void main()
{
    if(use_skeletal == false)
    {
        gl_Position = camMatrix * model * vec4(aPos, 1.0);
    }
    else
    {
        vec4 totalPosition = vec4(0.0f);
    
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(aBoneIds[i] == -1) 
                continue;

            if(aBoneIds[i] >=MAX_BONES) 
            {
                totalPosition = vec4(aPos,1.0f);
                break;
            }

            vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos,1.0f);
            totalPosition += localPosition * aWeights[i];
            vec3 localNormal = mat3(finalBonesMatrices[aBoneIds[i]]) * aNormal;
        }

        gl_Position = camMatrix * model * totalPosition; 
    }

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
