#pragma once

#ifndef OSUTIL_H
#define OSUTIL_H

#include<string>
#include<vector>

using namespace std;

struct Surface
{
    unsigned char *data;
    unsigned int w,h;
    unsigned int format;
};

class OSUtil
{
public:
    static vector<string> getFileLists(const char *dir_name, const char *file_ext);
    static bool isBMP(const char *filePath);
    static bool LoadBMP(Surface *surface, const char *imagepath);
    static void recycle(Surface*surface);

};
#endif