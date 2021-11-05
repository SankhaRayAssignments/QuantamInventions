#ifndef ISHADERHANDLER_H
#define ISHADERHANDLER_H

#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <string>
#include <iostream>
#include <vector>
using namespace std;

class ShaderHandler {
public:

    ShaderHandler (string vertShader, string fragShader, vector<string> attribList, vector<string> uniformList);

    GLvoid InitShader(string vertShader, string fragShader, vector<string> attribList, vector<string> uniformList);

    const char *GetSource(string path);

    GLint getAttribHandle(string& shaderVar);
    GLint getUniformHandle(string& shaderVar);

    GLint getProgramID();

    GLboolean isAttirb(string& attribVar);
    GLboolean isUniform(string& uinformVar);

    GLvoid setAttribs(string& name, GLint size, GLint type, GLboolean normalized, GLint stride, GLvoid* ptr);
    GLvoid unsetAttribs();

    GLvoid setUniforms(string& name, GLfloat value);
    GLvoid setUniforms(string& name, GLint count, GLboolean transpose, GLfloat* value);
    GLvoid setUniformsMat2f(string& name, GLint count, GLboolean transpose, GLfloat* value);

    GLvoid useShader();

    GLvoid DetachShaders();

protected:

    GLint mProgramID;
    GLuint mShaderID[2];

    vector<GLint> mAttribHandles;
    vector<GLint> mUniformHandles;

    vector<string> m_sAttribs;
    vector<string> m_sUniform;

private:
    GLuint CompileShader(string path, GLuint type);
};

#endif