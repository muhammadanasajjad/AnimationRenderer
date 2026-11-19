#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "scene/Scene.h"
#include "language/Tokeniser.h"
#include "language/Parser.h"
#include "language/ASTPrinter.h"
#include "language/Executor.h"

int main() {
    std::string source = R"(
SETTINGS : {
  resolution: 1920*1080
  frameRate: 60fps
}

PRELOAD: {
  "test.irt"
  "some.ttf" as mainFont
  "some.obj" as cuboid, "sphere.obj" as sphere
}

SCENE someScene: {
  INTERPOLATION customInterp: {
    function: f(t) = 1-e^(-6*t)*cos(10*t)
  }
  CUBE cube: { size: 2, position: {0, 1, 1} }
  fadeIn cube over 1000ms using easeInOut
  wait
  rotate cube { x: 10deg, y: -30deg } over 1500ms using customInterp
  wait
  SPHERE someName: { radius: 1 }
  show someName, edit someName { radius: 7 } over 1000ms using customInterp
  move cube { x: 0, y: 6, z: 6 } over 1000ms using customInterp
  wait, wait 1000ms
  TEXT helloText: { text: "Hello!" }
  fadeOut cube over 1000ms
  fadeOut someName over 1000ms
  fadeIn helloText over 3000ms
  wait
}
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
play someScene
)";

    Tokeniser tokeniser = Tokeniser(source);
    tokeniser.tokenise();

    Parser parser = Parser(tokeniser.getTokens());
    std::unique_ptr<ProgramNode> program = parser.parse();

    ASTPrinter::print(program.get());

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
    Executor executor = Executor(scene.getObjects());
    executor.execute(program.get());

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

        executor.update(1.0f / 60.0f);
        scene.update(1.0f / 60.0f);
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
