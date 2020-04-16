#include "dialoguefigure.h"

DialogueFigure::DialogueFigure(QObject *parent) : QObject(parent)
{

}

bool DialogueFigure::isNarrator()
{
    return type == NarrChat;
}
