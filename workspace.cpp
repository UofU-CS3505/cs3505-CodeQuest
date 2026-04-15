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

void Workspace::registerBlock(CodeBlock *block)
{
    connect(block, &CodeBlock::blockClicked,
            this,
            [this](CodeBlock *clickedBlock) {
                if (clickedBlock->getType() == "move") {
                    emit moveBlockClicked(clickedBlock);
                }
            });
}

void Workspace::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-codeblock")) {

        QString text = event->mimeData()->text();
        QString type = event->mimeData()->data("application/x-codeblock");

        if (event->dropAction() == Qt::MoveAction) {
            CodeBlock *block = qobject_cast<CodeBlock*>(event->source());
            if (block) {
                layout->addWidget(block);
            }
        } else {
            CodeBlock *block = new CodeBlock(text, type);
            layout->addWidget(block);
            registerBlock(block);
        }

        event->acceptProposedAction();
    }
}
