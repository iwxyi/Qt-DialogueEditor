#include "dialoguefigure.h"

DialogueFigure::DialogueFigure(QObject *parent) : QObject(parent)
{

}

bool DialogueFigure::isNarrator()
{
    return type == NarrChat;
}

void DialogueFigure::setLineReg(QString s)
{
    line_reg.setPattern(s);
}
