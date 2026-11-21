#pragma once

#include <QWidget>

#include "ColourPalette.h"

class CodeEditor;
class LineNumberArea;

class CodeEditorContainer : public QWidget {
    Q_OBJECT

public:
    CodeEditorContainer(QWidget* parent, ColourPalette colours);

    CodeEditor* getEditor();

private:
    CodeEditor* editor;
    LineNumberArea* lineNumberArea;
};
