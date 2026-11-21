#include "LineNumberArea.h"

#include "CodeEditor.h"

#include <QPalette>
#include <QPaintEvent>
#include <QPainter>

LineNumberArea::LineNumberArea(QWidget* parent, ColourPalette colours)
    : QWidget(parent), editor(nullptr), colours(colours) {

    QPalette widgetPalette = palette();
    widgetPalette.setColor(QPalette::Window, colours.lineNumberBackground);
    setPalette(widgetPalette);
    setAutoFillBackground(true);
}

void LineNumberArea::setEditor(CodeEditor* editor) {
    this->editor = editor;
}

void LineNumberArea::redraw() {
    update();
}

void LineNumberArea::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    if (editor != nullptr) {
        editor->paintLineNumbers(event);
    }
}
