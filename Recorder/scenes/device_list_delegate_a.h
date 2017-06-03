#ifndef DEVICELISTDELEGATEA_H
#define DEVICELISTDELEGATEA_H

#include <QStyledItemDelegate>

class DeviceListDelegateA : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DeviceListDelegateA(QObject *parent = 0);
    ~DeviceListDelegateA();

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

#endif // DEVICELISTDELEGATEA_H
