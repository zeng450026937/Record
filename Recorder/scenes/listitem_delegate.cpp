#include "listitem_delegate.h"
#include <QDebug>
#include <QEvent>
#include <QListView>
#include <QPainter>
#include <QVector>
#include "list_form.h"

ListItemDelegate::ListItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

ListItemDelegate::~ListItemDelegate() {}

QWidget *ListItemDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const {
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void ListItemDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
  QListView *view = qobject_cast<QListView *>(option.styleObject);

  if (view) {
    if (view->indexWidget(index) == Q_NULLPTR) {
      ListForm *widget = new ListForm();
      view->setIndexWidget(index, widget);
      QObject::connect(widget, SIGNAL(itemClicked(QString)), this,
                       SIGNAL(itemClicked(QString)));
    } else {
      ListForm *widget = qobject_cast<ListForm *>(view->indexWidget(index));
      if (widget) widget->update_display(index.data(Qt::UserRole).toMap());
    }
  }

  QRect rect = option.rect.adjusted(0, option.rect.height() - 1, 0, 0);
  painter->fillRect(rect, QColor(200, 200, 200));

  return QStyledItemDelegate::paint(painter, option, index);
}

void ListItemDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
  return QStyledItemDelegate::setEditorData(editor, index);
}

void ListItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const {
  return QStyledItemDelegate::setModelData(editor, model, index);
}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
  QListView *view = qobject_cast<QListView *>(option.styleObject);
  QSize hintSize = QStyledItemDelegate::sizeHint(option, index);

  if (view) {
    if (view->indexWidget(index) != Q_NULLPTR) {
      ListForm *widget = qobject_cast<ListForm *>(view->indexWidget(index));
      if (widget) {
        hintSize.setHeight(widget->height());
      }
    }
  }

  return hintSize;
}

void ListItemDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const {
  return QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

bool ListItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {
  Q_UNUSED(event);
  Q_UNUSED(model);
  Q_UNUSED(option);
  Q_UNUSED(index);
  return false;
}
