#include "CodeEditorContainer.h"

#include "CodeEditor.h"
#include "LineNumberArea.h"

#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QScrollBar>

CodeEditorContainer::CodeEditorContainer(QWidget* parent, ColourPalette colours)
    : QWidget(parent) {

    editor = new CodeEditor(this, colours);
    lineNumberArea = new LineNumberArea(this, colours);
    lineNumberArea->setEditor(editor);
    editor->setLineNumberArea(lineNumberArea);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(lineNumberArea);
    layout->addWidget(editor);

    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged,
            lineNumberArea, &LineNumberArea::redraw);
    connect(editor, &QPlainTextEdit::textChanged,
            lineNumberArea, &LineNumberArea::redraw);
    connect(editor, &QPlainTextEdit::cursorPositionChanged,
            lineNumberArea, &LineNumberArea::redraw);
}

CodeEditor* CodeEditorContainer::getEditor() {
    return editor;
}
