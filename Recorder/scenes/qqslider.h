#ifndef QQSLIDER_H
#define QQSLIDER_H


#include <QtGui>
#include <QStyle>
#include <QStyleOption>
#include <QSlider>

class QQSlider : public QSlider
{
    Q_OBJECT
public:
    explicit QQSlider(QWidget *parent = 0);
    ~QQSlider();

    void paintEvent(QPaintEvent *event);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

    bool event(QEvent *event);

    int get_select_value();
    int get_start_value();
    int get_end_value();
    int get_handle_width();
    int get_groove_left();
    void set_handle_value(int left_value, int right_value);
    int RangeValueToPixelPos(int value);

signals:
    void select_value_changed(int value);//for mousemove action
    void set_value_changed(int value);//for manual set

public slots:
    void setSubValue(int value);
    void showSubValue(bool display = true);
    void showSelectValue(bool display = true);

private:
    int pick(const QPoint &pt)
    {
        return this->orientation() == Qt::Horizontal ? pt.x() : pt.y();
    }

private:
    int     _left_value;
    int     _left_value_pos;
    int     _right_value;
    int     _right_value_pos;
    int     _subGroove_value;
    QRect   _subGroove;
    QRect   _groove;
    QRect   _handle;

    QStyleOptionButton *_sub_handle;
    QStyleOptionButton *_add_handle;
    bool    _sub_handle_moveable;
    bool    _add_handle_moveable;
    int     _click_offset;
    QRect   _selected_groove;
    bool    _is_show_select_handle;
    bool    _is_show_subvalue;

};

#endif // QQSLIDER_H
