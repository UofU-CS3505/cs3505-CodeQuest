/**
 * @file blockSpace.h
 * @brief Header file for the BlockSpace class.
 * @author BitCat
 *
 * BlockSpace provides the area where code blocks can be dropped
 * and arranged. It manages drag-and-drop behavior for CodeBlock
 * objects and stores them using a vertical layout.
 */

#ifndef BLOCKSPACE_H
#define BLOCKSPACE_H

#include <QWidget>
#include <QVBoxLayout>
#include "codeBlock.h"

class BlockSpace : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates a new BlockSpace widget.
     * @brief Constructs the block workspace area
     * @param parent The parent widget, or nullptr if none
     * @return None
     */
    BlockSpace(QWidget *parent = nullptr);

    /**
     * Adds a CodeBlock widget to the BlockSpace layout.
     * @brief Inserts a code block into the block area
     * @param block Pointer to the CodeBlock being added
     * @return None
     */
    void addBlock(CodeBlock *block);

    /**
     * Returns the layout used to organize blocks.
     * @brief Gets the vertical layout of the block space
     * @param None
     * @return Pointer to the QVBoxLayout managing the blocks
     */
    QVBoxLayout* getLayout();

protected:
    /**
     * Handles drag enter events for incoming draggable blocks.
     * @brief Accepts valid dragged CodeBlock objects
     * @param event The drag enter event being processed
     * @return None
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * Handles drop events when a block is dropped into the space.
     * @brief Removes the block from its old parent and accepts the drop
     * @param event The drop event being processed
     * @return None
     */
    void dropEvent(QDropEvent *event) override;

private:
    /// Layout used to store and display CodeBlock widgets vertically
    QVBoxLayout *layout;
};

#endif // BLOCKSPACE_H
