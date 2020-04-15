#include "dialogueeditor.h"

DialogueEditor::DialogueEditor(QWidget *parent) : QWidget(parent)
{
    initView();
    initStyle();

    setBucket(nullptr);
}

void DialogueEditor::initView()
{
    avatar_btn = new QPushButton(this);
    name_label = new QLabel("名字", this);
    name_edit = new QLineEdit(this);
    said_label = new QLabel("说的话/旁白", this);
    said_edit = new QPlainTextEdit(this);
    style_label = new QLabel("样式表", this);
    style_edit = new QPlainTextEdit(this);
    name_check = new QCheckBox("显示名字", this);
    delete_bucket_button = new QPushButton("删除此行", this);
    export_picture_button = new QPushButton("导出对话", this);
    save_figure_button = new QPushButton("保存角色", this);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(avatar_btn);
    vlayout->addWidget(name_label);
    vlayout->addWidget(name_edit);
    vlayout->addWidget(said_label);
    vlayout->addWidget(said_edit);
    vlayout->addWidget(style_label);
    vlayout->addWidget(style_edit);
    vlayout->addWidget(name_check);
    QHBoxLayout *button_hlayout = new QHBoxLayout;
    button_hlayout->addWidget(save_figure_button);
    button_hlayout->addWidget(delete_bucket_button);
    button_hlayout->addWidget(export_picture_button);
    vlayout->addLayout(button_hlayout);

    vlayout->setMargin(0);

    connect(avatar_btn, &QPushButton::clicked, this, [=]{
        QString path = QFileDialog::getOpenFileName(this, "选择头像", "", tr("Images (*.png *.xpm *.jpg)"));
        if (path.isEmpty())
            return ;
        QPixmap pixmap(path);
        avatar_btn->setIconSize(pixmap.size());
        avatar_btn->setIcon(pixmap);
        current_bucket->setAvatar(pixmap);
    });
    connect(name_edit, &QLineEdit::textEdited, this, [=]{
        if (!current_bucket)
            return ;
        current_bucket->nickname->setText(name_edit->text());
    });
    connect(said_edit, &QPlainTextEdit::textChanged, this, [=]{
       if (!current_bucket)
           return ;
       QLabel* label = current_bucket->isNarrator()
               ? (QLabel*)current_bucket->narrator
               : (QLabel*)current_bucket->bubble;
       label->setText(said_edit->toPlainText());
       label->adjustSize();
       emit current_bucket->signalBubbleChanged();
    });
    connect(style_edit, &QPlainTextEdit::textChanged, this, [=]{
       if (!current_bucket)
           return ;
       current_bucket->setStyleSheet(style_edit->toPlainText());
    });
    connect(name_check, &QCheckBox::stateChanged, this, [=](int){
        if (!current_bucket)
            return ;
        current_bucket->setNameVisible(name_check->isChecked());
    });

    connect(delete_bucket_button, &QPushButton::clicked, this, [=]{
        emit signalDelete();
    });
    connect(save_figure_button, &QPushButton::clicked, this, [=]{
        emit signalSaveFigure(current_bucket);
    });
    connect(export_picture_button, &QPushButton::clicked, this, [=]{
        emit signalSaveToFile();
    });
}

void DialogueEditor::initStyle()
{
}

void DialogueEditor::setBucket(DialogueBucket *bucket)
{
    current_bucket = bucket;
    if (bucket == nullptr)
    {
        name_label->setEnabled(false);
        name_edit->setEnabled(false);
        said_label->setEnabled(false);
        said_edit->setEnabled(false);
        style_label->setEnabled(false);
        style_edit->setEnabled(false);
        name_check->setEnabled(false);
        save_figure_button->setEnabled(false);
        delete_bucket_button->setEnabled(false);
        return ;
    }

    style_edit->setPlainText(bucket->styleSheet());
    if (bucket->isNarrator())
    {
        name_label->setEnabled(false);
        name_edit->setEnabled(false);
        said_label->setEnabled(true);
        said_edit->setEnabled(true);
        style_label->setEnabled(true);
        style_edit->setEnabled(true);
        name_check->setEnabled(false);
        save_figure_button->setEnabled(true);
        delete_bucket_button->setEnabled(false);

        said_edit->setPlainText(bucket->narrator->text());
    }
    else
    {
        name_label->setEnabled(true);
        name_edit->setEnabled(true);
        said_label->setEnabled(true);
        said_edit->setEnabled(true);
        style_label->setEnabled(true);
        style_edit->setEnabled(true);
        name_check->setEnabled(true);
        save_figure_button->setEnabled(true);
        delete_bucket_button->setEnabled(true);

        QPixmap pixmap = *bucket->avatar->pixmap();
        avatar_btn->setIcon(QIcon(pixmap));
        avatar_btn->setIconSize(pixmap.size());
        name_edit->setText(bucket->nickname->text());
        said_edit->setPlainText(bucket->bubble->text());
    }
}

void DialogueEditor::focusSaid()
{
    QTimer::singleShot(100, [=]{
        said_edit->setFocus();
        said_edit->selectAll();
    });
}
