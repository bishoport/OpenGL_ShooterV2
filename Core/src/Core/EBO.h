#pragma once

#include<glad/glad.h>
#include<vector>


//-- Element Buffer Object(EBO)------------------------------
//A veces tambi�n llamado Index Buffer Object(IBO), es otro tipo de buffer de memoria que almacena �ndices 
//que OpenGL utiliza para decidir qu� v�rtices forman una primitiva(por ejemplo, un tri�ngulo).
//Utilizar EBOs es eficiente porque te permite reutilizar v�rtices que son compartidos entre varias primitivas, 
//lo que puede disminuir significativamente la cantidad de datos que necesitas almacenar y procesar.
//------------------------------------------------------------------------------------------


class EBO
{
public:

	// ID reference of Elements Buffer Object
	GLuint ID;

	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(std::vector<GLuint>& indices);

	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Deletes the EBO
	void Delete();
};