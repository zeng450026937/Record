#ifndef DEVICELISTDELEGATE_H
#define DEVICELISTDELEGATE_H

#include <QStyledItemDelegate>

class DeviceListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DeviceListDelegate(QObject *parent = 0);
    ~DeviceListDelegate();

signals:
    void device_select_stateChanged(bool checked, QVariantMap device);

private slots:
    void receive_checkBox_stateChanged(bool checked, QVariantMap device);

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

#endif // DEVICELISTDELEGATE_H
