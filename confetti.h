#ifndef CONFETTI_H
#define CONFETTI_H

#include <QWidget>
#include <QTimer>
#include <vector>
#include <Box2D/Box2D.h>

class Confetti : public QWidget
{
    Q_OBJECT

public:
    Confetti(QWidget *parent = nullptr);
    void startConfetti();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateWorld();
    void spawnConfetti();

private:
    b2World world;

    std::vector<b2Body*> bodies;
    std::vector<QColor> colors;
    std::vector<float> seeds;

    QTimer physicsTimer;
    QTimer spawnTimer;

    int spawnCount = 0;
    const int maxSpawns = 4;
};

#endif
