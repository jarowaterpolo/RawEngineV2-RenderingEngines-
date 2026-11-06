//
// Created by jaron on 05/11/2026.
//

#ifndef RAWENGINE_INPUTHANDLER_H
#define RAWENGINE_INPUTHANDLER_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Camera.h"

namespace core {
    class InputHandler {
    private:
        GLFWwindow* window = nullptr;
        Camera* camera = nullptr;
        bool inWindow;
        bool holdingRMB;
        double lastX;
        double lastY;
        bool firstMouse;
        float movementSpeed;
        float speedMultiplier;

        //https://www.youtube.com/watch?v=EE5cS8EMT78 Cursor & Mouse callbacks
        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
        static void cursorEnterCallback(GLFWwindow *window, int entered);
        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

    public:
        InputHandler();
        void SetWindow(GLFWwindow* window);
        void SetCamera(Camera* camera);
        void Input(GLFWwindow *window, float deltaTime);
        void SetupCallBacks();
    };
}


#endif //RAWENGINE_INPUTHANDLER_H
