#ifndef RECORDERSPLASH_H
#define RECORDERSPLASH_H

#include <QSplashScreen>
#include <QMovie>

class RecorderSplash : public QSplashScreen
{
    Q_OBJECT
public:
    RecorderSplash();
    ~RecorderSplash() {}

    void SetDiplayMovie(QString& gif);

    void DiplayMessage(QString& message);


    void drawContents(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *e) override;

private slots:
    void start_movie(QString gif);
    void on_frame_changed(QRect rect);

private:
    QScopedPointer<QMovie> _movie;
};

#endif // RECORDERSPLASH_H
