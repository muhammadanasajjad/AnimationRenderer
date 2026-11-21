#pragma once

#include <QOpenGLWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QString>

#include <memory>
#include <string>

#include "../scene/Scene.h"
#include "../language/Executor.h"
#include "../language/Parser.h"

class Viewport : public QOpenGLWidget {
    Q_OBJECT

public:
    Viewport(QWidget* parent = nullptr);

    void setSourceCode(QString code);

signals:
    void errorOccurred(QString message, int lineNumber, int columnNumber);

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private:
    void loadProgram();

    std::string source;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Executor> executor;
    std::unique_ptr<ProgramNode> program;

    QTimer timer;
    QElapsedTimer frameTimer;
};
