#pragma once

#include <QLabel>
#include <QMouseEvent>

class CodeBlock : public QLabel {
    Q_OBJECT

public:
    explicit CodeBlock(QString text, QString type, QWidget *parent = nullptr);

    QString getType() const;

signals:
    void blockClicked(CodeBlock *block);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString blockType;
    QPoint dragStartPos;
    bool dragged = false;
};
