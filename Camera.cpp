#include "Camera.hpp"

namespace gps {

    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget); //cameraDirection
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));

        //TODO - Update the rest of camera parameters

    }

    void Camera::setPosition(glm::vec3 cameraPosition,glm::vec3 cameraTarget) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = this ->cameraUpDirection;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget); //cameraDirection
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
    }
    void Camera::setTarget(glm::vec3 cameraTarget) {
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget); //cameraDirection
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch(direction) {

		case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            cameraTarget += cameraFrontDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            cameraTarget -= cameraFrontDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition -= cameraRightDirection * speed;
            cameraTarget -= cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget += cameraRightDirection * speed;
            break;
        }
        
        
    }

    
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 direction;
        direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));

        
        cameraFrontDirection = glm::normalize(direction);
        cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
        cameraTarget = cameraPosition + cameraFrontDirection;
    }


    void Camera::preview(float angle) {
        // set the camera
        this->cameraPosition = glm::vec3(0.0, 15.0, 30.0);

        // rotate with specific angle around Y axis
        glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

        // compute the new position of the camera 
        // previous position * rotation matrix
        this->cameraPosition = glm::vec4(r * glm::vec4(this->cameraPosition, 1));
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

   
}