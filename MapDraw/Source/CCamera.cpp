#include "CCamera.h"
#include <string.h>
#include <glm/gtx/rotate_vector.hpp>

static const float MIN_X = -100.0f;
static const float MAX_X = 1500.0f;

static const float MIN_Y =  10.0f;
static const float MAX_Y = 100.0f;

static const float MIN_Z = -500.0f;
static const float MAX_Z =  320.0f;

static const float MIN_RIGHT =   4.0f;      // Orthographics max zoom
static const float MAX_RIGHT = 200.0f;      // Orthographics min zoom

CCamera::CCamera(float speed)
{
    mSpeed = speed;
    mb_IsTopView = false;
    mRotAngle = 0.0f;
}

void CCamera::InitCamera(bool topview)
{
    mb_IsTopView = topview;

    reset_eye_ortho     = glm::vec3(0.00f, 0.00f, 25.0f);
    reset_center_ortho  = glm::vec3(0.00f, 0.00f,-10.0f);
    reset_up_ortho      = glm::vec3(0.00f, 1.00f,  0.00f);

    reset_eye_perspective       = glm::vec3(2.45f, 1.50f, 18.12f);
    reset_center_perspective    = glm::vec3(3.04f, 1.50f, 17.32f);
    reset_up_perspective        = glm::vec3(0.00f, 1.00f,  0.00f);

    if ( mb_IsTopView )
    {
        eye     = reset_eye_ortho;
        center  = reset_center_ortho;
        up      = reset_up_ortho;
    }
    else
    {
        eye     = reset_eye_perspective;
        center  = reset_center_perspective;
        up      = reset_up_perspective;
    }

    // Update View Matrix
    lookAt(
        eye,     // Eye
        center,  // LookAt
        up);     // Up

}

void CCamera::setPerspective(float fov, float w, float h, float zNear, float zFar)
{
    mFOV = fov;
    mWidth = w;
    mHeight = h;
    mZNear  = zNear;
    mZFar   = mZFar;
}
void CCamera::setOrtho(float left, float right,float bottom, float top,
                        float zNear, float zFar)
{
    mLeft   = left;
    mRight  = right;
    mBottom = bottom;
    mTop    = top;
    mZNear  = zNear;
    mZFar   = zFar;
}

void CCamera::setProjectionMatrix(bool topview)
{
    mb_IsTopView = topview;
    if ( !mb_IsTopView )
        projection = glm::perspective(mFOV, mWidth/mHeight, mZNear, mZFar);
    else
    {
        projection = glm::ortho(mLeft, mRight, mBottom, mTop, mZNear, mZFar);
    }
}

void CCamera::lookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz)
{
    eye     = glm::vec3(eyex, eyey, eyez);
    center  = glm::vec3(centerx, centery, centerz);
    up      = glm::vec3(upx, upy, upz);

    view    = glm::lookAt(eye, center, up);
}

void CCamera::lookAt(glm::vec3& inEye, glm::vec3& inCenter, glm::vec3& inUp)
{
    eye     = inEye;
    center  = inCenter;
    up      = inUp;

#ifdef __DEBUG__
    printf("%s:Eye [%0.2f,%0.2f,%0.2f]. Lookat [%0.2f,%0.2f,%0.2f] UP [%0.2f,%0.2f,%0.2f] \n",
        __FUNCTION__,
        eye.x, eye.y, eye.z,
        center.x, center.y, center.z,
        up.x, up.y, up.z);
#endif
    // Update view matrix
    view    = glm::lookAt(eye, center, up);
}

void CCamera::getCameraMatrix(float *matrix){

    glm::mat4 pv;
    pv = projection * view;
    memcpy(matrix, glm::value_ptr(pv), sizeof (float) * 16);
}

glm::mat4 CCamera::getCameraMatrix()
{
    if ( !mb_IsTopView )
        return projection * view;
    else
    {
        glm::mat4 view_01 = glm::rotate(view, 90.0f, 1.0f,0.0f,0.0f);
        return projection * view_01;
    }
}

glm::mat4 CCamera::getViewMatrix()
{
    return view;
}

glm::mat4 CCamera::getViewCheatMatrix()
{
    glm::mat4 cheatView = view;
    for( int i=0; i<3; i++ )
        for( int j=0; j<3; j++ ) {
            if ( i==j )
                cheatView[i][j] = 1.0;
            else
                cheatView[i][j] = 0.0;
        }
        return projection * cheatView;
}

glm::vec3& CCamera::getEye()
{
    return eye;
}
glm::vec3& CCamera::getCenter()
{
    return center;
}
glm::vec3& CCamera::getUp()
{
    return up;
}

bool CCamera::isTopView()
{
    return mb_IsTopView;
}

glm::vec3 CCamera::getLookVec()
{
    return glm::normalize(center - eye);
}

glm::vec3 CCamera::getRightVec()
{
    glm::vec3 lookVec  = glm::normalize(center - eye);
    glm::vec3 rightVec = glm::normalize(glm::cross(lookVec, up));
    return rightVec;
}

float CCamera::getHeight()
{
    return mRight;
}

float CCamera::getRotation()
{
    return mRotAngle;
}

void CCamera::togggleView()
{
    mb_IsTopView ^= true;
    mRotAngle = 0.0f;

    glm::vec3 lookVec  = glm::normalize(center - eye);

    if ( mb_IsTopView ) // Look Down
    {
        eye.z    = reset_eye_ortho.z;
        center.z = reset_center_ortho.z;
    }
    else    // Look up
    {
        center.y = eye.y = reset_eye_perspective.y;
        up       = reset_up_perspective;
    }

    // Update Projection Matrix
    setProjectionMatrix(mb_IsTopView);

    // Compute View Matrix
    lookAt(eye, center, up);
}

bool CCamera::reachedLimit(glm::vec3& direction)
{
    glm::vec3 temp_eye = eye + mSpeed * direction;
    if ( temp_eye.x >= MAX_X ) return true;
    if ( temp_eye.x <= MIN_X ) return true;
    if ( temp_eye.z >= MAX_Z ) return true;
    if ( temp_eye.z <= MIN_Z ) return true;

    return false;
}

void CCamera::handleKeyInput(unsigned char key, int x, int y)
{
    glm::vec3 lookVec  = glm::normalize(center - eye);
    glm::vec3 rightVec = glm::normalize(glm::cross(lookVec, up));
    up                 = glm::normalize(glm::cross(rightVec, lookVec));

    if ( key == 114 || key == 82)           // 'r' pressed
    {
        if ( mb_IsTopView )
        {
            // Rotate Along up vec
            up = glm::rotateZ(up, 1.0f);
        }
        else
        {
            // Rotate the camera lookvec
            center  = eye + glm::rotateY(lookVec, 1.0f);
        }
        mRotAngle += 1.0f;
        if ( mRotAngle >= 360.0f )
            mRotAngle -= 360.0f;
    }
    else if ( key == 116 || key == 84 )      // 't' pressed
    {
        if ( mb_IsTopView )
        {
            // Rotate Along up vec
            up = glm::rotateZ(up, -1.0f);
        }
        else
        {
            // Rotate the camera lookvec
            center  = eye + glm::rotateY(lookVec, -1.0f);
        }
        mRotAngle -= 1.0f;
        if ( mRotAngle <= -360.0f )
            mRotAngle += 360.0f;
    }

    if ( key == 119 || key == 87 )           // 'w' pressed
    {
        if ( mb_IsTopView )
        {
            if ( mRight <= MIN_RIGHT ) return;
            float unit = 1.0f;
            mRight -=   unit;
            mLeft   = -mRight;
            mTop    =  mRight;
            mBottom = -mRight;
            setProjectionMatrix(true);
        }
        else
        {
            if (reachedLimit(lookVec)) return;

            eye     = eye    + mSpeed * lookVec;
            center  = center + mSpeed * lookVec;
        }
    }
    else if ( key == 115 || key == 83 )      // 's' pressed
    {
        if ( mb_IsTopView )
        {
            if ( mRight >= MAX_RIGHT ) return;
            float unit = 1.0f;
            mRight +=   unit;
            mLeft   = -mRight;
            mTop    =  mRight;
            mBottom = -mRight;
            setProjectionMatrix(true);
        }
        else
        {
            if ( reachedLimit(-lookVec)) return;

            eye     = eye    - mSpeed * lookVec;
            center  = center - mSpeed * lookVec;
        }
    }

    if ( key == 97 || key == 65 )            // 'a' pressed
    {
        if ( reachedLimit(-rightVec)) return;

        eye    = eye    - mSpeed * rightVec;
        center = center - mSpeed * rightVec;
    }
    else if ( key == 100 || key == 68 )      // 'd' pressed
    {
        if ( reachedLimit(rightVec)) return;

        eye    = eye    + mSpeed * rightVec;
        center = center + mSpeed * rightVec;
    }

    if ( mb_IsTopView )     // Only in OrthoGraphic
    {
        if ( key == 113 || key == 81 )           // 'q' pressed
        {
            eye    = eye    + mSpeed * up;
            center = center + mSpeed * up;
        }
        else if ( key == 101 || key == 69 )      // 'e' pressed
        {
            eye    = eye    - mSpeed * up;
            center = center - mSpeed * up;
        }
    }

    // Update the View Matrix
    lookAt(
        eye,        // Eye
        center,     // LookAt
        up);        // Up
}