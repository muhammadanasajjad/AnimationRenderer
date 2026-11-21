#pragma once

#include <QPlainTextEdit>

#include "ColourPalette.h"

class LineNumberArea;
class QKeyEvent;
class QPaintEvent;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    CodeEditor(QWidget* parent, ColourPalette colours);

    void setLineNumberArea(LineNumberArea* area);
    void paintLineNumbers(QPaintEvent* event);
    void highlightErrorLine(int lineNumber);
    int lineNumberAreaWidth();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void updateLineNumberWidth(int newBlockCount);
    void highlightCurrentLine();

private:
    LineNumberArea* lineNumberArea;
    ColourPalette colours;
    int errorLine;
};
