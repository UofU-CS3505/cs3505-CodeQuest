#include "codeBlock.h"
#include <QDrag>
#include <QMimeData>
#include <QPainter>

CodeBlock::CodeBlock(QString text, QString type, QWidget *parent)
    : QLabel(text, parent), blockType(type)
{
    setAlignment(Qt::AlignCenter);

    if (type == "move") {
        setStyleSheet("background-color: lightgreen; padding: 6px; border: 1px solid black;");
    }
    else if (type == "ifStatement") {
        setStyleSheet("background-color: purple; color: white; padding: 6px; border: 1px solid black;");
    }
    else {
        setStyleSheet("background-color: lightblue; padding: 6px; border: 1px solid black;");
    }
}

QString CodeBlock::getType() const
{
    return blockType;
}

void CodeBlock::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setText(this->text());
        mimeData->setData("application/x-codeblock", blockType.toUtf8());

        drag->setMimeData(mimeData);

        QPixmap pixmap(this->size());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setOpacity(0.7);
        this->render(&painter);


        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width()/2, pixmap.height()/2));

        drag->exec(Qt::CopyAction);
    }
}
