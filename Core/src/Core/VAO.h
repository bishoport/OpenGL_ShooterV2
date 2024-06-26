#pragma once

#include<glad/glad.h>
#include"VBO.h"


//-- Vertex Array Object(VAO) -------------------------------------------------------------------------------------
//Act�a como un administrador de los VBOs y EBOs.Un VAO guarda la configuraci�n de c�mo los datos de v�rtices
//y los �ndices est�n almacenados en los VBOs y EBOs respectivamente.Esto incluye informaci�n sobre el formato 
//de los datos de v�rtices, c�mo acceder a ellos, y qu� VBO contiene qu� datos.El VAO no almacena los datos de 
//v�rtices por s� mismo, sino que mantiene referencias a los VBOs y EBOs que s� lo hacen.
//---------------------------------------------------------------------------------------------------------------

namespace libCore
{
	class VAO
	{
	public:
		// ID reference for the Vertex Array Object
		GLuint ID;
		// Constructor that generates a VAO ID
		VAO();

		// Links a VBO Attribute such as a position or color to the VAO
		void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
		// Binds the VAO
		void Bind();
		// Unbinds the VAO
		void Unbind();
		// Deletes the VAO
		void Delete();
	};
}


//--La relaci�n de trabajo entre estos objetos es la siguiente :
//--Creas un VBO y subes tus datos de v�rtices a la GPU.
//--Opcionalmente, creas un EBO y subes tus �ndices, si est�s usando indexaci�n para tus v�rtices.
//Creas un VAO y configuras c�mo OpenGL debe interpretar los datos del VBO(y opcionalmente del EBO).
//Esto incluye indicar el layout de los datos del v�rtice y la configuraci�n de los �ndices del EBO si es que usas uno.
//Cuando dibujas tu objeto, enlazas(bind) el VAO, y OpenGL utiliza toda la configuraci�n almacenada en el VAO para obtener 
//los datos del VBO y EBO para renderizar la geometr�a.
// 
//Cada vez que quieras dibujar un objeto, solo necesitas enlazar el VAO correspondiente y ejecutar el comando de dibujo.
//Esto hace que la gesti�n de m�ltiples objetos en la escena sea mucho m�s eficiente y organizada.
