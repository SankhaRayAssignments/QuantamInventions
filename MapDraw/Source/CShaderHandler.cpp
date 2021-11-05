/**
* @file ShaderHandler.cpp
* @brief Class to Handle Shader Files
*
* This file takes vertex & fragment shader code as an file input and converts to opengles shader binaries
* and also helps to set it to opengl pipeline.
*/

/********************************************************************/

#include "CShaderHandler.h"
#include "mb_GLDebug.h"
#include "CFileIO.h"
#include <string.h>

ShaderHandler::ShaderHandler (string vertShader, string fragShader, vector<string> attribList, vector<string> uniformList)
{
    InitShader(vertShader, fragShader, attribList, uniformList);
}

GLvoid ShaderHandler::InitShader(string vertShader, string fragShader, vector<string> attribList, vector<string> uniformList)
{
    // Vertex Shader
    {
        mShaderID[0] = CompileShader(vertShader, GL_VERTEX_SHADER);
        if ( mShaderID[0] == -1 )
            return;
    }
    // Fragment Shader
    {
        mShaderID[1] = CompileShader(fragShader, GL_FRAGMENT_SHADER);
        if ( mShaderID[1] == -1 )
            return;
    }

    if ( mShaderID[0] != 0 && mShaderID[1] != 0 ) {
        mProgramID = glCreateProgram();
        GL_CHECK();
    }
    else
    {
        //return new Exception("Error while Loading shaders");
        printf("Error while Loading shaders");
    }

    if ( mProgramID != 0 ) {
        glAttachShader(mProgramID, mShaderID[0]);
        glAttachShader(mProgramID, mShaderID[1]);

        // Link the two shaders together into a program.
        glLinkProgram(mProgramID);
        GLint isLinked;
        glGetProgramiv(mProgramID, GL_LINK_STATUS, &isLinked);

        if (!isLinked)
        {
            // If an error happened, first retrieve the length of the log message
            int infoLogLength, charactersWritten;
            glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);

            // Allocate enough space for the message and retrieve it
            char* infoLog = new char[infoLogLength];
            glGetProgramInfoLog(mProgramID, infoLogLength, &charactersWritten, infoLog);

            // Display the error in a dialog box
            infoLogLength>1 ? printf("%s", infoLog) : printf("Failed to link shader program.");

            delete[] infoLog;
            return;
        }

        glUseProgram(mProgramID);
    }

    // Bind All Attrib Shader Variables
    GLint length = 0;
    vector<string> attribs = attribList;
    length = attribs.size();
    if ( length  > 0 ) {
        for ( int i = 0; i < length; i++ ) {
            const char  *buff = attribs[i].c_str();
            //cout << attribs[i].c_str();
            m_sAttribs.push_back( attribs[i] );
            GLint attribLoc = glGetAttribLocation(mProgramID, buff );
            if ( attribLoc == -1 ) { printf("Could not get attrib location for %s\n", buff); return; }
            mAttribHandles.push_back ( attribLoc );
        }
    }

    // Bind All uniform Shader variables
    vector<string> uniforms = uniformList;
    length = uniforms.size();
    if ( length  > 0 ) {
        for ( int i = 0; i < length; i++ ) {
            const char  *buff = uniforms[i].c_str();
            m_sUniform.push_back( buff );
            GLint uniformLoc = glGetUniformLocation(mProgramID, buff );

            if ( uniformLoc == -1 ) { printf("Could not get uniform location for %s\n", buff ); return; }
            mUniformHandles.push_back ( uniformLoc );
            //cout << "\nuniformLoc :: " << uniformLoc;
        }
    }
}

GLuint ShaderHandler::CompileShader(string path, GLuint type)
{
    CFileIO file(path);

    if ( !file.isFileOpened())
        return -1;

    // Create a fragment shader object
    GLuint shaderId = glCreateShader(type);

    // Load the source code into it
    const char *source = file.GetContent();
    glShaderSource(shaderId, 1, (const char**)&source, NULL);

    // Compile the source code
    glCompileShader(shaderId);

    // Check that the shader compiled
    GLint isShaderCompiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isShaderCompiled);
    if (!isShaderCompiled)
    {
        // If an error happened, first retrieve the length of the log message
        int infoLogLength, charactersWritten;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        // Allocate enough space for the message and retrieve it
        char* infoLog = new char[infoLogLength];
        glGetShaderInfoLog(shaderId, infoLogLength, &charactersWritten, infoLog);

        // Display the error in a dialog box
        infoLogLength>1 ? printf("%s", infoLog) : printf("Failed to compile shader file: %s\n", path.c_str());

        delete[] infoLog;
        return -1;
    }

    return shaderId;
}

GLvoid ShaderHandler::DetachShaders()
{
    glDetachShader( mProgramID, mShaderID[0] );
    glDetachShader ( mProgramID, mShaderID[1] );
}

GLint ShaderHandler::getAttribHandle(string& shaderVar) {

    if ( shaderVar.length() ==  0) return -1;

    for ( GLuint i = 0 ; i < m_sAttribs.size(); i++) {
        if ( shaderVar.compare(m_sAttribs[i]) == 0 )
            return mAttribHandles[i];
    }
    return -1;
}
GLint ShaderHandler::getUniformHandle(string& shaderVar) {

    if ( shaderVar.length() == 0 ) return -1;
    for ( GLuint i = 0 ; i < m_sUniform.size(); i++) {
        if ( shaderVar.compare(m_sUniform[i]) == 0)
            return mUniformHandles[i];
    }
    return -1;
}

GLint ShaderHandler::getProgramID() {
    return mProgramID;
}

GLboolean ShaderHandler::isAttirb(string& attribVar) {

    for ( GLuint i = 0 ; i < m_sAttribs.size(); i++) {
        if ( attribVar.compare(m_sAttribs[i]) == 0) return true;
    }
    return false;
}

GLboolean ShaderHandler::isUniform(string& uinformVar) {
    for ( GLuint i = 0 ; i < m_sUniform.size(); i++) {
        if ( uinformVar.compare(m_sUniform[i]) == 0) return true;
    }
    return false;
}

GLvoid ShaderHandler::setAttribs(string& name, GLint size, GLint type, GLboolean normalized, GLint stride, GLvoid *ptr) {
    // Pass the vertex attributes to shaders
    if ( ptr == NULL) return;
    GLint index = getAttribHandle(name);
    if ( index != -1 ) {
        glVertexAttribPointer ( index, size, type, normalized, stride, ptr ) ;
        glEnableVertexAttribArray(index);
    }
}

GLvoid ShaderHandler::unsetAttribs() {
    for ( GLuint i=0; i < mAttribHandles.size(); i++)
        glDisableVertexAttribArray(mAttribHandles[i]);
}

GLvoid ShaderHandler::setUniforms(string& name, GLfloat value) {
    GLint location = getUniformHandle(name);
    glUniform1f(location, value);
}

GLvoid ShaderHandler::setUniforms(string& name, GLint count, GLboolean transpose, GLfloat* value) {
    GLint location = getUniformHandle(name);
    glUniformMatrix4fv(location, count, transpose, value);
}

GLvoid ShaderHandler::setUniformsMat2f(string& name, GLint count, GLboolean transpose, GLfloat* value) {
    GLint location = getUniformHandle(name);
    glUniformMatrix2fv(location, count, transpose, value);
}


GLvoid ShaderHandler::useShader() {
    glUseProgram(mProgramID);
}
