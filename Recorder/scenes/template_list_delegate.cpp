#include "template_list_delegate.h"
#include "template_list_item.h"
#include <QListView>
#include <QEvent>
#include <QPainter>
#include <QDebug>

TemplateListDelegate::TemplateListDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

TemplateListDelegate::~TemplateListDelegate()
{

}

QWidget *TemplateListDelegate::createEditor(QWidget *parent,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TemplateListDelegate::receive_editButton(QVariantMap info)
{
    emit editTemplate(info);
}
void TemplateListDelegate::receive_deleteButton(QString uuid)
{
    emit deleteTemplate(uuid);
}

void TemplateListDelegate::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
    QVariantMap info = index.data(Qt::UserRole).toMap();

    QListView *view = qobject_cast<QListView*>(option.styleObject);

    if(view){
        if(view->indexWidget(index) == Q_NULLPTR){

            TemplateListItem* widget = new TemplateListItem();
            view->setIndexWidget( index, widget );
            QObject::connect(widget,SIGNAL(editButton(QVariantMap)),this,SLOT(receive_editButton(QVariantMap)));
            QObject::connect(widget,SIGNAL(deleteButton(QString)),this,SLOT(receive_deleteButton(QString)));
        }
        else{
            TemplateListItem *widget = qobject_cast<TemplateListItem*>(view->indexWidget(index));
            if(widget)
                widget->update_display(index.data(Qt::UserRole).toMap());
        }
    }

    QRect rect = option.rect.adjusted(0,option.rect.height()-1,0,0);
    painter->fillRect(rect, QColor(200, 200, 200));

    return QStyledItemDelegate::paint(painter, option, index);
}

void TemplateListDelegate::setEditorData(QWidget *editor,
                           const QModelIndex &index) const
{
    return QStyledItemDelegate::setEditorData(editor, index);
}

void TemplateListDelegate::setModelData(QWidget *editor,
                          QAbstractItemModel *model,
                          const QModelIndex &index) const
{
    return QStyledItemDelegate::setModelData(editor, model, index);
}

QSize TemplateListDelegate::sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QListView *view = qobject_cast<QListView*>(option.styleObject);
    QSize hintSize = QStyledItemDelegate::sizeHint(option, index);

    if(view){
        if(view->indexWidget(index) != Q_NULLPTR){
            TemplateListItem *widget = qobject_cast<TemplateListItem*>(view->indexWidget(index));
            if(widget){
                hintSize.setHeight( widget->height() );
            }
        }
    }

    return hintSize;
}

void TemplateListDelegate::updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool TemplateListDelegate::editorEvent(QEvent *event,
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
