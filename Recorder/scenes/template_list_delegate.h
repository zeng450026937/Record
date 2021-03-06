#ifndef TEMPLATELISTDELEGATE_H
#define TEMPLATELISTDELEGATE_H

#include <QStyledItemDelegate>

class TemplateListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TemplateListDelegate(QObject *parent = 0);
    ~TemplateListDelegate();

signals:
    void editTemplate(QVariantMap info);
    void deleteTemplate(QString uuid);

private slots:
    void receive_editButton(QVariantMap info);
    void receive_deleteButton(QString uuid);

protected:
    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;

    virtual void paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

    virtual void setEditorData(QWidget *editor,
                               const QModelIndex &index) const;

    virtual void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    virtual void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;

    virtual bool editorEvent(QEvent *event,
                             QAbstractItemModel *model,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index);

};

#endif // TEMPLATELISTDELEGATE_H
