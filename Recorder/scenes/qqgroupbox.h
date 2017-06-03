#ifndef QQGROUPBOX_H
#define QQGROUPBOX_H

#include <QGroupBox>
#include "qqslider.h"
#include <QPushButton>
#include <QList>

class QQSlider;

class QQGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit QQGroupBox(QWidget *parent = 0);
    ~QQGroupBox();

    void paintEvent(QPaintEvent *event);

    bool event(QEvent *e);

    void combine_slider(QQSlider* sliderCallback);

    QVariantMap add_mark(QString text,int time = 0);

    void add_mark(QVariantList mark_list);

    void clear_marks();

    QVariantList get_current_mark_list();


private:
    QMap<int, QStyleOptionButton*> _mark_map;
    QMap<int, QString>             _text_map;

    QVariantList   _current_mark_list;

    QQSlider*      _sliderCallback;

};


#endif // QQGROUPBOX_H
