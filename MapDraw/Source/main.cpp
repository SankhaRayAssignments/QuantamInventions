#pragma once
#include <windows.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "CRoadsGL.h"
#include "CFeaturesGL.h"
#include "CPOIsGL.h"
#include "CBuildingsGL.h"
//#include "CQuad.h"
#include "CCamera.h"

//#define DISABLE_TEXT_RENDERING    // Temporary Disables Text Rendering
//#define TOP_VIEW      // OrthoGraphics Projection

static CRoadsGL     g_Roads      ("data/roads.txt");
static CFeaturesGL  g_Features   ("data/features.txt");
static CPOIsGL      g_POIs       ("data/poi.txt");
static CBuildingsGL g_Buildings  ("data/buildings.txt");
static CRoadsGL     g_Routed_Path("data/routed_path.txt");
//static CQuad      g_Terrain    (1000, -15.0f);

static CCamera *g_Camera;

static float g_Width  = 1280;
static float g_Height = 720;

static float g_WinX   =  0;
static float g_WinY   =  0;

static bool g_Fullscreen = false;

/* Function Declarations */
static void RenderScene(void) ;
static void CalculateFramePerSec ( GLfloat fElapsedTime );
#ifndef DISABLE_TEXT_RENDERING
static void RenderBitmapString(float x, float y, void *font,char *string);
static void RenderText ();
#endif
static void LoadScene(void) ;
static void DestroyScene(void) ;
static void ProcessKeyBoard(unsigned char key, int x, int y);
static void ProcessSpecialKeyBoard(int key, int x, int y);
static void ProcessMouse(int button, int state, int x, int y);
static void ProcessRelativeMouse ( int x, int y );
static void ProcessMouseMotion ( int x, int y, int z );

static GLvoid InitGLUT(GLsizei width, GLsizei height);
static GLvoid EstablishProjectionMatrix(GLsizei width, GLsizei height);
static GLvoid TimerFunction ( GLint delay );
static GLvoid OnFrameMove ( GLfloat fElapsedTime );

static void LoadDataFromFile()
{
    g_Buildings.LoadDataFromFile();
    g_Features.LoadDataFromFile();
    g_POIs.LoadDataFromFile();

    glm::vec4 color(142,121,106,255);
    color /= 255.0f;
    g_Roads.setColor(color);
    g_Roads.setWidth(3.0f);
    g_Roads.LoadDataFromFile();

    color = glm::vec4(71,244,62,255);
    color /= 255.0f;
    g_Routed_Path.setColor(color);
    g_Routed_Path.setWidth(1.5f);
    g_Routed_Path.LoadDataFromFile();

    //g_Terrain.LoadDataFromFile();
}

static GLvoid InitGLUT(GLsizei width, GLsizei height)
{
    glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA );

    glutInitWindowSize( width, height );
    glutInitWindowPosition( g_WinX, g_WinY);
    glutCreateWindow( "MapDraw" );

    glutDisplayFunc( RenderScene );
    glutIdleFunc( RenderScene );
    glutReshapeFunc( EstablishProjectionMatrix );
    //  glutTimerFunc ( TIMER_DELAY, TimerFunction, 0 );

    glutKeyboardFunc( ProcessKeyBoard );
    glutSpecialFunc ( ProcessSpecialKeyBoard );
    glutMouseFunc( ProcessMouse );
    glutPassiveMotionFunc ( ProcessRelativeMouse );
    glutSpaceballRotateFunc  ( ProcessMouseMotion );

    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
        printf("Ready for OpenGL 2.0\n");
    else {
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }

    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
        printf("Ready for GLSL\n");
    else {
        printf("Not totally ready :( \n");
        exit(1);
    }
}

static GLvoid EstablishProjectionMatrix(GLsizei width, GLsizei height)
{
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(height == 0) height = 1;

    g_Width = width;
    g_Height = height;
    glViewport(0, 0, width, height);        // Set the Viewport

    GLfloat fov     = 45.0f;
    GLfloat aspect  = GLfloat(width)/height;
    GLfloat zn      = 1.0f;
    GLfloat zf      = 75.0f;
    GLfloat unit    = 20.0f;

    g_Camera->setPerspective(45, width, height, zn, zf);
    g_Camera->setOrtho(-unit,unit,-unit,unit, zn, zf);

    // Set Projection Matrix
    g_Camera->setProjectionMatrix(g_Camera->isTopView());
}

static void LoadScene(void)
{
    g_Buildings.LoadScene();
    g_Features.LoadScene();
    g_Roads.LoadScene();
    g_Routed_Path.LoadScene();
    g_POIs.LoadScene();

    //g_Terrain.LoadScene();

#ifdef TOP_VIEW
    g_Camera->InitCamera(true);
#else
    g_Camera->InitCamera(false);
#endif
}

static void RenderScene(void)
{
    glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glShadeModel(GL_SMOOTH);
    //  glShadeModel(GL_FLAT );

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Perspective Nicest Calculation
    glEnable(GL_PERSPECTIVE_CORRECTION_HINT);           // Enable

    glEnable(GL_DEPTH_TEST);                            // Enable Z - Enable
    glDepthFunc(GL_LEQUAL);                             // Pixel Depth

    glEnable ( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glFrontFace ( GL_CCW );

    //g_Terrain.DrawScene(g_Camera);
    g_Features.DrawScene(g_Camera);
    g_Roads.DrawScene(g_Camera);
    g_Routed_Path.DrawScene(g_Camera);
    g_Buildings.DrawScene(g_Camera);
    g_POIs.DrawScene(g_Camera);

    glDisable( GL_CULL_FACE );
    glDisable(GL_DEPTH_TEST);

#ifndef DISABLE_TEXT_RENDERING
    // Draw Text Information
    RenderText();
#endif

    glFinish();

    // Swap EGL Buffers
    glutSwapBuffers();
}

static void DestroyScene(void)
{
    g_Buildings.DestroyScene();
    g_Features.DestroyScene();
    g_Roads.DestroyScene();
    g_Routed_Path.DestroyScene();
    g_POIs.DestroyScene();
    //g_Terrain.DestroyScene();

    delete g_Camera;
}

#ifndef DISABLE_TEXT_RENDERING
static void RenderText ()
{
    // switch to projection mode
    glMatrixMode(GL_PROJECTION);
    // save previous matrix which contains the
    //settings for the perspective projection
    glPushMatrix();
    // reset matrix
    glLoadIdentity();
    // set a 2D orthographic projection
    gluOrtho2D(0, g_Width , 0, g_Height);
    // invert the y axis, down is positive
    glScalef(1, -1, 1);
    // mover the origin from the bottom left corner
    // to the upper left corner
    glTranslatef(0, -g_Height, 0);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    glLoadIdentity();

    int font=(int)GLUT_BITMAP_TIMES_ROMAN_24;
    int x = 10;
    int y = 20;
    char buff[80];
    sprintf_s(buff, 80 * sizeof(char), "Current Camera View: %s", g_Camera->isTopView() ? "TOP VIEW(2D)":"FRONT VIEW(3D)");
    RenderBitmapString(x, y, (void *)font, buff);

    // Display Controls
    font=(int)GLUT_BITMAP_8_BY_13;
    y += 40;
    sprintf_s(buff, sizeof(buff), "F6  - Change Camera View (2D/3D)");
    RenderBitmapString(x, y, (void *)font, buff);

    if ( !g_Camera->isTopView() ) {
        y += 20;
        sprintf_s(buff, sizeof(buff), "W/S - Move Front/Back");
        RenderBitmapString(x, y, (void *)font, buff);
        y += 20;
        sprintf_s(buff, sizeof(buff), "A/D - Move Left/Right");
        RenderBitmapString(x, y, (void *)font, buff);
    }
    else
    {
        y += 20;
        sprintf_s(buff, sizeof(buff), "W/S - Zoom In/Out");
        RenderBitmapString(x, y, (void *)font, buff);
        y += 20;
        sprintf_s(buff, sizeof(buff), "A/D - Pan Left/Right");
        RenderBitmapString(x, y, (void *)font, buff);
        y += 20;
        sprintf_s(buff, sizeof(buff), "Q/E - Pan Up/Down");
        RenderBitmapString(x, y, (void *)font, buff);
    }
    y += 20;
    sprintf_s(buff, sizeof(buff), "R/T - Rotate ClockWise/AntiClockWise");
    RenderBitmapString(x, y, (void *)font, buff);
    y += 20;
    sprintf_s(buff, sizeof(buff), "ESC - EXIT");
    RenderBitmapString(x, y, (void *)font, buff);

    // Display Copyright
    sprintf_s(buff, sizeof(buff), "%c Copyright : SANKHA RAY (saheb_ray85@yahoo.co.in) - APR 2016", 184);
    x = g_Width - 8*strlen(buff);
    y = g_Height - 20;
    RenderBitmapString(x, y, (void *)font, buff);

    glPopMatrix();
    // set the current matrix to GL_PROJECTION
    glMatrixMode(GL_PROJECTION);
    // restore previous settings
    glPopMatrix();
    // get back to GL_MODELVIEW matrix
    glMatrixMode(GL_MODELVIEW);
}

static void RenderBitmapString(float x, float y, void *font,char *string)
{
    char *c;
    // set position to start drawing fonts
    glRasterPos2f(x, y);
    glColor4f(0.0f,0.0f,0.0f,1.0f);

    // loop all the characters in the string
    for (c=string; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}
#endif

static void ProcessMouse(int button, int state, int x, int y)
{
}
static void ProcessMouseMotion ( int x, int y, int z )
{
}
static void ProcessRelativeMouse ( int x, int y )
{
}

static void ProcessKeyBoard(unsigned char key, int x, int y)
{
#ifdef __DEBUG__
    //int KeyModifiers = glutGetModifiers();
    //printf("Inside ProcessKeyBoard %d\n", KeyModifiers);

    printf("Inside ProcessKeyBoard %d\n", key);
#endif
    if ( key == 27)                 // EXIT
        exit(0);

    g_Camera->handleKeyInput(key, x,y);
}

static void ProcessSpecialKeyBoard(int key, int x, int y)
{
#ifdef __DEBUG__
    printf("Key %d\n", key);

    if ( key == 101 )               // Up Arrow
        g_Features.increaseCount();
    else if ( key == 103 )          // Down Arrow
        g_Features.decreaseCount();

    if ( key == 1)                  // F1
        g_Buildings.toggleDraw();

    if ( key == 2)                  // F2
        g_Features.toggleDraw();

    if ( key == 3)                  // F3
        g_Roads.toggleDraw();

    if ( key == 4)                  // F4
        g_Routed_Path.toggleDraw();

    if ( key == 5)                  // F5
        g_POIs.toggleDraw();

    if ( key == 7 )
    {
        g_Fullscreen ^= true;

        if ( g_Fullscreen )
        {
            glutFullScreen();
            glutInitWindowSize( glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT) );
            //glutInitWindowPosition( g_WinX, g_WinY);
        }
        else
        {
            glutReshapeWindow ( g_Width, g_Height );
            glutPositionWindow( g_WinX,  g_WinY);
        }
    }
#endif

    if ( key == 6)                  // F6
        g_Camera->togggleView();
}

int main(int argc, char **argv)
{
    LoadDataFromFile();

    g_Camera = new CCamera(1.0f);

    glutInit(&argc, argv);

    InitGLUT (g_Width, g_Height);

    LoadScene();

    glutMainLoop();

    DestroyScene();

    return 0;
}