#pragma once

#include <QLabel>
#include <QMouseEvent>

class CodeBlock : public QLabel {
    Q_OBJECT

public:
    CodeBlock(QString text, QString type, QWidget *parent = nullptr);

    QString getType() const;

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString blockType;
};
