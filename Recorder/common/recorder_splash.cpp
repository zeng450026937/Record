#include "recorder_splash.h"
#include <QMouseEvent>
#include <QPainter>

RecorderSplash::RecorderSplash()
{
    start_movie(QString(":/resource/loading2.gif"));
}

void RecorderSplash::SetDiplayMovie(QString& gif)
{
    start_movie(gif);
}
void RecorderSplash::DiplayMessage(QString& message)
{
    this->showMessage(message, Qt::AlignHCenter|Qt::AlignBottom, QColor(128, 128, 128));
}

void RecorderSplash::drawContents(QPainter *painter)
{
    painter->save();
    painter->drawImage(0,0,_movie->currentImage());
    painter->restore();
    QSplashScreen::drawContents(painter);
}
void RecorderSplash::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
}

void RecorderSplash::start_movie(QString gif)
{
    _movie.reset(new QMovie(gif));
    this->connect(_movie.data(),SIGNAL(updated(QRect)),this,SLOT(on_frame_changed(QRect)));
    _movie->start();

    this->setPixmap(_movie->currentPixmap());
}
void RecorderSplash::on_frame_changed(QRect rect)
{
    Q_UNUSED(rect);
    if(!this->isVisible()){
        this->show();
    }
    this->update();
}
