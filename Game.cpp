#include "game.h"
#include <cstdlib> // For random number generation

Game::Game(QWidget *parent) : QMainWindow(parent), world(nullptr) {
    setFixedSize(800, 600);

    // Create the physics world
    b2Vec2 gravity(0.0f, -10.0f); // Gravity pulling downward
    world = new b2World(gravity);

    setupWorld();

    // Timer for updating the game
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Game::updateGame);
    timer->start(16); // ~60 FPS
}

Game::~Game() {
    delete world;
}

void Game::setupWorld() {
    // Create water boundary (floor)
    b2BodyDef groundDef;
    groundDef.position.Set(0.0f, 0.0f); // At y = 0
    b2Body *ground = world->CreateBody(&groundDef);

    b2EdgeShape groundShape;
    groundShape.Set(b2Vec2(0.0f, 0.0f), b2Vec2(20.0f, 0.0f)); // Horizontal line

    b2FixtureDef groundFixture;
    groundFixture.shape = &groundShape;
    groundFixture.friction = 0.5f;
    ground->CreateFixture(&groundFixture);

    createBoat();
    createFishingLine();
    createFish();
}

void Game::createBoat() {
    b2BodyDef boatDef;
    boatDef.type = b2_kinematicBody;
    boatDef.position.Set(10.0f, 6.0f); // Align with the water surface
    boat = world->CreateBody(&boatDef);

    b2PolygonShape boatShape;
    boatShape.SetAsBox(1.0f, 0.5f);

    b2FixtureDef boatFixture;
    boatFixture.shape = &boatShape;
    boatFixture.density = 1.0f;
    boatFixture.friction = 0.3f;
    boat->CreateFixture(&boatFixture);
}

void Game::createFishingLine() {
    b2BodyDef lineDef;
    lineDef.type = b2_kinematicBody;
    lineDef.position.Set(10.0f, 5.5f); // Start below the boat
    fishingLine = world->CreateBody(&lineDef);

    b2PolygonShape lineShape;
    lineShape.SetAsBox(0.1f, 2.0f); // Thin vertical line

    b2FixtureDef lineFixture;
    lineFixture.shape = &lineShape;
    lineFixture.density = 1.0f;
    fishingLine->CreateFixture(&lineFixture);
}

void Game::createFish() {
    for (int i = 0; i < 5; ++i) {
        b2BodyDef fishDef;
        fishDef.type = b2_dynamicBody;
        fishDef.position.Set(2.0f + i * 3.0f, 4.0f); // Start in the middle of the water
        fishDef.gravityScale = 0.0f; // Disable gravity for fish
        b2Body *fish = world->CreateBody(&fishDef);

        b2CircleShape fishShape;
        fishShape.m_radius = 0.5f;

        b2FixtureDef fishFixture;
        fishFixture.shape = &fishShape;
        fishFixture.density = 1.0f;
        fishFixture.friction = 0.3f;
        fish->CreateFixture(&fishFixture);

        fish->SetLinearDamping(0.5f); // Simulate water resistance
        fishBodies.push_back(fish);
    }
}

void Game::updateGame() {
    world->Step(1.0f / 60.0f, 8, 3); // Step the physics simulation

    // Attach the fishing line to the boat
    b2Vec2 boatPos = boat->GetPosition();
    b2Vec2 linePos = fishingLine->GetPosition();
    fishingLine->SetTransform(b2Vec2(boatPos.x, linePos.y), 0.0f);

    // Limit fishing line movement
    if (linePos.y > 6.0f) {
        fishingLine->SetLinearVelocity(b2Vec2(0, 0)); // Stop at upper limit
    } else if (linePos.y < 1.0f) {
        fishingLine->SetLinearVelocity(b2Vec2(0, 0)); // Stop at lower limit
    }

    // Move fish randomly and keep them within bounds
    for (b2Body *fish : fishBodies) {
        float randomForceX = (rand() % 20 - 10) / 10.0f; // Random horizontal force
        fish->ApplyForceToCenter(b2Vec2(randomForceX, 0), true);

        // Keep fish within bounds
        b2Vec2 fishPos = fish->GetPosition();
        if (fishPos.x < 1.0f || fishPos.x > 19.0f) {
            fish->SetLinearVelocity(b2Vec2(-fish->GetLinearVelocity().x, fish->GetLinearVelocity().y));
        }
    }

    update(); // Trigger Qt repaint
}

void Game::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Draw water
    painter.setBrush(Qt::blue);
    painter.drawRect(0, height() / 2, width(), height() / 2);

    // Draw boat
    b2Vec2 boatPos = boat->GetPosition();
    painter.setBrush(Qt::red);
    painter.drawRect(boatPos.x * SCALE - 50, height() - (boatPos.y * SCALE) - 25, 100, 50);

    // Draw fishing line
    b2Vec2 linePos = fishingLine->GetPosition();
    painter.setBrush(Qt::black);
    painter.drawRect(linePos.x * SCALE - 5, height() - (linePos.y * SCALE), 10, 200);

    // Draw fish
    painter.setBrush(Qt::green);
    for (b2Body *fish : fishBodies) {
        b2Vec2 fishPos = fish->GetPosition();
        painter.drawEllipse(fishPos.x * SCALE - 25, height() - (fishPos.y * SCALE) - 25, 50, 50);
    }
}

void Game::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left) {
        boat->SetLinearVelocity(b2Vec2(-5, 0)); // Move left
    } else if (event->key() == Qt::Key_Right) {
        boat->SetLinearVelocity(b2Vec2(5, 0)); // Move right
    }
}

void Game::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right) {
        boat->SetLinearVelocity(b2Vec2(0, 0)); // Stop movement
    }
}

void Game::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        fishingLine->SetLinearVelocity(b2Vec2(0, -2)); // Retract line
    } else if (event->button() == Qt::RightButton) {
        fishingLine->SetLinearVelocity(b2Vec2(0, 2)); // Extend line
    }
}
