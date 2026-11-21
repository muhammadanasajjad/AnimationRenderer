#include "CodeEditor.h"

#include "LineNumberArea.h"
#include "SyntaxHighlighter.h"

#include <QFontDatabase>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextEdit>
#include <QTextFormat>

CodeEditor::CodeEditor(QWidget* parent, ColourPalette colours)
    : QPlainTextEdit(parent), lineNumberArea(nullptr), colours(colours), errorLine(0) {

    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(11);
    setFont(font);

    QPalette editorPalette = palette();
    editorPalette.setColor(QPalette::Base, colours.editorBackground);
    editorPalette.setColor(QPalette::Text, colours.normalText);
    editorPalette.setColor(QPalette::Highlight, colours.selectionBackground);
    setPalette(editorPalette);

    setLineWrapMode(QPlainTextEdit::NoWrap);
    new SyntaxHighlighter(document(), colours);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberWidth);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    highlightCurrentLine();
}

void CodeEditor::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Tab) {
        insertPlainText("  ");
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void CodeEditor::setLineNumberArea(LineNumberArea* area) {
    lineNumberArea = area;
    updateLineNumberWidth(document()->blockCount());
}

int CodeEditor::lineNumberAreaWidth() {
    int maxLines = document()->blockCount();
    if (maxLines < 1) {
        maxLines = 1;
    }

    int digits = 1;
    while (maxLines >= 10) {
        digits++;
        maxLines = maxLines / 10;
    }

    return 12 + fontMetrics().horizontalAdvance('9') * digits;
}

void CodeEditor::updateLineNumberWidth(int newBlockCount) {
    if (lineNumberArea != nullptr) {
        lineNumberArea->setFixedWidth(lineNumberAreaWidth());
    }
}

void CodeEditor::paintLineNumbers(QPaintEvent* event) {
    if (lineNumberArea == nullptr) {
        return;
    }

    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), colours.lineNumberBackground);
    painter.setFont(font());
    painter.setPen(colours.lineNumberText);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, lineNumberArea->width() - 4, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        blockNumber++;
    }
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> selections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(colours.currentLineHighlight);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        selections.append(selection);
    }

    if (errorLine > 0) {
        QTextBlock block = document()->findBlockByNumber(errorLine - 1);
        if (block.isValid()) {
            QTextEdit::ExtraSelection errorSelection;
            errorSelection.format.setBackground(colours.errorLineHighlight);
            errorSelection.format.setProperty(QTextFormat::FullWidthSelection, true);
            errorSelection.cursor = QTextCursor(block);
            errorSelection.cursor.clearSelection();
            selections.append(errorSelection);
        }
    }

    setExtraSelections(selections);
}

void CodeEditor::highlightErrorLine(int lineNumber) {
    errorLine = lineNumber;
    highlightCurrentLine();
}
