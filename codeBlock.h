#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <QLabel>

class CodeBlock : public QLabel {
    Q_OBJECT

public:
    explicit CodeBlock(QString text, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif
