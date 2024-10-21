#include"EBO.h"
#include <GL/gl.h>


// Constructor that generates a Elements Buffer Object and links it to indices
EBO::EBO(){
  glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}
void EBO::MakeEBO(GLuint* indices, GLsizeiptr size){
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  //std::cout << "error: " << glGetError() << std::endl;
}

// Binds the EBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbinds the EBO
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Deletes the EBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}
