#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <Box2D/Box2D.h>
#include <vector>

class Game : public QMainWindow {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void updateGame();

private:
    QTimer *timer;
    b2World *world;
    b2Body *boat;
    b2Body *fishingLine;
    std::vector<b2Body *> fishBodies;

    void setupWorld();
    void createBoat();
    void createFishingLine();
    void createFish();

    static constexpr float SCALE = 50.0f; // Scale Box2D meters to pixels
};

#endif // GAME_H
