//
// Created by jaron on 05/11/2026.
//

#include "InputHandler.h"
#include <cstdio>

namespace core {
    InputHandler::InputHandler()
    {
        inWindow = false;
        holdingRMB = false;
        lastX = 0.0;
        lastY = 0.0;
        firstMouse = true;

        movementSpeed = 10.0f;
        speedMultiplier = 1.0f;
    }
    void InputHandler::SetWindow(GLFWwindow* window)
    {
        this->window = window;
        glfwSetWindowUserPointer(window, this);
    }
    void InputHandler::SetCamera(Camera* camera)
    {
        this->camera = camera;
    }
    void InputHandler::Input(GLFWwindow *window, float deltaTime)
    {
        float speed = movementSpeed * speedMultiplier * deltaTime;
        glm::vec3 translation(0.0f);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            translation += camera->GetForward() * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            translation -= camera->GetForward() * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            translation -= camera->GetRight() * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            translation += camera->GetRight() * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            translation += camera->GetUp() * speed;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            translation -= camera->GetUp() * speed;
        }

        if (translation != glm::vec3(0.0f)) {
            camera->transform.Translate(translation);
            camera->target += translation;
        }

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            if (camera->FOV > 30) {
                camera->FOV -= .05f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            if (camera->FOV < 120) {
                camera->FOV += .05f;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            speedMultiplier = 2.5f;
        }
        else {
            speedMultiplier = 1.0f;
        }
    }
    void InputHandler::SetupCallBacks()
    {
        //JJN get cursor pos setter
        glfwSetCursorPosCallback(window, cursorPosCallback);
        //disable cursor
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //when cursor enter setter
        glfwSetCursorEnterCallback(window, cursorEnterCallback);
        //mouse button setter
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        //sticky key keep track of a button press that might have happend in the same poll
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
    }
    //input get callback camera rotation
    void InputHandler::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
        if (!handler) return;

        if (handler->inWindow && handler->holdingRMB) {
            if (handler->firstMouse) {
                handler->lastX = xpos;
                handler->lastY = ypos;
                handler->firstMouse = false;
            }

            // printf("cursorPosCallback\n");
            // printf("pos = (%f,%f)\n", xpos, ypos);

            float xoffset = handler->lastX - xpos;
            float yoffset = handler->lastY - ypos;

            handler->lastX = xpos;
            handler->lastY = ypos;

            float sensitivity =
                0.2f;
            // 1.0f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            // printf("trying to rotate %f degrees on y and %f degrees on x\n", xoffset, yoffset);
            handler->camera->RotateOnX(yoffset);
            handler->camera->RotateOnY(xoffset);
        }
        else {
            handler->firstMouse = true;
        }
    }
    void InputHandler::cursorEnterCallback(GLFWwindow *window, int entered)
    {
        auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
        if (!handler) return;

        if (entered) {
            // printf("entered window\n");
            handler->inWindow = true;
        }
        else {
            // printf("left window\n");
            handler->inWindow = false;
        }
    }
    void InputHandler::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        auto* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
        if (!handler) return;

        if (handler->inWindow) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
                // printf("right mouse button pressed\n");
                handler->holdingRMB = true;
            }

            if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
                // printf("right mouse button released\n");
                handler->holdingRMB = false;
            }
        }
    }
}