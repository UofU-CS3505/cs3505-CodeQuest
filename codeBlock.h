#pragma once

#include <QLabel>
#include <QMouseEvent>
/**
 * Codeblock class, which contains a single instruction of code for the user to drag over.
 * @brief The CodeBlock class
 */
class CodeBlock : public QLabel {
    Q_OBJECT

public:
    /**
     * @brief CodeBlock Constructor for the Codeblock type.
     * @param text the text displayed on the codeblock
     * @param type Sets the block type
     * @param parent
     */
    CodeBlock(QString text, QString type, QWidget *parent = nullptr);

    /**
     * @brief getType Gets the type of this codeblock (move, if, etc.)
     * @return blockType (QString)
     */
    QString getType() const;

signals:
    /**
     * @brief blockClicked Broadcasted when the block is clicked.
     * @param block
     */
    void blockClicked(CodeBlock *block);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    /**
     * @brief blockType The type of block (move, if, etc.)
     */
    QString blockType;
    /**
     *
     * @brief dragStartPos
     */
    QPoint dragStartPos;
    /**
     * @brief dragged Whether the block is being dragged
     */
    bool dragged = false;
};
