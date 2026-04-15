#include "codeBlock.h"
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QApplication>

CodeBlock::CodeBlock(QString text, QString type, QWidget *parent)
    : QLabel(text, parent), blockType(type)
{
    setAlignment(Qt::AlignCenter);
    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(40);
    setMaximumHeight(50);

    if (type == "move") {
        setStyleSheet("background-color: lightgreen; padding: 6px; border: 1px solid black;");
    }
    else if (type == "ifStatement") {
        setStyleSheet("background-color: purple; color: white; padding: 6px; border: 1px solid black;");
    }
    else if (type == "turnLeft") {
        setStyleSheet("background-color: lightblue; padding: 6px; border: 1px solid black;");
    }
    else if (type == "turnRight") {
        setStyleSheet("background-color: khaki; padding: 6px; border: 1px solid black;");
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
        dragStartPos = event->pos();
        dragged = false;
    }

    QLabel::mousePressEvent(event);
}

void CodeBlock::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - dragStartPos).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    dragged = true;
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
    Qt::DropAction action = Qt::CopyAction;
    if (parentWidget() && parentWidget()->inherits("Workspace")) {
    action = Qt::MoveAction;
    }

    drag->exec(action);
}

void CodeBlock::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !dragged) {
        emit blockClicked(this);
    }

    QLabel::mouseReleaseEvent(event);
}
