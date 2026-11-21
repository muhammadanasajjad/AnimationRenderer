#pragma once

#include <QWidget>

#include "ColourPalette.h"

class CodeEditor;
class QPaintEvent;

class LineNumberArea : public QWidget {
    Q_OBJECT

public:
    LineNumberArea(QWidget* parent, ColourPalette colours);

    void setEditor(CodeEditor* editor);
    void redraw();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    CodeEditor* editor;
    ColourPalette colours;
};
