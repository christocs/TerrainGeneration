#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void update(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void render(GLFWwindow* window)
{
    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
}

int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Terrain Generation", glfwGetPrimaryMonitor(), nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialise glad" << std::endl;
        glfwTerminate();
        return -1;
    }

    printf("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

    glViewport(0, 0, mode->width, mode->height);

    while (!glfwWindowShouldClose(window)) {
        update(window);

        render(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
