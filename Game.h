#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <Box2D/Box2D.h>

// The Game class handles the physics simulation and rendering of the game.
class Game : public QWidget {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);  // Constructor

    // Setter function to modify the ball's starting position
    void setBallStartPosition(float x, float y);

    // Setter function to modify the ground's position
    void setGroundPosition(float x1, float y1, float x2, float y2);

protected:
    // Qt event to handle rendering
    void paintEvent(QPaintEvent *event) override;

    // Mouse events for interacting with the game
    void mousePressEvent(QMouseEvent *event) override;  // When the mouse button is pressed
    void mouseMoveEvent(QMouseEvent *event) override;   // When the mouse is moved
    void mouseReleaseEvent(QMouseEvent *event) override;  // When the mouse button is released

private:
    b2World world;  // The Box2D world where physics simulation happens
    b2Body* throwableBody;  // The throwable object (e.g., a ball)
    b2Body* groundBody;  // The static ground object

    b2Vec2 startingPosition;  // Starting position of the throwable object
    b2Vec2 initialVelocity;  // Initial velocity when the object is thrown
    b2Vec2 dragStart;  // Starting point of the drag (mouse press)
    b2Vec2 dragEnd;  // Ending point of the drag (mouse release)

    bool isDragging;  // Flag to check if the user is currently dragging

    void createThrowableBody();  // Function to create the throwable object
    void createGround(float x1, float y1, float x2, float y2);  // Function to create the static ground
    b2Vec2 getTrajectoryPoint(const b2Vec2& startPos, const b2Vec2& startVel, float step) const;
    // Helper function to calculate the trajectory points
};

#endif // GAME_H
