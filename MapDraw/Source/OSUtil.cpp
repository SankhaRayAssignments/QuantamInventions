/**
* @file OSUtil.cpp
* @brief OS functionalities
*
* This file helps to return files from a directoty, loading & checking of bmp files
*/

/*********************************************************************/

#include "OSUtil.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <errno.h>
#include <string.h>

bool OSUtil::isBMP(const char *imagepath)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header

    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file)
    {
        printf("\nImage could not be opened: Image path %s\n", imagepath);
        return false;
    }

    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }

    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return false;
    }

    //Everything is in memory now, the file can be closed
    fclose(file);

    return true;
}

bool OSUtil::LoadBMP(Surface* surface, const char *imagepath)
{
    //if ( __DEBUG__ ) printf("\nInside TexHandler::LoadBMP ==> Image Path : [%s]", imagepath);

    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3

    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file)
    {
        printf("\nImage could not be opened: Image path %s\n", imagepath);
        return false;
    }

    if ( fread(header, 1, 54, file)!= 54 ){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }

    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return false;
    }
    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    surface->data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(surface->data, 1, imageSize,file);

    // Convert BGR --> RGB
    for ( unsigned int i =0; i < imageSize; i+=3 ) {
        unsigned char b = surface->data[i + 0];     //R
        unsigned char r = surface->data[i + 2];     //G
        surface->data[i + 0] = r;
        surface->data[i + 2] = b;
    }

    //Everything is in memory now, the file can be closed
    fclose(file);

    surface->w = width;
    surface->h = height;
    //surface->format = GL_RGB;

    return true;
}

void OSUtil::recycle(Surface* surface)
{
    if ( surface->data )
        delete surface->data;
    surface->data = NULL;
}