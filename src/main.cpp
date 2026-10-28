
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "scene/Scene.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialise GLFW" << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        800,
        800,
        "Animation Rasteriser",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialise GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::cout << "OpenGL "
              << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << std::endl;
    
    Scene scene = Scene();
    double lastTime = glfwGetTime();
    int frames = 0;
    int fps = 0;

    while (!glfwWindowShouldClose(window)) {
        int width;
        int height;

        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        scene.update(1.0f/60.0f);
        scene.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        frames++;

        double currentTime = glfwGetTime();

        if (currentTime - lastTime >= 1.0) {
            fps = frames;
            frames = 0;
            lastTime = currentTime;

            glfwSetWindowTitle(
                window,
                ("Animation Rasteriser | FPS: " + std::to_string(fps)).c_str()
            );
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

