#include "Workspace.h"
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QLabel>

Workspace::Workspace(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setStyleSheet("background-color: lightgray;");
    layout = new QVBoxLayout(this);
}

void Workspace::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void Workspace::dropEvent(QDropEvent *event)
{
    QString text = event->mimeData()->text();

    QLabel *block = new QLabel(text);
    block->setStyleSheet("background-color: lightgreen; padding: 6px;");
    block->setAlignment(Qt::AlignCenter);

    layout->addWidget(block);

    event->acceptProposedAction();
}
