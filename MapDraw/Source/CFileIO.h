#pragma once
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <string>
using namespace std;
class CFileIO
{
protected :
    FILE *mFile;
    long mFilePos;
    string mFileName;

    char *mStringContent;

public:
    CFileIO(string filename);
    virtual ~CFileIO(void);
    bool OpenFile();
    void CloseFile();

    bool isFileOpened();

    int GetInt ( string prefix_text = "");

    string GetString ( string prefix_text = "" );

    glm::vec3 GetVerts();

    const char *GetContent();
};
