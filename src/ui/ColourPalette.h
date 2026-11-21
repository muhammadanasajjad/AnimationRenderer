#pragma once

#include <QColor>

struct ColourPalette {
    QColor editorBackground;
    QColor lineNumberBackground;
    QColor lineNumberText;
    QColor normalText;
    QColor keywordText;
    QColor identifierText;
    QColor objectTypeText;
    QColor integerText;
    QColor floatText;
    QColor stringText;
    QColor operatorText;
    QColor punctuationText;
    QColor currentLineHighlight;
    QColor selectionBackground;
    QColor errorText;
    QColor errorLineHighlight;
};

ColourPalette createDarkPalette();
ColourPalette createLightPallette();
