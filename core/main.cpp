#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "mesh.h"
#include "assimpLoader.h"
#include "texture.h"
#include "Camera.h"
#include "LightObject.h"
#include "InputHandler.h"
#include "modelRenderer.h"

//#define MAC_CLION
#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef VSTUDIO
// Note: install imgui with:
//     ./vcpkg.exe install imgui[glfw-binding,opengl3-binding]
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

int g_width = 800;
int g_height = 600;

//JJN Vars
float aspectRatio = float(g_width) / float(g_height);
core::Camera camera(aspectRatio);
glm::vec3 lightPos = glm::vec3(0.0f, 3.0f, 30.0f);
glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
core::LightObject lightObject(lightPos, lightColor);
GLenum drawMode = GL_TRIANGLES;
float colorMult = 0.0f;
glm::vec4 uniformColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
glm::vec4 uniformColor2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
float deltaTime = 0.0f;
float modelSpecularIntensity = 1.0f;
core::InputHandler inputHandler;
core::modelRenderer model_renderer(0, &lightObject, &camera, aspectRatio, &drawMode);

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //JJNInput
    inputHandler.Input(window, deltaTime);
}
void framebufferSizeCallback(GLFWwindow *window,
                             int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
    aspectRatio = float(g_width) / float(g_height);
}

std::string readFileToString(const std::string &filePath) {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        printf("Could not open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}
GLuint generateShader(const std::string &shaderPath, GLuint shaderType) {
    printf("Loading shader: %s\n", shaderPath.c_str());
    const std::string shaderText = readFileToString(shaderPath);
    const GLuint shader = glCreateShader(shaderType);
    const char *s_str = shaderText.c_str();
    glShaderSource(shader, 1, &s_str, nullptr);
    glCompileShader(shader);
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
    }
    return shader;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    inputHandler.SetCamera(&camera);
    inputHandler.SetWindow(window);
    inputHandler.SetupCallBacks();

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Setup platforms
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 400");

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const GLuint modelVertexShader = generateShader("shaders/modelVertex.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader = generateShader("shaders/fragment.fs", GL_FRAGMENT_SHADER);
    const GLuint textureShader = generateShader("shaders/texture.fs", GL_FRAGMENT_SHADER);

    int success;
    char infoLog[512];
    const unsigned int modelShaderProgram = glCreateProgram();
    glAttachShader(modelShaderProgram, modelVertexShader);
    glAttachShader(modelShaderProgram, fragmentShader);
    glLinkProgram(modelShaderProgram);
    glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }
    const unsigned int textureShaderProgram = glCreateProgram();
    glAttachShader(textureShaderProgram, modelVertexShader);
    glAttachShader(textureShaderProgram, textureShader);
    glLinkProgram(textureShaderProgram);
    glGetProgramiv(textureShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(textureShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);

    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({quad});
    quadModel.translate(glm::vec3(0,0,-2.5));
    quadModel.scale(glm::vec3(5, 5, 1));
    // quadModel.rotate(glm::vec3(0,0,1), glm::radians(180.0f));

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");

    //my own model
    // core::Model cube = core::AssimpLoader::loadModel("models/RandoCubeForRenderingEngines2.obj");
    core::Model cube = core::AssimpLoader::loadModel("models/RandoCubeForRenderingEnginesSmooth.obj");
    core::Model lightSphere = core::AssimpLoader::loadModel("models/LightTestSphere.obj");
    // lightSphere.scale(glm::vec3(5, 5, 1));

    // camera.RotateOnX(45);
    // camera.RotateOnY(45);
    camera.UpdateCameraVectors();

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);

    GLint colorMultUniformLocation = glGetUniformLocation(modelShaderProgram, "colorMult");
    GLint uniformColorLocation = glGetUniformLocation(modelShaderProgram, "uniformColor");

    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint mMatrixUniform = glGetUniformLocation(modelShaderProgram, "mMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    //lighting
    GLint light0colorUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].color");
    GLint light0positionUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].position");
    GLint light0ambientIntensityUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].ambientIntensity");
    GLint light0cameraPositionUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].cameraPos");
    GLint light0specularIntensityUniformLocation = glGetUniformLocation(modelShaderProgram, "lights[0].specularIntensity");


    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float rotationStrength = 100.0f;
    double Xpos, Ypos;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Raw Engine v2");
        ImGui::Text("Hello :)");
        ImGui::Text("Im Jaro");
        // if (ImGui::Button("Make something appear", ImVec2(200, 60)))
        // {
        //     printf("Clicked\n");
        //     if (colorMult < 1.0f) {
        //     colorMult += 0.01f;
        //     }
        // };
        //wireframe button
        if (ImGui::Button("Toggle Wireframe", ImVec2(120, 60))) {
            if (drawMode == GL_TRIANGLES) {
                drawMode = GL_LINES;
            }
            else {
                drawMode = GL_TRIANGLES;
            }
        }
        ImGui::End();

        ImGui::Begin("Monke");
        if (ImGui::ColorPicker3("Color_Monkey", glm::value_ptr(uniformColor))) {

        }
        if (ImGui::DragFloat ("Specular_Intensity_Monke", &modelSpecularIntensity, 1)) {

        }
        ImGui::End();

        ImGui::Begin("My Model");
        if (ImGui::ColorPicker3("Color_My_Model", glm::value_ptr(uniformColor2))) {

        }
        ImGui::End();

        ImGui::Begin("My Light");
        if (ImGui::DragFloat3("Light Position:", glm::value_ptr(lightObject.transform.position))) {
            model_renderer.UpdateLightObject(&lightObject);
        }
        ImGui::End();


        processInput(window);
        suzanne.rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        cube.rotate(glm::vec3(0.0f, 1.0f, 1.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));

        glfwGetCursorPos(window, &Xpos, &Ypos);


        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix(aspectRatio) * camera.GetViewMatrix() * quadModel.getModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        quadModel.render(drawMode);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);


        //first model(monke)
        glm::mat4 offsetMatrix = glm::mat4(1,0,0,0,
                                            0,1,0,0,
                                            0,0,1,0,
                                            0,0,0,1);

        model_renderer.SetModelShaderProgram(modelShaderProgram);
        model_renderer.UpdateLightObject(&lightObject);

        model_renderer.DrawModel(suzanne, uniformColor, offsetMatrix,
        uniformColorLocation,light0colorUniformLocation, light0positionUniformLocation,
        light0ambientIntensityUniformLocation, light0cameraPositionUniformLocation, light0specularIntensityUniformLocation,
        modelSpecularIntensity ,mMatrixUniform ,mvpMatrixUniform);

        offsetMatrix[3] += glm::vec4(3,0,1,0);

        model_renderer.DrawModel(suzanne, uniformColor, offsetMatrix,
        uniformColorLocation,light0colorUniformLocation, light0positionUniformLocation,
        light0ambientIntensityUniformLocation, light0cameraPositionUniformLocation, light0specularIntensityUniformLocation,
        256.0f ,mMatrixUniform ,mvpMatrixUniform);

        glBindVertexArray(0);

        offsetMatrix[3] += glm::vec4(2,0,0,0);

        // my own added model and shaders
        model_renderer.DrawModel(lightSphere, uniformColor, offsetMatrix,
            uniformColorLocation, light0colorUniformLocation, light0positionUniformLocation,
            light0ambientIntensityUniformLocation, light0cameraPositionUniformLocation, light0specularIntensityUniformLocation,
            modelSpecularIntensity, mMatrixUniform, mvpMatrixUniform);

        glUseProgram(modelShaderProgram);
        glUniform1f(colorMultUniformLocation, colorMult);
        glUniform4f(uniformColorLocation, uniformColor2.r, uniformColor2.g, uniformColor2.b, uniformColor2.a);
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix(aspectRatio) * camera.GetViewMatrix() * cube.getModelMatrix() * offsetMatrix));
        cube.render(drawMode);
        // glBindVertexArray(0);

        offsetMatrix[3] += glm::vec4(3,0,0,1);

        model_renderer.DrawModel(lightSphere, uniformColor2, offsetMatrix,
            uniformColorLocation,light0colorUniformLocation, light0positionUniformLocation,
            light0ambientIntensityUniformLocation, light0cameraPositionUniformLocation, light0specularIntensityUniformLocation,
            modelSpecularIntensity, mMatrixUniform, mvpMatrixUniform);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}