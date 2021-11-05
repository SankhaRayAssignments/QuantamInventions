#pragma once
#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "CFileIO.h"
#include "CCamera.h"
#include "CShaderHandler.h"

#define DEFAULT_Y_POS 0.0f

class IRenderObjects
{
public:
    IRenderObjects()
    {
        mb_Draw = true;
    }
    virtual void LoadScene()                 =0;
    virtual void DrawScene(CCamera *cam)      =0;
    virtual void DestroyScene()              =0;
    virtual bool LoadDataFromFile()          =0;
    void toggleDraw()
    {
        mb_Draw ^= true;
    }


protected:
    string mFilePath;
    bool mbDataLoaded;
    // Shaders
    ShaderHandler *mShader;

    bool mb_Draw;
};
