#include "scene_buttondelegate.h"

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>

DLButtonDelegate::DLButtonDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void DLButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);

    QStyleOptionButton* button = _button_map.value(index);

    if (!button) {
        button = new QStyleOptionButton();
        button->rect = option.rect.adjusted(4, 4, -4, -4);
        button->text = _button_text;
        button->state |= QStyle::State_Enabled;

        (const_cast<DLButtonDelegate *>(this))->_button_map.insert(index, button);
    }

    button->rect = option.rect.adjusted(4, 4, -4, -4);

    painter->save();

    painter->fillRect(option.rect, QBrush(QColor(255,255,255)));

    if(option.state & QStyle::State_Selected){
         painter->fillRect(option.rect, option.palette.highlight());

         if (option.state & QStyle::State_Enabled){
             painter->fillRect(option.rect, option.palette.background());

             if (option.state & QStyle::State_Active){
                 painter->fillRect(option.rect, option.palette.highlight());
             }
         }
    }
    int status = index.data().toInt();

    QPixmap pixmap;

    switch(status){
    case 0:
        pixmap = QPixmap(":/resource/u620.png");
        break;
    case 1:
        pixmap = QPixmap(":/resource/u1610.png");
        break;
    case 2:
        pixmap = QPixmap(":/resource/u1610.png");
        break;
    case 3:
        pixmap = QPixmap(":/resource/u579.png");
        break;
    }

    QRect prect = option.rect;
    prect.setWidth(option.rect.height());
    prect.moveLeft(option.rect.center().rx() - prect.width() / 2);

    painter->drawPixmap(prect.adjusted(3,3,-3,-3),pixmap);

    painter->restore();
    //QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
}

bool DLButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state |= QStyle::State_Sunken;
        }

        QTableView *b = qobject_cast<QTableView*>(option.styleObject);

        if(b){
            if(b->currentIndex().row() !=index.row() )
                b->selectRow(index.row());
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

            int status = index.data().toInt();

            if( status == 3)
                emit dl_file();
        }
    }

    if (event->type() == QEvent::MouseMove) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.contains(e->x(), e->y()) ) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

        }
    }

    return true;
}

BatteryIconDelegate::BatteryIconDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void BatteryIconDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);

    painter->save();

    QString per = index.data().toString();
    per = per.left(per.length()-1);
    int iper = per.toInt();

    QPixmap pixmap;
    if(iper > 60){
        pixmap = QPixmap(":/resource/u1405.png");
    }else if(iper > 20){
        pixmap = QPixmap(":/resource/u1412.png");
    }else{
        pixmap = QPixmap(":/resource/u1418.png");
    }
    painter->drawPixmap(option.rect.adjusted(4, 4, -4, -4),pixmap);

    painter->restore();

}


ClipButtonDelegate::ClipButtonDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

ClipButtonDelegate::ClipButtonDelegate(QString text, QObject *parent):
    _button_text(text),
    QStyledItemDelegate(parent)
{
}

void ClipButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton* button = _button_map.value(index);

    if (!button) {
        button = new QStyleOptionButton();
        button->rect = option.rect.adjusted(4, 4, -4, -4);
        button->text = _button_text;
        button->state |= QStyle::State_Enabled;

        (const_cast<ClipButtonDelegate *>(this))->_button_map.insert(index, button);
    }

    button->rect = option.rect.adjusted(4, 4, -4, -4);

    painter->save();

    if(option.state & QStyle::State_Selected){
         painter->fillRect(option.rect, option.palette.highlight());

         if (option.state & QStyle::State_Enabled){
             painter->fillRect(option.rect, option.palette.background());

             if (option.state & QStyle::State_Active){
                 painter->fillRect(option.rect, option.palette.highlight());
             }
         }
    }


    //QPixmap pixmap = QPixmap(":/resource/edit.png");
    //painter->drawPixmap(button->rect,pixmap);

    painter->restore();
    QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
}

bool ClipButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state |= QStyle::State_Sunken;
        }

        QTableView *b = qobject_cast<QTableView*>(option.styleObject);

        if(b){
            b->selectRow(index.row());
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

            emit clip_file();
        }
    }

    if (event->type() == QEvent::MouseMove) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.contains(e->x(), e->y()) ) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

        }
    }

    return true;
}



EditButtonDelegate::EditButtonDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

EditButtonDelegate::EditButtonDelegate(QString text, QObject *parent):
    _button_text(text),
    QStyledItemDelegate(parent)
{
}

void EditButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);

    QStyleOptionButton* button = _button_map.value(index);

    if (!button) {
        button = new QStyleOptionButton();
        button->rect = option.rect.adjusted(4, 4, -4, -4);
        button->text = _button_text;
        button->state |= QStyle::State_Enabled;

        (const_cast<EditButtonDelegate *>(this))->_button_map.insert(index, button);
    }

    button->rect = option.rect.adjusted(4, 4, -4, -4);

    painter->save();

    /*
    if(option.state & QStyle::State_Selected){
         painter->fillRect(option.rect, option.palette.highlight());

         if (option.state & QStyle::State_Enabled){
             painter->fillRect(option.rect, option.palette.background());

             if (option.state & QStyle::State_Active){
                 painter->fillRect(option.rect, option.palette.highlight());
             }
         }
    }
    */

    QPixmap pixmap = QPixmap(":/resource/edit.png");
    QRect prect = option.rect;
    prect.setWidth(option.rect.height());
    prect.moveLeft(option.rect.center().rx() - prect.width() / 2);
    painter->drawPixmap(prect,pixmap);

    painter->restore();
    //QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
}

bool EditButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state |= QStyle::State_Sunken;
        }

       QTableView *b = qobject_cast<QTableView*>(option.styleObject);

        if(b){
            b->selectRow(index.row());
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

            emit conference_edit(model, index);
        }
    }

    if (event->type() == QEvent::MouseMove) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.contains(e->x(), e->y()) ) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

        }
    }

    return true;
}



DeleteButtonDelegate::DeleteButtonDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

DeleteButtonDelegate::DeleteButtonDelegate(QString text, QObject *parent):
    _button_text(text),
    QStyledItemDelegate(parent)
{
}

void DeleteButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter,option,index);

    QStyleOptionButton* button = _button_map.value(index);

    if (!button) {
        button = new QStyleOptionButton();
        button->rect = option.rect.adjusted(4, 4, -4, -4);
        button->text = _button_text;
        button->state |= QStyle::State_Enabled;

        (const_cast<DeleteButtonDelegate *>(this))->_button_map.insert(index, button);
    }

    button->rect = option.rect.adjusted(4, 4, -4, -4);

    painter->save();

    /*
    if(option.state & QStyle::State_Selected){
         painter->fillRect(option.rect, option.palette.highlight());

         if (option.state & QStyle::State_Enabled){
             painter->fillRect(option.rect, option.palette.background());

             if (option.state & QStyle::State_Active){
                 painter->fillRect(option.rect, option.palette.highlight());
             }
         }
    }
    */

    if(option.state & QStyle::State_None){
        painter->fillRect(option.rect, option.palette.background());
    }

    QPixmap pixmap = QPixmap(":/resource/delete.png");
    QRect prect = option.rect;
    prect.setWidth(option.rect.height());
    prect.moveLeft(option.rect.center().rx() - prect.width() / 2);
    painter->drawPixmap(prect,pixmap);

    painter->restore();
    //QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
}

bool DeleteButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state |= QStyle::State_Sunken;
        }

       QTableView *b = qobject_cast<QTableView*>(option.styleObject);

        if(b){
            b->selectRow(index.row());
        }
    }

    if (event->type() == QEvent::MouseButtonRelease) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(4, 4, -4, -4).contains(e->x(), e->y()) && _button_map.contains(index)) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

            emit conference_delete(model, index);

            /*
            QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model);
            if(m){
                m->removeRow(index.row());
            }
            */
        }
    }

    if (event->type() == QEvent::MouseMove) {

        QMouseEvent* e =(QMouseEvent*)event;

        if (option.rect.adjusted(-1,-1,-1,-1).contains(e->x(), e->y()) ) {
            _button_map.value(index)->state &= (~QStyle::State_Sunken);

        }
    }

    return true;
}

