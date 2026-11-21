#include "MainWindow.h"
#include "ui/CodeEditor.h"
#include "ui/ColourPalette.h"
#include "ui/CodeEditorContainer.h"
#include "ui/Viewport.h"

#include <QMenuBar>
#include <QStatusBar>

#include <QFontDatabase>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QPlainTextEdit>
#include <QTextEdit>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    setWindowTitle("Animation Rasteriser");
    resize(1000, 1000);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    QWidget* codeViewport = new QWidget(centralWidget);
    QHBoxLayout* cVLayout = new QHBoxLayout(codeViewport);

    ColourPalette palette = createDarkPalette();
    codeEditor = new CodeEditorContainer(codeViewport, palette);
    codeEditor->getEditor()->setPlainText(
        "SETTINGS : {\n"
        "  resolution: 1920*1080\n"
        "  frameRate: 60fps\n"
        "}\n"
        "\n"
        "SCENE someScene: {\n"
        "  CUBE cube: { size: 2, position: {0, 1, 1} }\n"
        "  fadeIn cube over 1000ms using easeInOut\n"
        "  wait\n"
        "}\n"
        "play someScene\n"
    );
    viewport = new Viewport(codeViewport);

    errorBox = new QTextEdit(centralWidget);
    errorBox->setReadOnly(true);
    errorBox->setPlaceholderText("No errors");
    errorBox->setMaximumHeight(160);
    errorBox->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    QPalette errorPalette = errorBox->palette();
    errorPalette.setColor(QPalette::Base, palette.editorBackground);
    errorPalette.setColor(QPalette::Text, palette.errorText);
    errorBox->setPalette(errorPalette);

    connect(codeEditor->getEditor(), &QPlainTextEdit::textChanged,
            this, &MainWindow::updateSourceCode);
    connect(viewport, &Viewport::errorOccurred, this, &MainWindow::showError);
    updateSourceCode();

    cVLayout->addWidget(codeEditor, 1);
    cVLayout->addWidget(viewport, 1);

    layout->addWidget(codeViewport, 4);
    layout->addWidget(errorBox, 1);

    setCentralWidget(centralWidget);
}

void MainWindow::updateSourceCode() {
    viewport->setSourceCode(codeEditor->getEditor()->toPlainText());
}

void MainWindow::showError(QString message, int lineNumber, int columnNumber) {
    codeEditor->getEditor()->highlightErrorLine(lineNumber);

    if (message.isEmpty()) {
        errorBox->clear();
    } else {
        errorBox->append(message);
    }
}
