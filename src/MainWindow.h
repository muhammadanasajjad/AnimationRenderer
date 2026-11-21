#pragma once

#include <QMainWindow>
#include <QString>

class Viewport;
class CodeEditorContainer;
class QTextEdit;

class MainWindow : public QMainWindow {
    // this is a qt macro for some qt features
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots:
    void updateSourceCode();
    void showError(QString message, int lineNumber, int columnNumber);

private:
    Viewport* viewport;
    CodeEditorContainer* codeEditor;
    QTextEdit* errorBox;
};
