#ifndef _mb_DEBUG_H_
#define _mb_DEBUG_H_

#define GL_CHECK() \
    { \
        GLint glError = glGetError(); \
        if(glError != GL_NO_ERROR) { \
            fprintf(stdout, "glGetError() = %i (0x%.8x) at line %i of file %s\n", glError, glError, __LINE__ , __FILE__); \
            printf("glGetError() = %i (0x%.8x) at line %i of file %s\n", glError, glError, __LINE__ , __FILE__); \
        } \
    }

#endif //_mb_DEBUG_H_
