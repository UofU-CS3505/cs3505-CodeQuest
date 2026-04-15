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
    if (!event->mimeData()->hasFormat("application/x-codeblock"))
        return;

    QString text = event->mimeData()->text();
    QString type = QString::fromUtf8(event->mimeData()->data("application/x-codeblock"));

    CodeBlock *sourceBlock = qobject_cast<CodeBlock*>(event->source());

    CodeBlock *block = nullptr;

    if (sourceBlock && sourceBlock->parentWidget() == this) {
        block = sourceBlock;

        layout->removeWidget(block);
    }

    else {
        block = new CodeBlock(text, type);
    }

    int insertIndex = layout->count();

    QPoint dropPosition = event->position().toPoint();

    for (int i = 0; i < layout->count(); i++) {
        QWidget *w = layout->itemAt(i)->widget();
        if (!w) continue;

        int middleY = w->y() + w->height() / 2;

        if (dropPosition.y() < middleY) {
            insertIndex = i;
            break;
        }
    }

    layout->insertWidget(insertIndex, block);

    event->acceptProposedAction();
}
