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
    
void updateShaderMatrixes(Shader& shader);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
glm::vec3 getSquareCoord(float x, float y);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const float BOARD_SCALE = 0.3f;
const float LAMP_SCALE  = 0.008f;
const float PIECE_SCALE = 0.02f;
const float SQUARE_SIZE = 0.78f;

const float SPOTLIGHT_HEIGHT            = 1.5f;
const float SPOTLIGHT_FULL_TURN_TIME_S  = 12.0f;
const float SPOTLIGHT_MOVEMENT_RADIUS   = 5.0f;
glm::vec3 STARTING_POS  = glm::vec3(3.5f * SQUARE_SIZE, 0, -SQUARE_SIZE);

float last_camera_change_time = 0;
int current_camera_index = 0;
Camera spotlight_camera = Camera();
Camera moving_camera(glm::vec3(-8.0f, 6.0f, 0.0f) + STARTING_POS);
Camera stable_camera(glm::vec3(0, 9.0f, 0.0f) + STARTING_POS, glm::vec3(0, 1, 0), 0, -90);
Camera* cameras[] = {
    &spotlight_camera, // id = 0
    &moving_camera,    // id = 1
    &stable_camera     // id = 2
};

glm::vec3 lampPos(0, 4, 0);
float lamp_brightness_level = 9; // scale: 0 - 9

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float spotlight_angle = -30.0f;


float deltaTime = 0.0f;
float lastFrame = 0.0f;


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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader basicShader("../Shaders/basic_shader.vert", "../Shaders/basic_shader.frag");
    Shader lampShader("../Shaders/lamp_shader.vert", "../Shaders/lamp_shader.frag");
    Shader spotlightShader("../Shaders/lamp_shader.vert", "../Shaders/lamp_shader.frag");
    Model spotlight(
        "../Models/spotlight/spotlight.obj",
        STARTING_POS + glm::vec3(0.0f, 1.0f, 0.0f),
        0.01f,
        glm::vec3(90, 0, -90)
    );
    Model spotlight2(
        "../Models/spotlight/spotlight2.obj",
        STARTING_POS + glm::vec3(0.0f, 1.0f, 0.0f),
        0.01f,
        glm::vec3(90, 0, -90)
    );
    Model chess_board(
        "../Models/board/board.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        BOARD_SCALE
    );
    Model lamp1(
        "../Models/lamp/lamp1.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        LAMP_SCALE
    );
    Model lamp2(
        "../Models/lamp/lamp2.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        LAMP_SCALE
    );
    /*Model bishop_black(
        "../Models/black/bishop/bishop.obj",
        glm::vec3(0, 0.12f, 0.0f),
        PIECE_SCALE
    );
    Model king_black(
        "../Models/black/king/king.obj",
        glm::vec3(0.0, 0.12f, -1.4f),
        PIECE_SCALE
    );
    Model pawn_black(
        "../Models/black/pawn/pawn.obj",
        glm::vec3(0, 0.12f, 1.9f),
        PIECE_SCALE
    );
    Model knight_black(
        "../Models/black/knight/knight.obj",
        glm::vec3(0, 0.12f, 0.6f),
        PIECE_SCALE
    );
    Model queen_black( 
        "../Models/black/queen/queen.obj",
        glm::vec3(0, 0.12f,-0.68f),
        PIECE_SCALE
    );
    Model rook_black(
        "../Models/black/rook/rook.obj",
        glm::vec3(0, 0.12f, 1.28),
        PIECE_SCALE
    );

    Model bishop_white(
        "../Models/white/bishop/bishop.obj",
        glm::vec3(0, 0.12f, 0.0f),
        PIECE_SCALE
    );
    Model king_white(
        "../Models/white/king/king.obj",
        glm::vec3(0.0, 0.12f, -1.4f),
        PIECE_SCALE
    );
    Model pawn_white(
        "../Models/white/pawn/pawn.obj",
        glm::vec3(0, 0.12f, 1.9f),
        PIECE_SCALE
    );
    Model knight_white(
        "../Models/white/knight/knight.obj",
        glm::vec3(0, 0.12f, 0.6f),
        PIECE_SCALE
    );
    Model queen_white(
        "../Models/white/queen/queen.obj",
        glm::vec3(0, 0.12f, -0.68f),
        PIECE_SCALE
    );
    Model rook_white(
        "../Models/white/rook/rook.obj",
        glm::vec3(0, 0.12f, 1.28),
        PIECE_SCALE
    );*/

    while (!glfwWindowShouldClose(window))
    {
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float time = (float)glfwGetTime();
        float angle = (time / SPOTLIGHT_FULL_TURN_TIME_S) * 2 * MATH_PI;
        spotlight_camera.Position = STARTING_POS + glm::vec3(std::cos(angle) * SPOTLIGHT_MOVEMENT_RADIUS,
                                                   SPOTLIGHT_HEIGHT,
                                                   std::sin(angle) * SPOTLIGHT_MOVEMENT_RADIUS);
        spotlight_camera.Front = STARTING_POS - spotlight_camera.Position;

        updateShaderMatrixes(lampShader);
        lampShader.setFloat("brightnessLevel", lamp_brightness_level / 9);
        lamp2.Draw(lampShader, lampPos);

        updateShaderMatrixes(spotlightShader);
        lampShader.setFloat("brightnessLevel", 1);
        spotlight2.Draw(spotlightShader,
            glm::vec3(std::cos(angle) * SPOTLIGHT_MOVEMENT_RADIUS,
                SPOTLIGHT_HEIGHT,
                std::sin(angle) * SPOTLIGHT_MOVEMENT_RADIUS),
            glm::vec3(spotlight_angle, 0, glm::degrees(angle)));

        updateShaderMatrixes(basicShader);


        basicShader.setVec3("viewPos", cameras[current_camera_index]->Position);

        // lamp light definition
        basicShader.setVec3("lampLight.position", lampPos + STARTING_POS);
        basicShader.setFloat("lampLight.constant", 1.0f);
        basicShader.setFloat("lampLight.linear", 0.004);
        basicShader.setFloat("lampLight.quadratic", 0.009);
        basicShader.setVec3("lampLight.ambient", 0.2f, 0.2f, 0.2f);
        basicShader.setVec3("lampLight.diffuse", 0.9f, 0.9f, 0.9f);
        basicShader.setVec3("lampLight.specular", 1.0f, 1.0f, 1.0f);
        basicShader.setFloat("lampLight.brightnessLevel", lamp_brightness_level / 9);

        // spotlight light definition
        float spotlight_aim_h = 5;
        glm::vec3 spotlight_aim = glm::vec3(STARTING_POS.x, spotlight_aim_h, STARTING_POS.z);
        basicShader.setVec3("spotlightLight.direction", spotlight_aim - spotlight_camera.Position);
        basicShader.setVec3("spotlightLight.position", spotlight_camera.Position);
        basicShader.setFloat("spotlightspotlightLight.cutOff", glm::cos(glm::radians(90.0f)));
        basicShader.setVec3("spotlightLight.ambient", 0.1f, 0.1f, 0.1f);
        basicShader.setVec3("spotlightLight.diffuse", 0.8f, 0.8f, 0.8f);
        basicShader.setVec3("spotlightLight.specular", 1.0f, 1.0f, 1.0f);
        basicShader.setFloat("spotlightLight.constant", 1.0f);
        basicShader.setFloat("spotlightLight.linear", 0.09f);
        basicShader.setFloat("spotlightLight.quadratic", 0.032f);

        basicShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        basicShader.setFloat("material.shininess", 64.0f);

        lamp1.Draw(basicShader, lampPos);
        spotlight.Draw(basicShader,
            glm::vec3(std::cos(angle) * SPOTLIGHT_MOVEMENT_RADIUS,
                SPOTLIGHT_HEIGHT,
                std::sin(angle) * SPOTLIGHT_MOVEMENT_RADIUS),
            glm::vec3(spotlight_angle, 0, glm::degrees(angle)));

        /*bishop_black.Draw(basicShader, getSquareCoord(5, 0));
        bishop_black.Draw(basicShader, getSquareCoord(2, 0));
        king_black.Draw(basicShader, getSquareCoord(4, 0));
        knight_black.Draw(basicShader, getSquareCoord(1, 0));
        knight_black.Draw(basicShader, getSquareCoord(6, 0));
        queen_black.Draw(basicShader, getSquareCoord(3, 0));
        rook_black.Draw(basicShader, getSquareCoord(0, 0));
        rook_black.Draw(basicShader, getSquareCoord(7, 0)); 
        for (int i = 0; i < 8; i++)
            pawn_black.Draw(basicShader, getSquareCoord(i, 1));
        
        bishop_white.Draw(basicShader, getSquareCoord(5, 7));
        bishop_white.Draw(basicShader, getSquareCoord(2, 7));
        king_white.Draw(basicShader, getSquareCoord(4, 7));
        knight_white.Draw(basicShader, getSquareCoord(1, 7));
        knight_white.Draw(basicShader, getSquareCoord(6, 7));
        queen_white.Draw(basicShader, getSquareCoord(3, 7));
        rook_white.Draw(basicShader, getSquareCoord(0, 7));
        rook_white.Draw(basicShader, getSquareCoord(7, 7));
        for (int i = 0; i < 8; i++)
            pawn_white.Draw(basicShader, getSquareCoord(i, 6));*/
         
        chess_board.Draw(basicShader);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void updateShaderMatrixes(Shader& shader) {
    shader.use();
    glm::mat4 projection = glm::perspective(glm::radians(moving_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = cameras[current_camera_index]->GetViewMatrix();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (float)glfwGetTime() - last_camera_change_time > 0.5f) {
        last_camera_change_time = (float)glfwGetTime();
        current_camera_index = (current_camera_index + 1) % 3;
    }
    if (current_camera_index == 1) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            moving_camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            moving_camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            moving_camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            moving_camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && spotlight_angle < -0.15f)
        spotlight_angle += 0.1;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && spotlight_angle > -45.0f)
        spotlight_angle -= 0.1;
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        lamp_brightness_level = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        lamp_brightness_level = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        lamp_brightness_level = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        lamp_brightness_level = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        lamp_brightness_level = 4;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        lamp_brightness_level = 5;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        lamp_brightness_level = 6;
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        lamp_brightness_level = 7;
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        lamp_brightness_level = 8;
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        lamp_brightness_level = 9;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

    moving_camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    moving_camera.ProcessMouseScroll(yoffset);
}

glm::vec3 getSquareCoord(float x, float y) {
    return glm::vec3(x * SQUARE_SIZE, 0, -y * SQUARE_SIZE);
}
