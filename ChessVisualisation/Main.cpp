#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

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
const float CUBE_SCALE  = 0.2f;
const float PIECE_SCALE = 0.02f;
const float SQUARE_SIZE = 0.78f;
glm::vec3 STARTING_POS  = glm::vec3(3.5f * SQUARE_SIZE, 0, -SQUARE_SIZE);

Camera camera(glm::vec3(-8.0f, 6.0f, 0.0f) + STARTING_POS);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(3, 1, 3);


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

    Shader basicShader("../Shaders/basic_shader.vs.c", "../Shaders/basic_shader.fs.c");
    Shader lightCubeShader("../Shaders/light_cube_shader.vs.c", "../Shaders/light_cube_shader.fs.c");
    Model chess_board(
        "../Models/board/board.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        BOARD_SCALE
    );
    Model light_cube(
        "../Models/cube/cube.obj",
        STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
        CUBE_SCALE
    );
    Model bishop_black(
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
    );

    while (!glfwWindowShouldClose(window))
    {
        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateShaderMatrixes(basicShader);
        basicShader.setVec3("lightPos", lightPos + STARTING_POS);
        basicShader.setVec3("light.position", lightPos + STARTING_POS);
        basicShader.setVec3("viewPos", camera.Position);
        basicShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        basicShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        basicShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        basicShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        basicShader.setFloat("material.shininess", 64.0f);

        bishop_black.Draw(basicShader, getSquareCoord(5, 0));
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
            pawn_white.Draw(basicShader, getSquareCoord(i, 6));
         

        chess_board.Draw(basicShader);
        updateShaderMatrixes(lightCubeShader);
        light_cube.Draw(lightCubeShader, lightPos);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void updateShaderMatrixes(Shader& shader) {
    shader.use();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ResetCameraPosition();
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

glm::vec3 getSquareCoord(float x, float y) {
    return glm::vec3(x * SQUARE_SIZE, 0, -y * SQUARE_SIZE);
}
