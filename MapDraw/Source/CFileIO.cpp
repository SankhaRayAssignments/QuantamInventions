#include <assert.h>
#include "CFileIO.h"

CFileIO::CFileIO(string filename)
{
    mFileName = filename;
    mStringContent = "";

    mFilePos = 0;
    if ( OpenFile() == false)
    {
        printf("Cant open file %s\n", mFileName.c_str());
        mFileName = "";
        assert(0);
    }
}

CFileIO::~CFileIO(void)
{
    if ( mStringContent != "" )
    {
        free ( mStringContent);
        mStringContent = NULL;
    }
    fclose ( mFile);
}

bool CFileIO::isFileOpened()
{
    return mFileName != "";
}

bool CFileIO::OpenFile()
{
    if ( ( mFile = fopen( mFileName.c_str(), "rb")) == NULL )
        return false;
    else
    {
        fseek ( mFile, mFilePos, SEEK_SET );
        return true;
    }
}
void CFileIO::CloseFile()
{
    mFilePos = ftell ( mFile );
    fclose ( mFile );
}

int CFileIO::GetInt ( string prefix_text )
{
    int numVar;

    if ( prefix_text != "" )
        fseek(mFile, prefix_text.size(), SEEK_CUR);

    fscanf ( mFile, "%d\n",  &numVar );

    return numVar;

}

string CFileIO::GetString ( string prefix_text )
{
    char str[80];

    if ( prefix_text != "" )
        fseek(mFile, prefix_text.size(), SEEK_CUR);

    fscanf ( mFile, "%s\n",  str );

    return string(str);

}

glm::vec3 CFileIO::GetVerts()
{
    glm::vec3 vert;
    float x, y;

    fscanf ( mFile, "%f,%f\n",  &x, &y );
    vert.x = x;
    vert.y = 0;
    vert.z = y;

    return vert;
}

const char *CFileIO::GetContent()
{
    fseek(mFile, 0, SEEK_END);
    long fsize = ftell(mFile);
    fseek(mFile, 0, SEEK_SET);

    mStringContent = (char*)malloc(fsize + 1);
    fread(mStringContent, fsize, 1, mFile);

    mStringContent[fsize] = 0;

    return mStringContent;
}