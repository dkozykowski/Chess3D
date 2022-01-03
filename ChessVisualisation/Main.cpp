#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define MATH_PI 3.14159265359f

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <math.h>
#include <iostream>
#include <figureset.h>

    
// Functions definitions 
void UpdateShaderMatrixes(Shader& shader);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
void UpdateLightningShaderSettings(Shader& shader);
void UpdateShaderMatrixes(Shader& shader);


// Settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 750;

const float LAMP_SCALE  = 0.008f;

const float SPOTLIGHT_HEIGHT            = 1.5f;
const float SPOTLIGHT_FULL_TURN_TIME_S  = 12.0f;
const float SPOTLIGHT_MOVEMENT_RADIUS   = 5.0f;


float lastCameraChangeTime = 0;
int currentCameraIndex = 0;
Camera spotlightCamera = Camera();
Camera movingCamera(glm::vec3(-8.0f, 6.0f, 0.0f) + STARTING_POS);
Camera stableCamera(glm::vec3(0, 9.0f, 0.0f) + STARTING_POS, glm::vec3(0, 1, 0), 0, -90);
Camera* cameras[] = {
    &spotlightCamera, // id = 0
    &movingCamera,    // id = 1
    &stableCamera     // id = 2
};

glm::vec3 lampPos(0, 4, 0);
float lampBrightnessLevel = 9; // scale: 0 - 9

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool spotlightLightIsActive = false;
float spotlightAngle = -30.0f;
float lastSpotlightChangeTime = 0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int currentShaderIndex = 0;

bool useBlinn = true;


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Chess 3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Figureset figureset("../Models/");
    //figureset.LoadFigures();

    Shader phongShader("../Shaders/phong_lighting_shader.vert", "../Shaders/phong_lighting_shader.frag");
    Shader gouraudShader("../Shaders/gouraud_lighting_shader.vert", "../Shaders/gouraud_lighting_shader.frag");
    Shader lampShader("../Shaders/lamp_shader.vert", "../Shaders/lamp_shader.frag");
    Shader spotlightShader("../Shaders/lamp_shader.vert", "../Shaders/lamp_shader.frag");


    Shader* shaders[] = {
        &phongShader,   // id = 0
        &gouraudShader  // id = 1
    };

    Model spotlight(
        "../Models/spotlight/spotlight.obj",
        STARTING_POS + glm::vec3(0.0f, 1.0f, 0.0f),
        0.01f,
        glm::vec3(90, 0, -90)
    );
    Model spotlightLight(
        "../Models/spotlight/spotlightLight.obj",
        STARTING_POS + glm::vec3(0.0f, 1.0f, 0.0f),
        0.01f,
        glm::vec3(90, 0, -90)
    );
    
    Model lamp(
        "../Models/lamp/lamp.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        LAMP_SCALE
    );
    Model lampLight(
        "../Models/lamp/lampLight.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        LAMP_SCALE
    );

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ProcessInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = (float)glfwGetTime();
        float angle = (time / SPOTLIGHT_FULL_TURN_TIME_S) * 2 * MATH_PI;
        spotlightCamera.Position = STARTING_POS + glm::vec3(std::cos(angle) * SPOTLIGHT_MOVEMENT_RADIUS,
                                                   SPOTLIGHT_HEIGHT,
                                                   std::sin(angle) * SPOTLIGHT_MOVEMENT_RADIUS);
        spotlightCamera.Front = STARTING_POS - spotlightCamera.Position;

        UpdateShaderMatrixes(lampShader);
        lampShader.SetFloat("brightnessLevel", lampBrightnessLevel / 9);
        lampLight.Draw(lampShader, lampPos);

        UpdateShaderMatrixes(spotlightShader);
        lampShader.SetFloat("brightnessLevel", 1);
        if (spotlightLightIsActive) {
            spotlightLight.Draw(spotlightShader,
                glm::vec3(std::cos(angle) * SPOTLIGHT_MOVEMENT_RADIUS,
                    SPOTLIGHT_HEIGHT,
                    std::sin(angle) * SPOTLIGHT_MOVEMENT_RADIUS),
                glm::vec3(spotlightAngle, 0, glm::degrees(angle)));
        }

        UpdateShaderMatrixes(*shaders[currentShaderIndex]);
        UpdateLightningShaderSettings(*shaders[currentShaderIndex]);


        lamp.Draw(*shaders[currentShaderIndex], lampPos);
        spotlight.Draw(*shaders[currentShaderIndex],
            glm::vec3(std::cos(angle) * SPOTLIGHT_MOVEMENT_RADIUS,
                SPOTLIGHT_HEIGHT,
                std::sin(angle) * SPOTLIGHT_MOVEMENT_RADIUS),
            glm::vec3(spotlightAngle, 0, glm::degrees(angle)));

        figureset.Draw(*shaders[currentShaderIndex]);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void UpdateShaderMatrixes(Shader& shader) {
    shader.Use();
    glm::mat4 projection = glm::perspective(glm::radians(movingCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = cameras[currentCameraIndex]->GetViewMatrix();

    shader.SetMat4("projection", projection);
    shader.SetMat4("view", view);
}

void UpdateLightningShaderSettings(Shader& shader) {
    shader.Use();

    shader.SetVec3("viewPos", cameras[currentCameraIndex]->Position);

    // lamp light definition
    shader.SetVec3("lampLight.position", lampPos + STARTING_POS);
    shader.SetFloat("lampLight.constant", 1.0f);
    shader.SetFloat("lampLight.linear", 0.004);
    shader.SetFloat("lampLight.quadratic", 0.009);
    shader.SetVec3("lampLight.ambient", 0.2f, 0.2f, 0.2f);
    shader.SetVec3("lampLight.diffuse", 0.9f, 0.9f, 0.9f);
    shader.SetVec3("lampLight.specular", 1.0f, 1.0f, 1.0f);
    shader.SetFloat("lampLight.brightnessLevel", lampBrightnessLevel / 9);

    // spotlight light definition
    float spotlight_aim_h = (spotlightAngle + 45) / 10 - 1.5f;
    glm::vec3 spotlight_aim = glm::vec3(STARTING_POS.x, spotlight_aim_h, STARTING_POS.z);
    shader.SetBool("spotlightLight.ON", spotlightLightIsActive);
    shader.SetVec3("spotlightLight.direction", spotlight_aim - spotlightCamera.Position);
    shader.SetVec3("spotlightLight.position", spotlightCamera.Position);
    shader.SetFloat("spotlightLight.cutOff", glm::cos(glm::radians(30.0f)));
    shader.SetFloat("spotlightLight.outerCutOff", glm::cos(glm::radians(40.0f)));
    shader.SetVec3("spotlightLight.ambient", 0.1f, 0.1f, 0.1f);
    shader.SetVec3("spotlightLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader.SetVec3("spotlightLight.specular", 1.0f, 1.0f, 1.0f);
    shader.SetFloat("spotlightLight.constant", 1.0f);
    shader.SetFloat("spotlightLight.linear", 0.09f);
    shader.SetFloat("spotlightLight.quadratic", 0.032f);

    shader.SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
    shader.SetFloat("material.shininess", 64.0f);
    shader.SetBool("useBlinn", useBlinn);
}

void ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (float)glfwGetTime() - lastCameraChangeTime > 0.5f) {
        lastCameraChangeTime = (float)glfwGetTime();
        currentCameraIndex = (currentCameraIndex + 1) % 3;
    }
    if (currentCameraIndex == 1) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            movingCamera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            movingCamera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            movingCamera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            movingCamera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        currentShaderIndex = 0;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        currentShaderIndex = 1;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        useBlinn = true;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        useBlinn = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && (float)glfwGetTime() - lastSpotlightChangeTime > 0.25f) {
        lastSpotlightChangeTime = (float)glfwGetTime();
        spotlightLightIsActive = !spotlightLightIsActive;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && spotlightAngle < -0.15f)
        spotlightAngle += 0.05;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && spotlightAngle > -45.0f)
        spotlightAngle -= 0.05;
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        lampBrightnessLevel = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        lampBrightnessLevel = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        lampBrightnessLevel = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        lampBrightnessLevel = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        lampBrightnessLevel = 4;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        lampBrightnessLevel = 5;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        lampBrightnessLevel = 6;
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        lampBrightnessLevel = 7;
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        lampBrightnessLevel = 8;
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        lampBrightnessLevel = 9;
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    movingCamera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    movingCamera.ProcessMouseScroll(yoffset);
}
