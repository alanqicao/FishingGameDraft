#include "Game.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Game game;
    game.resize(800, 600);
    game.show();
    return a.exec();
}
