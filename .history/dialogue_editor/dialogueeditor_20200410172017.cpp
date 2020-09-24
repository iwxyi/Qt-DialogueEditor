#include "dialogueeditor.h"

DialogueEditor::DialogueEditor(QWidget *parent) : QWidget(parent)
{
    initView();
    initStyle();
}

void DialogueEditor::initView()
{
    name_label = new QLabel("名字", this);
    name_edit = new QLineEdit(this);
    said_label = new QLabel("说的话", this);
    said_edit = new QPlainTextEdit(this);
    narr_label = new QLabel("旁白", this);
    narr_edit = new QPlainTextEdit(this);
    style_label = new QLabel("样式表", this);
    style_edit = new QPlainTextEdit(this);
    name_check = new QCheckBox("显示名字", this);
    export_picture_button = new QPushButton("导出图片", this);
    export_text_button = new QPushButton("导出文字", this);
    delete_bucket_button = new QPushButton("删除对话", this);

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->addWidget(name_label);
    vlayout->addWidget(name_edit);
    vlayout->addWidget(said_label);
    vlayout->addWidget(said_edit);
    vlayout->addWidget(narr_label);
    vlayout->addWidget(narr_edit);
    vlayout->addWidget(style_label);
    vlayout->addWidget(style_edit);
    vlayout->addWidget(name_check);
    QHBoxLayout* button_hlayout = new QHBoxLayout;
    button_hlayout->addWidget(export_picture_button);
    button_hlayout->addWidget(export_text_button);
    button_hlayout->addWidget(delete_bucket_button);
    vlayout->addLayout(button_hlayout);

    vlayout->setMargin(0);
}

void DialogueEditor::initStyle()
{

}

void DialogueEditor::setBucket(DialogueBucket *bucket)
{
    current_bucket = bucket;
    if (bucket == nullptr)
    {

    }

    if (bucket->isNarrator())
    {
    }
    else
    {

    }
}
