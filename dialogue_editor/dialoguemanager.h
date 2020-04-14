#ifndef DIALOGUEMANAGER_H
#define DIALOGUEMANAGER_H

#include <QList>
#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QSettings>
#include <QDebug>
#include "dialoguefigure.h"
#include "dialoguebucket.h"

class DialogueManager : public QObject
{
    Q_OBJECT
public:
    DialogueManager(QObject *parent = nullptr);

    void setDataDir(QString dir);
    void loadData();
    void saveData(DialogueFigure* figure);

    void saveFigure(DialogueBucket* bucket);
    void deleteFigure(DialogueFigure* figure);

    QList<DialogueFigure*>& getFigures();
    DialogueFigure* getFigureByName(QString name);
    DialogueFigure* getFigureById(QString id);

private:
    DialogueFigure* createFigure(ChatType t, QString n, QPixmap a, QString ss, QString id = "");
    DialogueFigure* createFigure(ChatType t, QString ss, QString id = "");
    QString createFigureID();

private:
    static QString readTextFile(QString path);

    static bool writeTextFile(QString path, QString text);

signals:

public slots:

protected:
    QString data_dir; // 数据路径（末尾带/）
    QList<DialogueFigure*> figures; // 人物模板信息
    QString dialogue_qss, bucket_qss, narrator_qss; // 各种样式表
};

#endif // DIALOGUEMANAGER_H
