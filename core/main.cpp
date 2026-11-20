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
#include <list>

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
float deltaTime = 0.0f;
core::InputHandler inputHandler;
core::modelRenderer model_renderer(0, &lightObject, &camera, aspectRatio, &drawMode);
std::list<core::Model> modelList;
core::Model* Instantiate(core::Model);
//check renderdoc
//framebuffer
    unsigned int framebuffer;
    unsigned int textureColorbuffer;
    unsigned int rbo;
glm::ivec4 framebufferEffects;
glm::vec4 framebufferAdditionColor = glm::vec4(0.0f,0.0f,0.2f,0.0f);
glm::vec4 framebufferMultColor = glm::vec4(1.0f,1.0f,0.2f,1.0f);
//unsigned int fbo;
//glGenFramebuffers(1, &fbo);
//glBindFrameBuffer(GL_FRAMEBUFFER, fbo);

void CreateFrameBuffers()
{
    //start framebuffer setup
    //create framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    //create texture color buffer

    // generate texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_width, g_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    //create rendering buffer object
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_width, g_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //lookup stencil buffer

    //attach to rbo
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete! \n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //End frame buffer setup
}
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
    CreateFrameBuffers();
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

void ImGuiTab(core::Model* model)
{
    if (ImGui::CollapsingHeader(model->GetName().c_str()))
    {
        if (ImGui::TreeNode("Transform"))
        {
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Material")) {
            glm::vec4 newColor = model->material.GetColor();
            if (ImGui::ColorEdit3("_Color", &newColor[0])) {
                model->material.SetColor(newColor);
            }
            float newSpecularIntensity = model->material.GetSpecularIntensity();
            if (ImGui::InputFloat ("_Specular_Intensity", &newSpecularIntensity, 1)) {
                model->material.SetSpecularIntensity(newSpecularIntensity);
            }
            ImGui::TreePop();
        }
    }
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
    //frame buffer shaders
    const GLint fbVertexShader = generateShader("shaders/vertex.vs", GL_VERTEX_SHADER);
    const GLint fbFragmentShader = generateShader("shaders/fbFragment.fs", GL_FRAGMENT_SHADER);

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

    //framebuffer program
    const unsigned int frameBufferShaderProgram = glCreateProgram();
    glAttachShader(frameBufferShaderProgram, fbVertexShader);
    glAttachShader(frameBufferShaderProgram, fbFragmentShader);
    glLinkProgram(frameBufferShaderProgram);
    glGetProgramiv(frameBufferShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(frameBufferShaderProgram, 512, NULL, infoLog);
        printf("Error! Making Shader Program: %s\n", infoLog);
    }

    glDeleteShader(modelVertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(textureShader);

    #pragma region load models ---
    core::Mesh quad = core::Mesh::generateQuad();
    core::Model quadModel({quad});
    quadModel.translate(glm::vec3(0,0,-2.5));
    quadModel.scale(glm::vec3(5, 5, 1));
    // quadModel.rotate(glm::vec3(0,0,1), glm::radians(180.0f));
    quadModel.SetName("QuadModel");

    core::Model suz = core::AssimpLoader::loadModel("models/nonormalmonkey.obj");
    core::Model* suzanne = Instantiate(suz);
    suzanne->material.SetColor(glm::vec4(0.6f,.3f,0.0f,1.0f));
    core::Texture cmgtGatoTexture("textures/CMGaTo_crop.png");
    suzanne->SetName("Monke");

    //my own model
    // core::Model cube = core::AssimpLoader::loadModel("models/RandoCubeForRenderingEngines2.obj");
    core::Model box = core::AssimpLoader::loadModel("models/RandoCubeForRenderingEnginesSmooth.obj");
    core::Model* cube = Instantiate(box);
    cube->material.SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    cube->SetName("Cube");

    core::Model LTS = core::AssimpLoader::loadModel("models/LightTestSphere.obj");
    core::Model* lightTestSphere = Instantiate(LTS);
    lightTestSphere->SetName("LightTestSphere");
    lightTestSphere->material.SetColor(glm::vec4(0.0f, 0.5f, 1.0f, 1.0f));
    // lightSphere.scale(glm::vec3(5, 5, 1));

    core::Mesh screenQuad = core::Mesh::generateQuad();
    core::Model screenQuadModel({quad});
    #pragma  endregion

    camera.UpdateCameraVectors();

    glm::vec4 clearColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glClearColor(clearColor.r,
                 clearColor.g, clearColor.b, clearColor.a);

    // GLint colorMultUniformLocation = glGetUniformLocation(modelShaderProgram, "colorMult");

    GLint textureModelUniform = glGetUniformLocation(textureShaderProgram, "mvpMatrix");
    GLint textureUniform = glGetUniformLocation(textureShaderProgram, "text");

    //framebuffer locations
    GLint sceneTextureUniform = glGetUniformLocation(frameBufferShaderProgram, "scene");
    GLint fbEffectsUniform = glGetUniformLocation(frameBufferShaderProgram, "effects");
    GLint fbAdditionColorUniform = glGetUniformLocation(frameBufferShaderProgram, "additionColor");
    GLint fbMultColorUniform = glGetUniformLocation(frameBufferShaderProgram, "multColor");

    CreateFrameBuffers();

    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float rotationStrength = 100.0f;
    double Xpos, Ypos;

    while (!glfwWindowShouldClose(window)) {
        // first pass of framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
        glEnable(GL_DEPTH_TEST);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Raw Engine v2");
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

        ImGui::BeginTabBar("Hierarchy");
        if (ImGui::BeginTabItem("Models")) {
            int i = 0;
            for (core::Model& model : modelList) {
                ImGui::PushID(i);
                ImGuiTab(&model);
                ImGui::PopID();
                i++;
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();

        ImGui::BeginTabBar("Lightining");
        if (ImGui::BeginTabItem("My Light")) {
            if (ImGui::DragFloat3("Light Position:", glm::value_ptr(lightObject.transform.position))) {
                model_renderer.UpdateLightObject(&lightObject);
            }
            // ImGui::PopID();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();

        ImGui::BeginTabBar("FrameBuffers");
        if (ImGui::BeginTabItem("My FrameBuffer")) {
            if (ImGui::InputInt4("Iteration:", glm::value_ptr(framebufferEffects))) {
                framebufferEffects[0] = std::clamp(framebufferEffects[0], 0, 4);
                framebufferEffects[1] = std::clamp(framebufferEffects[1], 0, 4);
                framebufferEffects[2] = std::clamp(framebufferEffects[2], 0, 4);
                framebufferEffects[3] = std::clamp(framebufferEffects[3], 0, 4);
            }
            if (ImGui::ColorEdit3("AdditionColor:", glm::value_ptr(framebufferAdditionColor)))
            {
                framebufferAdditionColor.a = 1.0f;
            }
            if (ImGui::ColorEdit3("MultColor:", glm::value_ptr(framebufferMultColor)))
            {
                framebufferMultColor.a = 1.0f;
            }
            // ImGui::PopID();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
        ImGui::End();




        processInput(window);
        suzanne->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));
        cube->rotate(glm::vec3(0.0f, 1.0f, 1.0f), glm::radians(rotationStrength) * static_cast<float>(deltaTime));

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

        model_renderer.DrawModel(*suzanne, offsetMatrix, suzanne->material.GetSpecularIntensity());

        offsetMatrix[3] += glm::vec4(3,0,1,0);

        model_renderer.DrawModel(*suzanne, offsetMatrix, suzanne->material.GetSpecularIntensity());

        offsetMatrix[3] += glm::vec4(2,0,0,0);

        // my own added model and shaders
        model_renderer.DrawModel(*lightTestSphere, offsetMatrix, lightTestSphere->material.GetSpecularIntensity());

        model_renderer.DrawModel(*cube, offsetMatrix, cube->material.GetSpecularIntensity());

        offsetMatrix[3] += glm::vec4(3,0,0,1);

        model_renderer.DrawModel(*lightTestSphere, offsetMatrix, lightTestSphere->material.GetSpecularIntensity());


        // second pass framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(frameBufferShaderProgram);
        //tell to use texture
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(sceneTextureUniform, 0);
        glUniform4iv(fbEffectsUniform,1, glm::value_ptr(framebufferEffects));
        // printf("iterations: %d\n", framebufferIteration);
        glUniform4fv(fbAdditionColorUniform, 1, glm::value_ptr(framebufferAdditionColor));
        glUniform4fv(fbMultColorUniform, 1, glm::value_ptr(framebufferMultColor));
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        screenQuadModel.render(GL_TRIANGLES);

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

core::Model* Instantiate(core::Model model)
{
    modelList.push_back(model);
    return &modelList.back();
}