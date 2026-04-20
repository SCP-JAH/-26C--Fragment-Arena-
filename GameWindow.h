#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QMouseEvent>
#include "Bullet.h"

#include "Enemy.h"

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void gameLoop();

private:
    void updateGame(double dt);
    void resetGame();

private:
    QTimer* m_timer;
    QElapsedTimer m_elapsedTimer;
    qint64 m_lastTime;

    double m_playerX;
    double m_playerY;
    double m_playerSize;
    double m_playerSpeed;

    bool m_moveUp;
    bool m_moveDown;
    bool m_moveLeft;
    bool m_moveRight;

    QVector<Bullet> m_bullets;
    QPointF m_mousePos;
    QVector<Enemy> m_enemies;
    int m_playerHp;
    int m_playerMaxHp;
    double m_playerDamageTimer;
    bool m_gameOver;
    int m_killCount;
};

#endif // GAMEWINDOW_H