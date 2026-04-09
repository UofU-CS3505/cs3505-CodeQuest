#include "CodeBlock.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

CodeBlock::CodeBlock(QString text, QWidget *parent)
    : QLabel(text, parent)
{
    setStyleSheet("background-color: lightblue; padding: 6px;");
    setAlignment(Qt::AlignCenter);
}

void CodeBlock::mousePressEvent(QMouseEvent *event)
{
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText(this->text());
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction);
}
