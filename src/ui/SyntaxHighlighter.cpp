#include "SyntaxHighlighter.h"

#include "../language/Tokeniser.h"

#include <QString>
#include <QTextCharFormat>

bool isKeyword(QString value) {
    if (value == "SETTINGS") return true;
    if (value == "PRELOAD") return true;
    if (value == "SCENE") return true;
    if (value == "play") return true;
    if (value == "fadeIn") return true;
    if (value == "fadeOut") return true;
    if (value == "rotate") return true;
    if (value == "move") return true;
    if (value == "scale") return true;
    if (value == "show") return true;
    if (value == "edit") return true;
    if (value == "wait") return true;
    if (value == "over") return true;
    if (value == "using") return true;
    if (value == "as") return true;
    return false;
}

QTextCharFormat formatWithColour(QColor colour) {
    QTextCharFormat format;
    format.setForeground(colour);
    return format;
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* document, ColourPalette colours)
    : QSyntaxHighlighter(document), colours(colours) {
}

void SyntaxHighlighter::highlightBlock(const QString& text) {
    Tokeniser tokeniser(text.toStdString());
    tokeniser.tokenise(false);
    std::vector<Token> tokens = tokeniser.getTokens();

    for (Token token : tokens) {
        int length = token.value.length();
        if (token.type == StringLiteral) {
            length = length + 2;
        }

        if (length > 0 && token.type != FileEnd) {
            QTextCharFormat format;

            if (isKeyword(QString::fromStdString(token.value))) {
                format = formatWithColour(colours.keywordText);
            } else if (token.type == ObjectType) {
                format = formatWithColour(colours.objectTypeText);
            } else if (token.type == Identifier) {
                format = formatWithColour(colours.identifierText);
            } else if (token.type == IntegerLiteral) {
                format = formatWithColour(colours.integerText);
            } else if (token.type == FloatLiteral) {
                format = formatWithColour(colours.floatText);
            } else if (token.type == StringLiteral) {
                format = formatWithColour(colours.stringText);
            } else if (token.type == Equals || token.type == Caret || token.type == Asterisk ||
                       token.type == ForwardSlash || token.type == Plus || token.type == Minus) {
                format = formatWithColour(colours.operatorText);
            } else {
                format = formatWithColour(colours.punctuationText);
            }

            setFormat(token.columnNumber - 1, length, format);
        }
    }
}
