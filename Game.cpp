#include "Game.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>

// Constructor: Initializes the Box2D world and game objects
Game::Game(QWidget *parent)
    : QWidget(parent),
    world(b2Vec2(0.0f, -10.0f)),  // Initialize the Box2D world with gravity (-10 m/s²)
    throwableBody(nullptr),
    groundBody(nullptr),
    isDragging(false) {

    // Create the ground object
    createGround();

    // Create the throwable object (e.g., a ball)
    createThrowableBody();

    // Timer to update the physics simulation at ~60 FPS
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (!isDragging) {  // Only update the physics when the object is not being dragged
            world.Step(1.0f / 60.0f, 6, 2);  // Step the physics simulation forward by 1/60th of a second
            update();  // Request the widget to redraw itself
        }
    });
    timer->start(16);  // 16 milliseconds per update (60 FPS)
}

// Creates the static ground object
void Game::createGround() {
    // Define the ground body
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f);  // Ground is located at the bottom of the world
    groundBody = world.CreateBody(&groundBodyDef);  // Add the ground to the Box2D world

    // Define the shape of the ground as a straight edge
    b2EdgeShape groundShape;
    groundShape.Set(b2Vec2(0.0f, 0.0f), b2Vec2(25.0f, 0.0f));  // 25 meters wide

    // Attach the shape to the ground body
    groundBody->CreateFixture(&groundShape, 0.0f);  // 0.0f means no density (static object)
}

// Creates the throwable object (e.g., a ball)
void Game::createThrowableBody() {
    // Define the throwable object's body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;  // Dynamic body can move and be affected by forces
    bodyDef.position.Set(5.0f, 10.0f);  // Initial position (5 meters right, 10 meters up)
    throwableBody = world.CreateBody(&bodyDef);  // Add the body to the Box2D world

    // Define the shape of the throwable object as a circle
    b2CircleShape shape;
    shape.m_radius = 0.5f;  // Radius of 0.5 meters

    // Create a fixture to define the object's physical properties
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;  // Density affects mass
    fixtureDef.friction = 0.3f;  // Friction affects sliding
    fixtureDef.restitution = 0.5f;  // Restitution affects bounciness
    throwableBody->CreateFixture(&fixtureDef);  // Attach the fixture to the body
}

// Handles the rendering of the game
void Game::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);  // Ignore the unused parameter

    QPainter painter(this);  // Create a QPainter for drawing
    painter.setRenderHint(QPainter::Antialiasing);  // Enable smooth rendering

    float scale = 30.0f;  // Convert Box2D meters to pixels (1 meter = 30 pixels)

    // === DRAW THE GROUND ===
    painter.setPen(QPen(Qt::green, 3));  // Green line with thickness 3
    painter.drawLine(
        0, height() - 0.0f * scale,  // Starting point (left side of screen)
        width(), height() - 0.0f * scale  // Ending point (right side of screen)
        );

    // === DRAW THE THROWABLE OBJECT ===
    b2Vec2 position = throwableBody->GetPosition();  // Get the position from Box2D
    float radius = 0.5f * scale;  // Convert radius from meters to pixels
    painter.setBrush(Qt::blue);  // Fill the object with blue color
    painter.drawEllipse(
        QPointF(position.x * scale, height() - position.y * scale),  // Center of the circle
        radius, radius  // Radii of the circle
        );

    // === DRAW THE TRAJECTORY (IF DRAGGING) ===
    if (isDragging) {
        painter.setPen(QPen(Qt::red, 2));  // Red line with thickness 2
        for (int i = 0; i < 180; ++i) {  // Predict the trajectory for 3 seconds (180 steps)
            b2Vec2 trajectoryPoint = getTrajectoryPoint(startingPosition, initialVelocity, i);
            float x = trajectoryPoint.x * scale;  // Convert X-coordinate to pixels
            float y = height() - trajectoryPoint.y * scale;  // Convert Y-coordinate to pixels
            painter.drawPoint(QPointF(x, y));  // Draw a red point at the trajectory position
        }
    }
}

// Handles mouse press events
void Game::mousePressEvent(QMouseEvent *event) {
    float scale = 30.0f;  // Convert pixels to Box2D meters
    dragStart.Set(event->pos().x() / scale, (height() - event->pos().y()) / scale);  // Record drag start position
    startingPosition = throwableBody->GetPosition();  // Record the current position of the object
    isDragging = true;  // Start dragging
}

// Handles mouse move events
void Game::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        float scale = 30.0f;  // Convert pixels to Box2D meters
        dragEnd.Set(event->pos().x() / scale, (height() - event->pos().y()) / scale);  // Record drag end position
        initialVelocity = 10.0f * (dragEnd - dragStart);  // Calculate velocity based on drag
        update();  // Redraw the widget to update the trajectory
    }
}

// Handles mouse release events
void Game::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);  // Ignore the unused parameter

    if (isDragging) {
        isDragging = false;  // Stop dragging
        throwableBody->SetLinearVelocity(initialVelocity);  // Apply the calculated velocity to the object
        dragStart.SetZero();  // Reset drag start
        dragEnd.SetZero();  // Reset drag end
    }
}

// Calculates the position of the object at a future time step
b2Vec2 Game::getTrajectoryPoint(const b2Vec2& startPos, const b2Vec2& startVel, float step) const {
    float t = 1.0f / 60.0f;  // Time step (60 FPS)
    b2Vec2 stepVelocity = t * startVel;  // Velocity at each time step
    b2Vec2 stepGravity = t * t * world.GetGravity();  //
    return startPos + step * stepVelocity + 0.5f * (step * step + step) * stepGravity;  // Position formula
}
