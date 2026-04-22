#include "codeBlock.h"
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QApplication>

CodeBlock::CodeBlock(QString text, QString type, QWidget *parent)
    : CodeBlock(text, type, text, parent)
{
}

CodeBlock::CodeBlock(QString displayText, QString type, QString command, QWidget *parent)
    : QLabel(displayText, parent), blockType(type), command(command)
{
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMinimumHeight(40);
    setMaximumHeight(50);
    setMinimumWidth(120);
    setMaximumWidth(220);

    if (type == "routePoint") {
        setStyleSheet(
            "background-color: lightblue;"
            "font-weight: bold;"
            "border: 2px solid rgba(0,0,0,0.6);"
            "border-bottom: 4px solid rgba(0,0,0,0.25);"
            "border-radius: 12px;"
            "padding: 6px;"
            );
    }
    else if (type == "ifStatement") {
        setStyleSheet(
            "background-color: purple;"
            "color: white;"
            "border: 2px solid rgba(0,0,0,0.6);"
            "border-bottom: 4px solid rgba(0,0,0,0.25);"
            "border-radius: 12px;"
            "padding: 6px;"
            "font-weight: bold;"
            );
        setMinimumWidth(200);
        setMaximumWidth(300);
    }
    else if (type == "forLoop") {
        setStyleSheet(
            "background-color: #1a6b1a;"
            "color: #ccffcc;"
            "border: 2px solid rgba(0,0,0,0.6);"
            "border-bottom: 4px solid rgba(0,0,0,0.25);"
            "border-radius: 12px;"
            "padding: 6px;"
            "font-weight: bold;"
            );
    }
    else if (type == "scopeBracket") {
        setStyleSheet(
            "background-color: #2a2a2a;"
            "color: #ffd700;"
            "border: 2px solid #555555;"
            "border-radius: 6px;"
            "font-size: 18px;"
            "font-weight: bold;"
            );
        setMinimumWidth(40);
        setMaximumWidth(60);
    }
}

QString CodeBlock::getType() const
{
    return blockType;
}

QString CodeBlock::getCommand() const
{
    return command;
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

    mimeData->setText(command);
    mimeData->setText(command);
    mimeData->setData("application/x-displaytext", text().toUtf8());
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
