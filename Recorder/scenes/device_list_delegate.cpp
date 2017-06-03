#include "device_list_delegate.h"
#include "device_list_item.h"
#include <QListView>
#include <QEvent>
#include <QPainter>
#include <QDebug>

DeviceListDelegate::DeviceListDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

DeviceListDelegate::~DeviceListDelegate()
{

}

QWidget *DeviceListDelegate::createEditor(QWidget *parent,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void DeviceListDelegate::receive_checkBox_stateChanged(bool checked, QVariantMap device)
{
    emit device_select_stateChanged(checked, device);
}

void DeviceListDelegate::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
    QVariantMap info = index.data(Qt::UserRole).toMap();

    QListView *view = qobject_cast<QListView*>(option.styleObject);

    if(view){
        if(view->indexWidget(index) == Q_NULLPTR){

            DeviceListItem* widget = new DeviceListItem();
            view->setIndexWidget( index, widget );
            QObject::connect(widget,SIGNAL(checkBox_stateChanged(bool,QVariantMap)),this,SLOT(receive_checkBox_stateChanged(bool,QVariantMap)));
        }
        else{
            DeviceListItem *widget = qobject_cast<DeviceListItem*>(view->indexWidget(index));
            if(widget)
                widget->update_display(index.data(Qt::UserRole).toMap());
        }
    }

    QRect rect = option.rect.adjusted(0,option.rect.height()-1,0,0);
    painter->fillRect(rect, QColor(200, 200, 200));

    return QStyledItemDelegate::paint(painter, option, index);
}

void DeviceListDelegate::setEditorData(QWidget *editor,
                           const QModelIndex &index) const
{
    return QStyledItemDelegate::setEditorData(editor, index);
}

void DeviceListDelegate::setModelData(QWidget *editor,
                          QAbstractItemModel *model,
                          const QModelIndex &index) const
{
    return QStyledItemDelegate::setModelData(editor, model, index);
}

QSize DeviceListDelegate::sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QListView *view = qobject_cast<QListView*>(option.styleObject);
    QSize hintSize = QStyledItemDelegate::sizeHint(option, index);

    if(view){
        if(view->indexWidget(index) != Q_NULLPTR){
            DeviceListItem *widget = qobject_cast<DeviceListItem*>(view->indexWidget(index));
            if(widget){
                hintSize.setHeight( widget->height() );
            }
        }
    }

    return hintSize;
}

void DeviceListDelegate::updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool DeviceListDelegate::editorEvent(QEvent *event,
                         QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index)
{
    Q_UNUSED(event);
    Q_UNUSED(model);
    Q_UNUSED(option);
    Q_UNUSED(index);
    return false;
}
