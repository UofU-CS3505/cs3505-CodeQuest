/**
 * @file confetti.h
 * @brief Header file for the Confetti class.
 * @author BitCat
 *
 * Confetti creates and manages a celebratory particle animation using
 * Box2D physics. It spawns falling confetti pieces, updates their
 * physics simulation, and renders them on the screen.
 */

#ifndef CONFETTI_H
#define CONFETTI_H

#include <QWidget>
#include <QTimer>
#include <vector>
#include <Box2D/Box2D.h>

/**
 * Displays animated confetti using Box2D physics.
 * @brief The Confetti class
 */
class Confetti : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates a Confetti widget.
     * @brief Constructs the confetti animation widget
     * @param parent The parent widget, or nullptr if none
     * @return None
     */
    Confetti(QWidget *parent = nullptr);

    /**
     * Starts the confetti animation and spawning process.
     * @brief Begins the confetti celebration effect
     * @param None
     * @return None
     */
    void startConfetti();

protected:
    /**
     * Paints all active confetti pieces on the widget.
     * @brief Draws the confetti particles
     * @param event The paint event being processed
     * @return None
     */
    void paintEvent(QPaintEvent *event) override;

private slots:
    /**
     * Updates the Box2D physics world and redraws the widget.
     * @brief Advances the physics simulation
     * @param None
     * @return None
     */
    void updateWorld();

    /**
     * Spawns a new batch of confetti pieces into the world.
     * @brief Creates new confetti particles
     * @param None
     * @return None
     */
    void spawnConfetti();

private:
    /**
     * The Box2D physics world used for confetti simulation.
     * @brief world
     */
    b2World world;

    /**
     * Stores all active Box2D confetti bodies.
     * @brief bodies
     */
    std::vector<b2Body*> bodies;

    /**
     * Stores the display color for each confetti piece.
     * @brief colors
     */
    std::vector<QColor> colors;

    /**
     * Stores random values used for animation variation.
     * @brief seeds
     */
    std::vector<float> seeds;

    /**
     * Timer used to update the physics simulation.
     * @brief physicsTimer
     */
    QTimer physicsTimer;

    /**
     * Timer used to control confetti spawning intervals.
     * @brief spawnTimer
     */
    QTimer spawnTimer;

    /**
     * Tracks how many spawn cycles have occurred.
     * @brief spawnCount
     */
    int spawnCount = 0;

    /**
     * Maximum number of confetti spawn cycles allowed.
     * @brief MAX_SPAWNS
     */
    const int MAX_SPAWNS = 3;
};

#endif // CONFETTI_H
