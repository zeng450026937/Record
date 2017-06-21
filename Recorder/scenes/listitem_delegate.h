#ifndef LISTITEMDELEGATE_H
#define LISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class ListForm;

class ListItemDelegate : public QStyledItemDelegate {
  Q_OBJECT
 public:
  explicit ListItemDelegate(QObject *parent = 0);
  ~ListItemDelegate();
 signals:
  void itemClicked(const QString &uuid);

 protected:
  virtual QWidget *createEditor(QWidget *parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const;

  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;

  virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

  virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const;

  virtual QSize sizeHint(const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;

  virtual void updateEditorGeometry(QWidget *editor,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const;

  virtual bool editorEvent(QEvent *event, QAbstractItemModel *model,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index);
};

#endif  // LISTITEMDELEGATE_H
