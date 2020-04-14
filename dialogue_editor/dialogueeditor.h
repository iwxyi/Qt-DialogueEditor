/**
 * 剧本 bucket 编辑器类
 */

#ifndef DIALOGUEEDITOR_H
#define DIALOGUEEDITOR_H

#include <QObject>
#include <QWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDebug>
#include "dialoguebucket.h"

class DialogueEditor : public QWidget
{
    Q_OBJECT
public:
    DialogueEditor(QWidget *parent = nullptr);

    void initView();
    void initStyle();
    void setBucket(DialogueBucket* bucket);

signals:
    void signalDelete();
    void signalExportPicture();
    void signalSaveFigure(DialogueBucket* bucket);

public slots:

private:
    DialogueBucket* current_bucket = nullptr; // 正在编辑的 bucket

    DialogueAvatar *avatar_label;
    QLabel *name_label, *said_label, *style_label;
    QPlainTextEdit *said_edit, *style_edit;
    QLineEdit *name_edit;
    QCheckBox *name_check;

    QPushButton *save_figure_button;
    QPushButton *export_picture_button;
    QPushButton *delete_bucket_button;
};

#endif // DIALOGUEEDITOR_H
