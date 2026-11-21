#pragma once

#include <QSyntaxHighlighter>

#include "ColourPalette.h"

class QTextDocument;

class SyntaxHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    SyntaxHighlighter(QTextDocument* document, ColourPalette colours);

protected:
    void highlightBlock(const QString& text) override;

private:
    ColourPalette colours;
};
