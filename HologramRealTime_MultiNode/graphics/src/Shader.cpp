
#include "graphics/Shader.h"
#include <stdio.h>

namespace graphics {


Shader::Shader()
:   vertex_shader_src_(0), 
	pixel_shader_src_(0), 
	geom_shader_src_(0),
	shader_id_(0), 
	vertex_shader_handle_(0), 
	fragment_shader_handle_(0),
	geometry_shader_handle_(0)
{
}

Shader::~Shader()
{
	if (vertex_shader_src_) {
		free(vertex_shader_src_);
	}
	if (pixel_shader_src_) {
		free(pixel_shader_src_);
	}
	if (geom_shader_src_) {
		free(geom_shader_src_);
	}

	glDeleteProgram(shader_id_);
}

GLhandleARB Shader::LoadShader(const char* source, unsigned int type)
{
	GLhandleARB handle;
	GLenum err;
	// shader Compilation variable
	GLint result;				// Compilation code result
	GLint errorLoglength ;
	char* errorLogText;
	GLsizei actualErrorLogLength;
	
	handle = glCreateShader(type);
	if (!handle)
	{
		//We have failed creating the vertex shader object.
		LOG("Failed creating vertex shader object.");
	}
	
	glShaderSource(
					  handle, //The handle to our shader
					  1, //The number of files.
					  &source, //An array of const char * data, which represents the source code of theshaders
					  NULL);

	glCompileShader(handle);

	//Compilation checking.
	glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

	// If an error was detected.
	if (!result)
	{
		//We failed to compile.
		LOG("Shader failed compilation.\n");
		
		//Attempt to get the length of our error log.
		glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
		
		//Create a buffer to read compilation error message
		errorLogText = (char*)malloc(sizeof(char) * errorLoglength);
		
		//Used to get the final length of the log.
		glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);
		
		// Display errors.
		LOG("%s\n",errorLogText);
		
		// Free the buffer malloced earlier
		free(errorLogText);
	}
	
	return handle;
}


void
Shader::BeginShader()
{

	glUseProgram(shader_id_);
}


void
Shader::EndShader()
{
	glUseProgram(0);
}


 
void
Shader::Initialize()
{
	GLenum err;

	shader_id_ = glCreateProgram();
	if (vertex_shader_src_)
		vertex_shader_handle_ = LoadShader(vertex_shader_src_, GL_VERTEX_SHADER);
	if (geom_shader_src_)
		geometry_shader_handle_ = LoadShader(geom_shader_src_, GL_GEOMETRY_SHADER);
	if (pixel_shader_src_)
		fragment_shader_handle_ = LoadShader(pixel_shader_src_,GL_FRAGMENT_SHADER);


	if (vertex_shader_handle_) glAttachShader(shader_id_, vertex_shader_handle_);
	if (vertex_shader_handle_) glDeleteShader(vertex_shader_handle_);
	if (geometry_shader_handle_) glAttachShader(shader_id_, geometry_shader_handle_);
	if (geometry_shader_handle_) glDeleteShader(geometry_shader_handle_);
	if (fragment_shader_handle_) glAttachShader(shader_id_, fragment_shader_handle_);
	if (fragment_shader_handle_) glDeleteShader(fragment_shader_handle_);
	glLinkProgram(shader_id_);
	//err = glGetError();
	//if (err) {
	//	LOG("Shader failed compilation.\n");
	//}
}

void Shader::SetVertexShaderSource(const char* source)
{
	if (!source) return;
	int length = strlen(source);
	vertex_shader_src_ = (char*)malloc(length+10);
	strcpy(vertex_shader_src_, source);
}

void Shader::SetPixelShaderSource(const char* source)
{
	if (!source) return;
	int length = strlen(source);
	pixel_shader_src_ = (char*)malloc(length+10);
	strcpy(pixel_shader_src_, source);
}

void Shader::SetGeometryShaderSource(const char* source)
{
	if (!source) return;
	int length = strlen(source);
	geom_shader_src_ = (char*)malloc(length + 10);
	strcpy(geom_shader_src_, source);
}
};