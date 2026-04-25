/**
 * @file workspace.h
 * @brief Header file for the Workspace class.
 * @author BitCat
 *
 * Workspace manages the draggable code blocks that the player uses
 * to build instructions for the princess. It supports drag-and-drop,
 * block ordering, indentation logic, and block interaction handling.
 */

#include <QWidget>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include "codeBlock.h"

#pragma once

/**
 * Handles interactive code block placement and organization.
 * @brief The Workspace class
 */
class CodeBlock;
/**
 * Workspace class, which handles the code blocks in a way that the user can interact with.
 * @brief The Workspace class
 */
class Workspace : public QWidget {
    Q_OBJECT

public:
    /**
     * Creates a new Workspace widget.
     * @brief Constructs the workspace area
     * @param parent The parent widget, or nullptr if none
     * @return None
     */
    Workspace(QWidget *parent = nullptr);
    /**
     * Clears all the blocks from the workspace.
     * @brief clearBlocks
     */
    void clearBlocks();
    /**
     * Gets a list of the Codeblocks in the workspace.
     * @brief getBlocks
     * @return the list of codeblocks
     */
    QList<CodeBlock*> getBlocks();

    /**
     * Removes a specified block in the workspace.
     * @brief removeBlock
     * @param block the block to remove
     */
    void removeBlock(CodeBlock *block);
    /**
     * Inserts a code block in the middle of the workspace after a specified moveBlock.
     * @brief insertRoutePointAfterMove
     * @param moveBlock the moveblock to insert it after
     * @param routePointBlock the block to move into there
     */
    void insertRoutePointAfterMove(CodeBlock *moveBlock, CodeBlock *routePointBlock);
    /**
     * Reindents all of the blocks.
     * @brief reindentAll
     */
    void reindentAll();

signals:
    /**
     * Triggered when a moveblock is clicked.
     * @brief moveBlockClicked
     * @param block the CodeBlock that is clicked.
     */
    void moveBlockClicked(CodeBlock *block);

protected:
    /**
     * Drag Enter event, which is triggered when a block is dragged into the workspace.
     * @brief dragEnterEvent
     * @param event the QDragEnterEvent
     */
    void dragEnterEvent(QDragEnterEvent *event) override;
    /**
     * The dropevent, which is triggered when a block is dropped into the workspace.
     * @brief dropEvent
     * @param event the QDropEvent.
     */
    void dropEvent(QDropEvent *event) override;
private:
    /**
     * Vertical layout used to organize all workspace blocks.
     * @brief layout
     */
    QVBoxLayout *layout;

    /**
     * Connects block signals after creation or insertion.
     * @brief Registers a block for interaction handling
     * @param block The CodeBlock to register
     * @return None
     */
    void registerBlock(CodeBlock *block);

    /**
     * Stores all CodeBlock objects currently in the workspace.
     * @brief blocks
     */
    QList<CodeBlock*> blocks;
};
