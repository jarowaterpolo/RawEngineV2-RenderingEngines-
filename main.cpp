#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "core/mesh.h"
#include "core/assimpLoader.h"
#include "core/texture.h"
#include "core/Camera.h"
#include "core/LightObject.h"

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
Camera camera(aspectRatio);
glm::vec3 lightPos = glm::vec3(0.0f, 3.0f, 30.0f);
glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
LightObject lightObject(lightPos, lightColor);
bool inWindow = false;
bool holdingRMB = false;
static double lastX = 0.0;
static double lastY = 0.0;
static bool firstMouse = true;
GLenum drawMode = GL_TRIANGLES;
float colorMult = 0.0f;
glm::vec4 uniformColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
glm::vec4 uniformColor2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
float deltaTime = 0.0f;
float modelSpecularIntensity = 1.0f;

//https://www.youtube.com/watch?v=EE5cS8EMT78 Cursor & Mouse callbacks
static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
void cursorEnterCallback(GLFWwindow *window, int entered);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void DrawModel(const unsigned int modelShaderProgram, core::Model model, glm::mat4 offsetMatrix, GLint colorMultUniformLocation, GLint uniformColorLocation,
    GLint light0colorUniformLocation, GLint light0positionUniformLocation, GLint light0ambientIntensityUniformLocation,
    GLint light0cameraPositionUniformLocation, GLint light0specularIntensityUniformLocation, GLint mvpMatrixUniform);
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //JJNInput
    //camera
    camera.Input(window, deltaTime);
}
void framebufferSizeCallback(GLFWwindow *window,
                             int width, int height) {
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
    aspectRatio = float(g_width) / float(g_height);
    camera.UpdateAspectRatio(aspectRatio);
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
    //my own added shaders
    const GLuint modelVertexShader2 = generateShader("shaders/modelVertex2.vs", GL_VERTEX_SHADER);
    const GLuint fragmentShader2 = generateShader("shaders/fragment2.fs", GL_FRAGMENT_SHADER);


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
    //my own added shaders
    const unsigned int modelShaderProgram2 = glCreateProgram();
    glAttachShader(modelShaderProgram2, modelVertexShader2);
    glAttachShader(modelShaderProgram2, fragmentShader2);
    glLinkProgram(modelShaderProgram2);
    glGetProgramiv(modelShaderProgram2, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(modelShaderProgram2, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);
    //my own added shaders
    glDeleteShader(modelVertexShader2);
    glDeleteShader(fragmentShader2);

    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({quad});
    quadModel.translate(glm::vec3(0,0,-2.5));
    quadModel.scale(glm::vec3(5, 5, 1));
    // quadModel.rotate(glm::vec3(0,0,1), glm::radians(180.0f));

    core::Model suzanne = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");

    //my own model
    core::Model cube = core::AssimpLoader::loadModel("models/RandoCubeForRenderingEngines2.obj");
    core::Model lightSphere = core::AssimpLoader::loadModel("models/LightTestSphere.obj");

    // camera.RotateOnX(45);
    // camera.RotateOnY(45);
    camera.UpdateCameraVectors();

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);

    GLint colorMultUniformLocation = glGetUniformLocation(modelShaderProgram, "colorMult");
    GLint uniformColorLocation = glGetUniformLocation(modelShaderProgram, "uniformColor");
    GLuint uniformColorLocation2 = glGetUniformLocation(modelShaderProgram2, "uniformColor");

    GLint mvpMatrixUniform = glGetUniformLocation(modelShaderProgram, "mvpMatrix");
    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    //my own added shaders + model
    GLint mvpMatrixUniform2 = glGetUniformLocation(modelShaderProgram2, "mvpMatrix2");

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
        if (ImGui::Button("Make something appear", ImVec2(200, 60)))
        {
            printf("Clicked\n");
            if (colorMult < 1.0f) {
            colorMult += 0.01f;
            }
        };
        //wireframe button
        if (ImGui::Button("Toggle Wireframe", ImVec2(120, 60))) {
            if (drawMode == GL_TRIANGLES) {
                drawMode = GL_LINES;
            }
            else {
                drawMode = GL_TRIANGLES;
            }
        }

        if (ImGui::ColorPicker3("Color_Monkey", glm::value_ptr(uniformColor))) {

        }
        if (ImGui::ColorPicker3("Color_My_Model", glm::value_ptr(uniformColor2))) {

        }
        if (ImGui::DragFloat ("Specular_Intensity_Monke", &modelSpecularIntensity, 1)) {

        }
        ImGui::End();

        processInput(window);
        suzanne.rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        cube.rotate(glm::vec3(0.0f, 1.0f, 1.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));

        camera.LookAt();
        glfwGetCursorPos(window, &Xpos, &Ypos);


        glUseProgram(textureShaderProgram);
        glUniformMatrix4fv(textureModelUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix() * camera.GetViewMatrix() * quadModel.getModelMatrix()));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureUniform, 0);
        glBindTexture(GL_TEXTURE_2D, cmgtGatoTexture.getId());
        quadModel.render(drawMode);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);

        glUseProgram(modelShaderProgram);
        glUniform1f(colorMultUniformLocation, colorMult);
        glUniform4f(uniformColorLocation, uniformColor.r, uniformColor.g, uniformColor.b, 1);
        //light
        glUniform4fv(light0colorUniformLocation, 1, &lightObject.GetColor()[0]);
        // printf("lightObject color = (%f,%f,%f,%f) \n", lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        // printf("%i set to color (%f,%f,%f,%f)\n", light0colorUniformLocation, lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        glUniform3fv(light0positionUniformLocation, 1, &lightPos[0]);
        glUniform1f(light0ambientIntensityUniformLocation, 0.2f);
        glUniform3fv(light0cameraPositionUniformLocation, 1, &camera.GetPos()[0]);
        glUniform1f(light0specularIntensityUniformLocation, modelSpecularIntensity);

        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix() * camera.GetViewMatrix() * suzanne.getModelMatrix()));
        suzanne.render(drawMode);
        // glBindVertexArray(0);

        glm::mat4 offsetMatrix = glm::mat4(1,0,0,0,
                                            0,1,0,0,
                                            0,0,1,0,
                                            3,0,0,1);

        glUseProgram(modelShaderProgram);
        glUniform1f(colorMultUniformLocation, colorMult);
        glUniform4f(uniformColorLocation, uniformColor.r, uniformColor.g, uniformColor.b, uniformColor.a);
        //light
        glUniform4fv(light0colorUniformLocation, 1, &lightObject.GetColor()[0]);
        // printf("lightObject color = (%f,%f,%f,%f) \n", lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        // printf("%i set to color (%f,%f,%f,%f)\n", light0colorUniformLocation, lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        glUniform3fv(light0positionUniformLocation, 1, &lightPos[0]);
        glUniform1f(light0ambientIntensityUniformLocation, 0.2f);
        glUniform3fv(light0cameraPositionUniformLocation, 1, &camera.GetPos()[0]);

        glUniform1f(light0specularIntensityUniformLocation, 256.0f);

        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix() * camera.GetViewMatrix() * suzanne.getModelMatrix() * offsetMatrix));
        suzanne.render(drawMode);
        glBindVertexArray(0);

        offsetMatrix += glm::mat4(0,0,0,0,
                                    0,0,0,0,
                                    0,0,0,0,
                                    2,0,0,0);


        // my own added model and shaders
        glUseProgram(modelShaderProgram2);
        glUniform1f(colorMultUniformLocation, colorMult);
        glUniform4f(uniformColorLocation2, uniformColor2.r, uniformColor2.g, uniformColor2.b, uniformColor2.a);
        glUniformMatrix4fv(mvpMatrixUniform2, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix() * camera.GetViewMatrix() * cube.getModelMatrix() * offsetMatrix));
        cube.render(drawMode);
        glBindVertexArray(0);
        // glUseProgram(0);

        // glUseProgram(modelShaderProgram);
        offsetMatrix += glm::mat4(1,0,0,0,
                                            0,1,0,0,
                                            0,0,1,0,
                                            3,0,0,1);

        DrawModel(modelShaderProgram, lightSphere, offsetMatrix,
            colorMultUniformLocation, uniformColorLocation,light0colorUniformLocation, light0positionUniformLocation,
            light0ambientIntensityUniformLocation, light0cameraPositionUniformLocation, light0specularIntensityUniformLocation,
            mvpMatrixUniform);
        // glUseProgram(modelShaderProgram);
        // glUniform1f(colorMultUniformLocation, colorMult);
        // glUniform4f(uniformColorLocation, uniformColor.r, uniformColor.g, uniformColor.b, 1);
        // //light
        // glUniform4fv(light0colorUniformLocation, 1, &lightObject.GetColor()[0]);
        // // printf("lightObject color = (%f,%f,%f,%f) \n", lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        // // printf("%i set to color (%f,%f,%f,%f)\n", light0colorUniformLocation, lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        // glUniform3fv(light0positionUniformLocation, 1, &lightPos[0]);
        // glUniform1f(light0ambientIntensityUniformLocation, 0.2f);
        // glUniform3fv(light0cameraPositionUniformLocation, 1, &camera.GetPos()[0]);
        // glUniform1f(light0specularIntensityUniformLocation, apeSpecularIntensity);
        //
        // glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix() * camera.GetViewMatrix() * lightSphere.getModelMatrix() * offsetMatrix));
        // lightSphere.render(drawMode);
        // // glBindVertexArray(0);



        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    glDeleteProgram(modelShaderProgram);
    glDeleteProgram(modelShaderProgram2);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

//drawModel
void DrawModel(const unsigned int modelShaderProgram, core::Model model, glm::mat4 offsetMatrix, GLint colorMultUniformLocation, GLint uniformColorLocation,
    GLint light0colorUniformLocation, GLint light0positionUniformLocation, GLint light0ambientIntensityUniformLocation,
    GLint light0cameraPositionUniformLocation, GLint light0specularIntensityUniformLocation, GLint mvpMatrixUniform)
{
    glUseProgram(modelShaderProgram);
    glUniform1f(colorMultUniformLocation, colorMult);
    glUniform4f(uniformColorLocation, uniformColor.r, uniformColor.g, uniformColor.b, 1);
    //light
    if (light0colorUniformLocation != -1) {
        glUniform4fv(light0colorUniformLocation, 1, &lightObject.GetColor()[0]);
        // printf("lightObject color = (%f,%f,%f,%f) \n", lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        // printf("%i set to color (%f,%f,%f,%f)\n", light0colorUniformLocation, lightObject.GetColor().r, lightObject.GetColor().g, lightObject.GetColor().b, lightObject.GetColor().a);
        glUniform3fv(light0positionUniformLocation, 1, &lightPos[0]);
        glUniform1f(light0ambientIntensityUniformLocation, 0.2f);
        glUniform3fv(light0cameraPositionUniformLocation, 1, &camera.GetPos()[0]);
        glUniform1f(light0specularIntensityUniformLocation, modelSpecularIntensity);
    }

    glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix() * camera.GetViewMatrix() * model.getModelMatrix() * offsetMatrix));
    model.render(drawMode);
    // glBindVertexArray(0);
}

//input get callback camera rotation
static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (inWindow && holdingRMB) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        printf("cursorPosCallback\n");
        printf("pos = (%f,%f)\n", xpos, ypos);

        float xoffset = lastX - xpos;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        float sensitivity = 0.2f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        camera.RotateOnX(yoffset);
        camera.RotateOnY(xoffset);
    }
    else {
        firstMouse = true;
    }
}
void cursorEnterCallback(GLFWwindow *window, int entered)
{
    if (entered) {
        printf("entered window\n");
        inWindow = true;
    }
    else {
        printf("left window\n");
        inWindow = false;
    }
}
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (inWindow) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            printf("right mouse button pressed\n");
            holdingRMB = true;
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            printf("right mouse button released\n");
            holdingRMB = false;
        }
    }
}