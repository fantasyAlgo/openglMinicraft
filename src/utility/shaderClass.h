#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>

#include <glm/detail/qualifier.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <glm/vec2.hpp> // glm::vec3
#include <glm/mat4x4.hpp> // glm::vec4  

#include <iostream>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	// Reference ID of the Shader Program
	GLuint ID;
  bool isCompiled = false;
	// Constructor that build the Shader Program from 2 different shaders
  Shader(const char* vertexFile, const char* fragmentFile);
	//Shader();

  // process the shader code
  //void MakeShader(const char* vertexfile, const char* fragmentfile);
  //void ProcessShader(std::string vertexCode, std::string fragmentCode);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();
  // set uniforms
  void setMatrix4(const GLchar* name, glm::mat4x4 matrix);
  void setVector2f(const GLchar* name , glm::vec2 vec);
  void setVector3f(const GLchar* name , glm::vec3 vec);
};
#endif
