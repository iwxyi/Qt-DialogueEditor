/**
  * 剧本 对话气泡控件
  */

#ifndef DIALOGUEBUBBLE_H
#define DIALOGUEBUBBLE_H

#include <QObject>
#include <QLabel>

class DialogueBubble : public QLabel
{
public:
    DialogueBubble(QWidget* parent = nullptr);
};

#endif // DIALOGUEBUBBLE_H
