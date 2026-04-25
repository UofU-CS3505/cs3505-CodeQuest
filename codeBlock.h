/**
 * @file codeBlock.h
 * @brief Header file for the CodeBlock class.
 * @author BitCat
 *
 * CodeBlock represents a single draggable instruction block used in the
 * programming interface. Each block stores its display text, type,
 * command behavior, and drag interaction logic.
 */

#pragma once

#include <QLabel>
#include <QMouseEvent>

/**
 * Represents a draggable code instruction block.
 * @brief The CodeBlock class
 */
class CodeBlock : public QLabel
{
    Q_OBJECT

public:
    /**
     * Creates a CodeBlock using the displayed text as the command.
     * @brief Constructs a CodeBlock with default command behavior
     * @param text The text displayed on the code block
     * @param type The type of block (move, if, loop, etc.)
     * @param parent The parent widget, or nullptr if none
     * @return None
     */
    CodeBlock(QString text, QString type, QWidget *parent = nullptr);

    /**
     * Creates a CodeBlock with a separate command value.
     * @brief Constructs a CodeBlock with a custom command
     * @param text The text displayed on the code block
     * @param type The type of block (move, if, loop, etc.)
     * @param command The command the princess will follow
     * @param parent The parent widget, or nullptr if none
     * @return None
     */
    CodeBlock(QString text,
              QString type,
              QString command,
              QWidget *parent = nullptr);

    /**
     * Returns the type of this CodeBlock.
     * @brief Gets the block type
     * @param None
     * @return The block type as a QString
     */
    QString getType() const;

    /**
     * Returns the command associated with this CodeBlock.
     * @brief Gets the command for the block
     * @param None
     * @return The command as a QString
     */
    QString getCommand() const;

    /**
     * Highlights or restores the block during code execution.
     * @brief Sets the highlighted state of the block
     * @param highlighted True to highlight, false to restore original style
     * @return None
     */
    void setHighlighted(bool highlighted);

signals:
    /**
     * Emitted when the block is clicked without being dragged.
     * @brief Broadcasts a block click event
     * @param block Pointer to the clicked CodeBlock
     * @return None
     */
    void blockClicked(CodeBlock *block);

protected:
    /**
     * Handles mouse press events for drag setup.
     * @brief Stores the initial mouse position
     * @param event The mouse press event
     * @return None
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * Handles mouse movement for drag-and-drop behavior.
     * @brief Starts dragging when movement exceeds threshold
     * @param event The mouse move event
     * @return None
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Handles mouse release events for click detection.
     * @brief Emits blockClicked when released without dragging
     * @param event The mouse release event
     * @return None
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    /**
     * Stores the type of block (move, if, loop, etc.).
     * @brief blockType
     */
    QString blockType;

    /**
     * Stores the starting mouse position when dragging begins.
     * @brief dragStartPos
     */
    QPoint dragStartPos;

    /**
     * Tracks whether the block has been dragged.
     * @brief dragged
     */
    bool dragged = false;

    /**
     * Stores the command the princess will follow.
     * @brief command
     */
    QString command;

    /**
     * Stores the original stylesheet for restoring after highlight.
     * @brief originalStyle
     */
    QString originalStyle;
};
