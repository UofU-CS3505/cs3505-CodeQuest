#include "confetti.h"
#include <QPainter>
#include <QTime>
#include <cstdlib>
#include <cmath>

Confetti::Confetti(QWidget *parent)
    : QWidget(parent), world(b2Vec2(0.0f, 3.0f))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(&physicsTimer, &QTimer::timeout,
            this, &Confetti::updateWorld);

    connect(&spawnTimer, &QTimer::timeout,
            this, &Confetti::spawnConfetti);
}

void Confetti::startConfetti()
{
    spawnCount = 0;
    physicsTimer.start(16);
    spawnTimer.start(100);
}

void Confetti::spawnConfetti()
{
    if (spawnCount >= MAX_SPAWNS) {
        spawnTimer.stop();
        return;
    }

    spawnCount++;

    float centerX = width() / 200.0f;
    float centerY = 0.2f;

    for (int i = 0; i < 18; i++) {
        b2BodyDef def;
        def.type = b2_dynamicBody;
        def.position.Set(centerX, centerY);

        b2Body* body = world.CreateBody(&def);

        b2PolygonShape shape;
        shape.SetAsBox(0.03f, 0.08f);

        b2FixtureDef fix;
        fix.shape = &shape;
        fix.density = 1.0f;
        fix.friction = 0.3f;
        fix.restitution = 0.25f;

        body->CreateFixture(&fix);
        body->SetLinearDamping(1.4f);
        body->SetAngularDamping(2.2f);

        float angle = ((rand() % 360) * M_PI) / 180.0f;
        float speed = 3.5f + (rand() % 100) / 100.0f * 10.0f;
        b2Vec2 velocity(
            cos(angle) * speed,
            sin(angle) * speed - 3.5f
            );

        body->SetLinearVelocity(velocity);
        body->SetAngularVelocity(((rand() % 200) - 100) / 300.0f);
        bodies.push_back(body);

        colors.push_back(QColor(
            200 + rand() % 55,
            rand() % 255,
            rand() % 255
            ));

        seeds.push_back((rand() % 1000) / 100.0f);
    }
}

void Confetti::updateWorld()
{
    world.Step(1.0f / 60.0f, 6, 2);
    update();
}

void Confetti::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    for (size_t i = 0; i < bodies.size(); i++) {

        b2Vec2 pos = bodies[i]->GetPosition();
        float angle = bodies[i]->GetAngle();

        int x = pos.x * 100;
        int y = pos.y * 100;

        p.save();
        p.translate(x, y);
        p.rotate(angle * 180.0 / M_PI);
        p.setPen(Qt::NoPen);
        p.setBrush(colors[i]);
        p.drawRect(-4, -8, 8, 16);
        p.restore();
    }
}
