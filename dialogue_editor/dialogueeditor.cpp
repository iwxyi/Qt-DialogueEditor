#include "dialogueeditor.h"

DialogueEditor::DialogueEditor(QWidget *parent) : QWidget(parent)
{
    initView();
    initStyle();
    initData();
}

void DialogueEditor::initView()
{
    dialogues_list_widget = new QListWidget(this);
    left_button = new QPushButton("+左边", this);
    mid_button = new QPushButton("+旁白", this);
    right_button = new QPushButton("+右边", this);

    QVBoxLayout* main_vlayout = new QVBoxLayout(this);
    main_vlayout->addWidget(dialogues_list_widget);
    QHBoxLayout* button_hlayout = new QHBoxLayout;
    button_hlayout->addWidget(left_button);
    button_hlayout->addWidget(mid_button);
    button_hlayout->addWidget(right_button);
    main_vlayout->addLayout(button_hlayout);
    setLayout(main_vlayout);

    connect(left_button, SIGNAL(clicked()), this, SLOT(slotAddLeftChat()));
    connect(mid_button, SIGNAL(clicked()), this, SLOT(slotAddNarrator()));
    connect(right_button, SIGNAL(clicked()), this, SLOT(slotAddRightChat()));
}

void DialogueEditor::initStyle()
{

}

void DialogueEditor::initData()
{

}

void DialogueEditor::slotAddLeftChat()
{
    auto bucket = new DialogueBucket(OppoChat, "名字", QPixmap(":/avatars/girl_1"), "说的话", this);
    addChat(bucket);
}

void DialogueEditor::slotAddNarrator()
{
    addChat(new DialogueBucket("旁白", this));
}

void DialogueEditor::slotAddRightChat()
{
    auto bucket = new DialogueBucket(SelfChat, "我", QPixmap(":/avatars/boy_1"), "说的话", this);
    bucket->setNameVisible(false);
    addChat(bucket);
}

void DialogueEditor::addChat(DialogueBucket *bucket, int row)
{
    QListWidgetItem* item = new QListWidgetItem;
    if (row > -1)
    {
        buckets.insert(row, bucket);
        dialogues_list_widget->insertItem(row, item);
    }
    else
    {
        buckets.append(bucket);
        dialogues_list_widget->addItem(item);
    }
    dialogues_list_widget->setItemWidget(item, bucket);
    item->setSizeHint(bucket->size());
    bucket->show();
}
