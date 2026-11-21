#include "ColourPalette.h"

ColourPalette createDarkPalette() {
    ColourPalette palette;
    palette.editorBackground = QColor("#282828");
    palette.lineNumberBackground = QColor("#1D2021");
    palette.lineNumberText = QColor("#928374");
    palette.normalText = QColor("#EBDBB2");
    palette.keywordText = QColor("#D3869B");
    palette.identifierText = QColor("#83A598");
    palette.objectTypeText = QColor("#8EC07C");
    palette.integerText = QColor("#B8BB26");
    palette.floatText = QColor("#FABD2F");
    palette.stringText = QColor("#FE8019");
    palette.operatorText = QColor("#D5C4A1");
    palette.punctuationText = QColor("#EBDBB2");
    palette.currentLineHighlight = QColor("#3C3836");
    palette.selectionBackground = QColor("#504945");
    palette.errorText = QColor("#FB4934");
    palette.errorLineHighlight = QColor("#462222");
    return palette;
}

ColourPalette createLightPallette() {
    ColourPalette palette;
    palette.editorBackground = QColor("#F4ECD8");
    palette.lineNumberBackground = QColor("#E9DFC6");
    palette.lineNumberText = QColor("#A89B80");
    palette.normalText = QColor("#5B4636");
    palette.keywordText = QColor("#9C3D54");
    palette.identifierText = QColor("#2F6F8F");
    palette.objectTypeText = QColor("#4F7A28");
    palette.integerText = QColor("#B5651D");
    palette.floatText = QColor("#B5651D");
    palette.stringText = QColor("#8A6A2F");
    palette.operatorText = QColor("#5B4636");
    palette.punctuationText = QColor("#5B4636");
    palette.currentLineHighlight = QColor("#EBE1C8");
    palette.selectionBackground = QColor("#D8CAA6");
    palette.errorText = QColor("#9D0006");
    palette.errorLineHighlight = QColor("#F5CBC2");
    return palette;
}
