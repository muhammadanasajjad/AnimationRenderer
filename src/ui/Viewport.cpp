#include <glad/glad.h>

#include "Viewport.h"

#include <QOpenGLContext>
#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include "../language/Tokeniser.h"
#include "../language/ParseError.h"

QString describeParseError(Token token, QString message) {
    if (token.lineNumber <= 0) {
        return "End of file: " + message;
    }

    return "Line " + QString::number(token.lineNumber) +
           ", column " + QString::number(token.columnNumber) +
           ": " + message;
}

Viewport::Viewport(QWidget* parent)
    : QOpenGLWidget(parent) {

    setMinimumSize(400, 400);

    connect(&timer, &QTimer::timeout, this, [this]() {
        update();
    });

    timer.setInterval(1000.0f/60.0f);
}

void Viewport::initializeGL() {
    if (!gladLoadGLLoader([](const char* name) -> void* {
        return reinterpret_cast<void*>(
            QOpenGLContext::currentContext()->getProcAddress(name)
        );
    })) {
        std::cerr << "Failed to initialise GLAD" << std::endl;
        return;
    }

    std::cout << "OpenGL "
              << reinterpret_cast<const char*>(glGetString(GL_VERSION))
              << std::endl;

    glEnable(GL_DEPTH_TEST);

    loadProgram();

    frameTimer.start();
    timer.start();
}

void Viewport::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
}

void Viewport::paintGL() {
    // TODO: use export settings
    float deltaTime = 1.0f / 60.0f;

    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!scene || !executor) {
        return;
    }

    executor->update(deltaTime);
    scene->update(deltaTime);
    scene->render();
}

void Viewport::setSourceCode(QString code) {
    source = code.toStdString();
    if (scene) {
        loadProgram();
    }
}

void Viewport::loadProgram() {
    emit errorOccurred(QString(""), 0, 0);

    std::unique_ptr<Scene> newScene = std::make_unique<Scene>();
    std::unique_ptr<Executor> newExecutor = std::make_unique<Executor>(newScene->getObjects());
    std::unique_ptr<ProgramNode> newProgram;

    try {
        Tokeniser tokeniser(source);
        tokeniser.tokenise(false);

        Parser parser(tokeniser.getTokens());
        newProgram = parser.parse();
        newExecutor->execute(newProgram.get());
    } catch (ParseError& error) {
        QString message = describeParseError(error.token, QString::fromStdString(error.message));
        std::cerr << "Failed to load program: " << message.toStdString() << std::endl;
        emit errorOccurred(message, error.token.lineNumber, error.token.columnNumber);
        return;
    } catch (std::exception& error) {
        QString message = "Execution error: " + QString::fromStdString(error.what());
        std::cerr << "Failed to load program: " << message.toStdString() << std::endl;
        emit errorOccurred(message, 0, 0);
        return;
    }

    scene = std::move(newScene);
    executor = std::move(newExecutor);
    program = std::move(newProgram);
}
