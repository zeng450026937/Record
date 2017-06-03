#include "qqslider.h"
#include <QStyleOptionComplex>
#include <QPainter>
#include <QApplication>
#include <QToolTip>

QQSlider::QQSlider(QWidget *parent):
    QSlider(parent),
    _sub_handle(new QStyleOptionButton),
    _add_handle(new QStyleOptionButton),
    _is_show_select_handle(false),
    _is_show_subvalue(true),
    _sub_handle_moveable(false),
    _add_handle_moveable(false),
    _click_offset(0),
    _left_value(0),
    _right_value(0),
    _subGroove_value(0)
{
    this->setOrientation(Qt::Horizontal);

    this->setRange(0,100);
    this->setValue(50);

    this->setStyleSheet("QSlider::groove:horizontal {                                "
                        "     border: 1px solid #999999;                             "
                        "     height: 12px;                                           "
                        "     position: absolute;                                     "
                        "     margin: 0px 0;                                         "
                        "     left: 12px; right: 12px;                                 "
                        "     background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #B1B1B1, stop:1 #c4c4c4);"
                        "}                                                           "
                        "QSlider::handle:horizontal {                                "
                       // "     border: 1px solid #5c5c5c;                             "
                        "     width: 12px;                                           "
                        "     height: 12px;                                           "
                        "     margin: 0 -7px;                                          "
                        " }                                                          "
                        "QSlider::sub-page:horizontal{                               "
                        " background: gray;                             "
                        "}"
                        );
}

QQSlider::~QQSlider()
{
    if(_sub_handle){
        delete _sub_handle;
    }
    if(_add_handle){
        delete _add_handle;
    }
}

int QQSlider::get_select_value()
{
    return (this->get_end_value() - this->get_start_value());
}

int QQSlider::get_start_value()
{
    int offset = ( _sub_handle->rect.left() - _left_value_pos ) * (this->maximum() - this->minimum()) / _groove.width();

    int start = _left_value + offset;
    if(start < this->minimum()){
        start = this->minimum();
    }
    else if(start > this->maximum()){
        start = this->maximum();
    }

    return start;
}

int QQSlider::get_end_value()
{
    int offset = ( _add_handle->rect.left() - _right_value_pos ) * (this->maximum() - this->minimum()) / _groove.width() ;

    int end = _right_value + offset;
    if(end < this->minimum()){
        end = this->minimum();
    }
    else if(end > this->maximum()){
        end = this->maximum();
    }

    return end;
}

int QQSlider::get_handle_width()
{
    return _handle.width();
}

int QQSlider::get_groove_left()
{
    return _groove.left();
}

void QQSlider::set_handle_value(int left_value, int right_value)
{
    qDebug()<<left_value<<right_value;
    if(left_value >= right_value){
        return;
    }
    if(left_value < this->minimum()){
        left_value = this->minimum();
    }
    if(right_value > this->maximum()){
        right_value = this->maximum();
    }
    if( left_value >= right_value)
        left_value = right_value - 1;

    this->showSelectValue(true);

    _left_value = left_value;
    _right_value = right_value;

    qDebug()<<_left_value<<_right_value;

    _sub_handle->rect = _handle.adjusted(0,-4,0,4);
    _sub_handle->rect.moveLeft(RangeValueToPixelPos(left_value > this->minimum() ? left_value:this->minimum()) + _handle.width()/2);
    _sub_handle->state |= QStyle::State_Enabled;

    _add_handle->rect = _handle.adjusted(0,-4,0,4);
    _add_handle->rect.moveLeft(RangeValueToPixelPos(right_value < this->maximum() ? right_value:this->maximum()) + _handle.width()/2);
    _add_handle->state |= QStyle::State_Enabled;

    _selected_groove = QRect(_sub_handle->rect.right(),
                             _groove.top(),
                             _add_handle->rect.left()-_sub_handle->rect.left(),
                             _groove.height());

    _left_value_pos = _sub_handle->rect.left();
    _right_value_pos = _add_handle->rect.left();

    this->update();

    emit set_value_changed(this->get_select_value());
}

int QQSlider::RangeValueToPixelPos(int value)
{
    if(value > this->maximum() || value < this->minimum()){
        return -1;
    }

    QStyleOptionSlider option;
    this->initStyleOption(&option);

    _groove = this->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderGroove, this);
    _handle = this->style()->subControlRect(QStyle::CC_Slider, &option, QStyle::SC_SliderHandle, this);

    int sliderMin, sliderMax, sliderLength;

    if (this->orientation() == Qt::Horizontal) {
            sliderLength = _handle.width();
            sliderMin = _groove.x();
            sliderMax = _groove.right() + 1;
    } else {
        sliderLength = _handle.height();
        sliderMin = _groove.y();
        sliderMax = _groove.bottom() + 1;
    }

    return this->style()->sliderPositionFromValue(this->minimum(),this->maximum(),value,
                                                  sliderMax-sliderMin,option.upsideDown);
}


void QQSlider::setSubValue(int value)
{

    if(value > this->value()){
        _subGroove_value = this->value();
    }else{
       _subGroove_value = value;
    }
    this->update();
}

void QQSlider::showSubValue(bool display)
{
    _is_show_subvalue = display;
    this->update();
}

void QQSlider::showSelectValue(bool display)
{
    _is_show_select_handle = display;
    this->update();
}

void  QQSlider::mouseMoveEvent(QMouseEvent *ev)
{
    if ( (!_sub_handle_moveable) && (!_add_handle_moveable) ){
        ev->ignore();
        return;
    }

    ev->accept();

    if(_sub_handle_moveable){

        int destPos = this->pick(ev->pos()) - _click_offset;
        if(destPos > _add_handle->rect.left() - _sub_handle->rect.width() ){
            destPos = _add_handle->rect.left() - _sub_handle->rect.width() - 1 ;
        }
        if(destPos < _groove.left() - _sub_handle->rect.width()/2){
            destPos = _groove.left() - _sub_handle->rect.width()/2 - 1;
        }
        _sub_handle->rect.moveLeft(destPos);

        QTime time;
        int seconds = this->get_start_value();
        time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);
        QToolTip::showText(this->mapToGlobal(_sub_handle->rect.center()),time.toString("hh:mm:ss"));
    }

    if(_add_handle_moveable){

        int destPos = this->pick(ev->pos()) - _click_offset;
        if(destPos < _sub_handle->rect.right() ){
            destPos = _sub_handle->rect.right() - _sub_handle->rect.width() + 1;
        }
        if(destPos > _groove.right() - _add_handle->rect.width()/2){
            destPos = _groove.right() - _add_handle->rect.width()/2 + 2;
        }
        _add_handle->rect.moveLeft(destPos);

        QTime time;
        int seconds = this->get_end_value();
        time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);
        QToolTip::showText(this->mapToGlobal(_add_handle->rect.center()),time.toString("hh:mm:ss"));
    }

    _selected_groove = QRect(_sub_handle->rect.right(),
                             _groove.top(),
                             _add_handle->rect.left()-_sub_handle->rect.left(),
                             _groove.height());

    this->update();

    emit select_value_changed(this->get_select_value());

}
void  QQSlider::mousePressEvent(QMouseEvent *ev)
{
    if (this->maximum() == this->minimum() || _is_show_select_handle == false) {
        ev->ignore();
        return;
    }

    if(_sub_handle->rect.contains(ev->pos())){
        _sub_handle_moveable = true;
        _click_offset = this->pick(ev->pos() - _sub_handle->rect.topLeft());

        QTime time;
        int seconds = this->get_start_value();
        time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);
        QToolTip::showText(this->mapToGlobal(_sub_handle->rect.center()),time.toString("hh:mm:ss"));

        return;

    }else{
        _sub_handle_moveable = false;
    }

    if(_add_handle->rect.contains(ev->pos())){
        _add_handle_moveable = true;
        _click_offset = this->pick(ev->pos() - _add_handle->rect.topLeft());

        QTime time;
        int seconds = this->get_end_value();
        time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);
        QToolTip::showText(this->mapToGlobal(_add_handle->rect.center()),time.toString("hh:mm:ss"));

        return;

    }else{
        _add_handle_moveable = false;
    }

}

void QQSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    _sub_handle_moveable = false;
    _add_handle_moveable = false;
}


void QQSlider::paintEvent(QPaintEvent *event)
{
    QSlider::paintEvent(event);

    QPainter painter(this);

    int offset = RangeValueToPixelPos(_subGroove_value);

    if(_is_show_subvalue && offset){

        _subGroove = _groove;
        _subGroove.setWidth(offset);

        painter.save();
        painter.fillRect(_subGroove,QColor(85, 170, 200));

        painter.restore();
    }

    if(_is_show_select_handle){

        painter.save();
        painter.fillRect(_selected_groove,QColor(255, 85, 85, 150));

        painter.restore();

        QApplication::style()->drawControl(QStyle::CE_PushButton, _sub_handle, &painter,this);
        QApplication::style()->drawControl(QStyle::CE_PushButton, _add_handle, &painter,this);
    }
}

bool QQSlider::event(QEvent *event){

    switch(event->type()) {
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
        if (const QHoverEvent *he = static_cast<const QHoverEvent *>(event)){
            if(_sub_handle->rect.contains(he->pos())){
                _sub_handle->state |= QStyle::State_MouseOver;
            }else{
                _sub_handle->state &= (~QStyle::State_MouseOver);
            }

            if(_add_handle->rect.contains(he->pos())){
                _add_handle->state |= QStyle::State_MouseOver;
            }else{
                _add_handle->state &= (~QStyle::State_MouseOver);
            }

            this->update();

        }
        break;

    default:
        break;
    }
    return QSlider::event(event);
}
