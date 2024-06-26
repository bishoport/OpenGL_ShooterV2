#pragma once

#include <memory>

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args) 
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

#include <iostream>
#include <string>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define M_PI 3.14159265358979323846


//GLOBAL STRUCTS
enum TEXTURE_TYPES {
	ALBEDO,
	NORMAL,
	METALLIC,
	ROUGHNESS,
	AO
};

enum DRAW_GEOM_LIKE
{
	DOT,
	LINE,
	TRIANGLE
};

enum PRIMITIVES_3D
{
    PRIMITIVE_DOT,
    PRIMITIVE_LINE,
    PRIMIVITE_QUAD,
    PRIMIVITE_PLANE,
    PRIMIVITE_CUBE,
    PRIMIVITE_SPHERE
};