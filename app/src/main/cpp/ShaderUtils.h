//
// Created by apple on 2020/9/21.
//

#ifndef FACEU_SHADERUTILS_H
#define FACEU_SHADERUTILS_H
#include <GLES2/gl2.h>
#include <malloc.h>
class ShaderUtils {
public:
    GLuint createProgram(const char *vertexSource, const char *fragmentSource);

    GLuint loadShader(GLenum shaderType, const char *source);
};


#endif //FACEU_SHADERUTILS_H
