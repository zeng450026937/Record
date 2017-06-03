#include "device_list_delegate_a.h"
#include "device_list_item_a.h"
#include <QListView>
#include <QEvent>
#include <QPainter>
#include <QDebug>

DeviceListDelegateA::DeviceListDelegateA(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

DeviceListDelegateA::~DeviceListDelegateA()
{

}

QWidget *DeviceListDelegateA::createEditor(QWidget *parent,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void DeviceListDelegateA::receive_checkBox_stateChanged(bool checked, QVariantMap device)
{
    emit device_select_stateChanged(checked, device);
}

void DeviceListDelegateA::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
    QVariantMap info = index.data(Qt::UserRole).toMap();

    QListView *view = qobject_cast<QListView*>(option.styleObject);

    if(view){
        if(view->indexWidget(index) == Q_NULLPTR){

            DeviceListItemA* widget = new DeviceListItemA();
            view->setIndexWidget( index, widget );
            QObject::connect(widget,SIGNAL(checkBox_stateChanged(bool,QVariantMap)),this,SLOT(receive_checkBox_stateChanged(bool,QVariantMap)));
        }
        else{
            DeviceListItemA *widget = qobject_cast<DeviceListItemA*>(view->indexWidget(index));
            if(widget)
                widget->update_display(index.data(Qt::UserRole).toMap());
        }
    }

    QRect rect = option.rect.adjusted(0,option.rect.height()-1,0,0);
    painter->fillRect(rect, QColor(200, 200, 200));

    return QStyledItemDelegate::paint(painter, option, index);
}

void DeviceListDelegateA::setEditorData(QWidget *editor,
                           const QModelIndex &index) const
{
    return QStyledItemDelegate::setEditorData(editor, index);
}

void DeviceListDelegateA::setModelData(QWidget *editor,
                          QAbstractItemModel *model,
                          const QModelIndex &index) const
{
    return QStyledItemDelegate::setModelData(editor, model, index);
}

QSize DeviceListDelegateA::sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QListView *view = qobject_cast<QListView*>(option.styleObject);
    QSize hintSize = QStyledItemDelegate::sizeHint(option, index);

    if(view){
        if(view->indexWidget(index) != Q_NULLPTR){
            DeviceListItemA *widget = qobject_cast<DeviceListItemA*>(view->indexWidget(index));
            if(widget){
                hintSize.setHeight( widget->height() );
            }
        }
    }

    return hintSize;
}

void DeviceListDelegateA::updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool DeviceListDelegateA::editorEvent(QEvent *event,
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

