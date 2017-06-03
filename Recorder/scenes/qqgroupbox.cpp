#include "qqgroupbox.h"
#include <QApplication>
#include <QFontMetrics>
#include <QToolTip>

QQGroupBox::QQGroupBox(QWidget *parent) :
    QGroupBox(parent),
    _sliderCallback(nullptr)
{
}

QQGroupBox::~QQGroupBox()
{
    _current_mark_list.clear();
    qDeleteAll(_mark_map);
    _mark_map.clear();
    _sliderCallback = nullptr;
}

void QQGroupBox::paintEvent(QPaintEvent *event)
{
    QGroupBox::paintEvent(event);
    QPainter painter(this);

    painter.save();

    if(_mark_map.count() > 0){
        QMap<int, QStyleOptionButton*>::Iterator iter = _mark_map.begin();
        while(iter != _mark_map.end()){
            QStyleOptionButton *button = const_cast<QStyleOptionButton *>(iter.value());
            //QApplication::style()->drawControl(QStyle::CE_PushButton, button, &painter,this);
            style()->drawControl(QStyle::CE_PushButton, button, &painter,this);
            QPixmap pixmap = QPixmap(":/resource/icon.png");
            painter.drawPixmap(button->rect,pixmap);
            if (button->state & QStyle::State_MouseOver) {
                QString text = _text_map.value(iter.key());
                QRect textRect = button->rect.adjusted(0,-20,0,-20);
                QFontMetrics fm(this->font());
                textRect.setWidth(fm.width(text) + 5);
                textRect.setLeft(textRect.left() - fm.width(text) / 2 + button->rect.width() / 2 - 1);
                textRect.setHeight(fm.height() + 5);

                painter.drawText(textRect, Qt::AlignCenter, QString("%1").arg( text ));

                //QToolTip::showText(this->mapToGlobal(button->rect.center()),QString("%1").arg(iter.key()));
            }
            iter++;
        }
    }

    painter.restore();
}

bool QQGroupBox::event(QEvent *e)
{
    switch(e->type()) {
    case QEvent::HoverMove:
        if (const QHoverEvent *he = static_cast<const QHoverEvent *>(e)){

            if(_mark_map.count() > 0){
                QMap<int, QStyleOptionButton*>::Iterator iter = _mark_map.begin();
                while(iter != _mark_map.end()){
                    QStyleOptionButton *button = const_cast<QStyleOptionButton *>(iter.value());
                    if(button->rect.contains(he->pos())){
                        button->state |= QStyle::State_MouseOver;
                    }else{
                        button->state &= (~QStyle::State_MouseOver);
                    }
                    iter++;
                }
            }
        }
        this->update();

        break;

    default:

        break;

    }
    return QGroupBox::event(e);
}


void QQGroupBox::combine_slider(QQSlider* sliderCallback)
{
    _sliderCallback = sliderCallback;
}

QVariantMap QQGroupBox::add_mark(QString text,int time)
{
    QVariantMap _mark_info;

    QRect rect = {0,0,0,0};

    int _time = time;

    if(_sliderCallback->RangeValueToPixelPos(_time) == -1){
        return _mark_info;
    }

    if(_time == 0){
        _time = _sliderCallback->value();
    }

    if(_sliderCallback){
        rect.setLeft(_sliderCallback->RangeValueToPixelPos(_time) + _sliderCallback->get_groove_left() - 10);
        rect.setTop(40);
        rect.setWidth(20);
        rect.setHeight(20);
    }

    QStyleOptionButton* button = _mark_map.value(_time);

    if (!button) {
        button = new QStyleOptionButton();
        button->rect = rect;
        button->state |= QStyle::State_Enabled;

        (const_cast<QQGroupBox *>(this))->_mark_map.insert(_time, button);
    }

    _text_map.insert(_time, text);

    this->update();

    _mark_info.insert("index",(int)(_current_mark_list.count()+1));
    _mark_info.insert("mark",text);
    _mark_info.insert("time",(int)(_time));

    _current_mark_list.append(_mark_info);

    return _mark_info;

}

void QQGroupBox::add_mark(QVariantList mark_list)
{
    _current_mark_list.clear();
    qDeleteAll(_mark_map);
    _mark_map.clear();

    _current_mark_list = mark_list;

    if(_current_mark_list.count() > 0){

        foreach(QVariant mark_info,_current_mark_list){

            QRect rect = {0,0,0,0};

            int _time = (mark_info.toMap())["time"].toInt();
            QString text = (mark_info.toMap())["mark"].toString();
            qDebug()<<_time<<text;

            if(_sliderCallback->RangeValueToPixelPos(_time) == -1){
                continue;
            }

            QStyleOptionButton* button = _mark_map.value(_time);

            if(_time == 0){
                //_time = _sliderCallback->value();
                //continue;
            }

            if(_sliderCallback){
                rect.setLeft(_sliderCallback->RangeValueToPixelPos(_time) + _sliderCallback->get_groove_left() - 10);
                rect.setTop(40);
                rect.setWidth(20);
                rect.setHeight(20);
            }

            //qDebug()<<"position = "<<rect.left()<<_sliderCallback->RangeValueToPixelPos(_time);

            if (!button) {
                button = new QStyleOptionButton();
                button->rect = rect;
                button->state |= QStyle::State_Enabled;

                (const_cast<QQGroupBox *>(this))->_mark_map.insert(_time, button);
            }

            _text_map.insert(_time, text);
        }

    }

    this->update();

}

void QQGroupBox::clear_marks()
{
    _current_mark_list.clear();
    qDeleteAll(_mark_map);
    _mark_map.clear();

}

QVariantList QQGroupBox::get_current_mark_list()
{
    return _current_mark_list;
}


