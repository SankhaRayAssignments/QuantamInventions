#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "math_helper.h"

class CCamera
{
public:

    CCamera(float speed);
    void InitCamera(bool topview=false);
    void setPerspective(float fov, float w, float h, float near, float far);
    void setOrtho(float left, float right,float bottom, float top, float zNear, float zFar);
    glm::mat4 getCameraMatrix();
    glm::mat4 getViewMatrix();
    glm::mat4 getViewCheatMatrix();
    void getCameraMatrix(float *matrix);
    void lookAt(float eyex, float eyey, float eyez, float centerx, float centery, float centerz, float upx, float upy, float upz);
    void lookAt(glm::vec3& inEye, glm::vec3& inCenter, glm::vec3& inUp);

    glm::vec3& getEye();
    glm::vec3& getCenter();
    glm::vec3& getUp();

    glm::vec3 getLookVec();
    glm::vec3 getRightVec();

    void setProjectionMatrix(bool topview=false);

    void togggleView();

    bool isTopView();

    float getHeight();

    float getRotation();

    void handleKeyInput(unsigned char key, int x, int y);

private:
    glm::mat4 projection;
    glm::mat4 view;

    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;

    glm::vec3 reset_eye_perspective;
    glm::vec3 reset_center_perspective;
    glm::vec3 reset_up_perspective;

    glm::vec3 reset_eye_ortho;
    glm::vec3 reset_center_ortho;
    glm::vec3 reset_up_ortho;

    float mSpeed;

    float mHeight;
    float mWidth;
    float mZNear;
    float mZFar;
    float mFOV;

    float mLeft;
    float mRight;
    float mBottom;
    float mTop;

    float mRotAngle;

    bool mb_IsTopView;

    bool reachedLimit(glm::vec3& direction);
};

#endif      /*  _CAMERA_H_ */
