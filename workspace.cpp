#include "workspace.h"
#include "codeBlock.h"
#include <QMimeData>

Workspace::Workspace(QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setStyleSheet("background-color: lightgray;");

    layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    setLayout(layout);
}

void Workspace::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-codeblock")) {
        event->acceptProposedAction();
    }
}

void Workspace::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-codeblock")) {

        QString text = event->mimeData()->text();
        QString type = event->mimeData()->data("application/x-codeblock");

        CodeBlock *block = new CodeBlock(text, type);
        layout->addWidget(block);

        event->acceptProposedAction();
    }
}
