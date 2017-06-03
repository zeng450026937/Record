#include "template_list_delegate_a.h"
#include "template_list_item_a.h"
#include <QListView>
#include <QEvent>
#include <QPainter>
#include <QDebug>

TemplateListDelegateA::TemplateListDelegateA(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

TemplateListDelegateA::~TemplateListDelegateA()
{

}

QWidget *TemplateListDelegateA::createEditor(QWidget *parent,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TemplateListDelegateA::receive_editButton(QVariantMap info)
{
    emit editTemplate(info);
}
void TemplateListDelegateA::receive_deleteButton(QString uuid)
{
    emit deleteTemplate(uuid);
}

void TemplateListDelegateA::paint(QPainter *painter,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
{
    QVariantMap info = index.data(Qt::UserRole).toMap();

    QListView *view = qobject_cast<QListView*>(option.styleObject);

    if(view){
        if(view->indexWidget(index) == Q_NULLPTR){

            TemplateListItemA* widget = new TemplateListItemA();
            view->setIndexWidget( index, widget );
        }
        else{
            TemplateListItemA *widget = qobject_cast<TemplateListItemA*>(view->indexWidget(index));
            if(widget)
                widget->update_display(index.data(Qt::UserRole).toMap());
        }
    }

    QRect rect = option.rect.adjusted(0,option.rect.height()-1,0,0);
    painter->fillRect(rect, QColor(200, 200, 200));

    return QStyledItemDelegate::paint(painter, option, index);
}

void TemplateListDelegateA::setEditorData(QWidget *editor,
                           const QModelIndex &index) const
{
    return QStyledItemDelegate::setEditorData(editor, index);
}

void TemplateListDelegateA::setModelData(QWidget *editor,
                          QAbstractItemModel *model,
                          const QModelIndex &index) const
{
    return QStyledItemDelegate::setModelData(editor, model, index);
}

QSize TemplateListDelegateA::sizeHint(const QStyleOptionViewItem &option,
                       const QModelIndex &index) const
{
    QListView *view = qobject_cast<QListView*>(option.styleObject);
    QSize hintSize = QStyledItemDelegate::sizeHint(option, index);

    if(view){
        if(view->indexWidget(index) != Q_NULLPTR){
            TemplateListItemA *widget = qobject_cast<TemplateListItemA*>(view->indexWidget(index));
            if(widget){
                hintSize.setHeight( widget->height() );
            }
        }
    }

    return hintSize;
}

void TemplateListDelegateA::updateEditorGeometry(QWidget *editor,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool TemplateListDelegateA::editorEvent(QEvent *event,
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
