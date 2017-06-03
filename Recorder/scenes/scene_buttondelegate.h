#ifndef BUTTONDELEGATE_H
#define BUTTONDELEGATE_H

#include <QStyledItemDelegate>
#include <QItemDelegate>
#include <QtGui>
#include <QDialog>
#include <QApplication>
#include <QDesktopWidget>

class DLButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DLButtonDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void dl_file();
public slots:

private:
    QMap<QModelIndex, QStyleOptionButton*> _button_map;
    QString _button_text;
};

class BatteryIconDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BatteryIconDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:

private:

};

class ClipButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ClipButtonDelegate(QObject *parent = 0);
    explicit ClipButtonDelegate(QString text, QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void clip_file();
public slots:

private:
    QMap<QModelIndex, QStyleOptionButton*> _button_map;
    QString _button_text;
};

class EditButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit EditButtonDelegate(QObject *parent = 0);
    explicit EditButtonDelegate(QString text, QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void conference_edit(QAbstractItemModel *model,QModelIndex index);

public slots:

private:
    QMap<QModelIndex, QStyleOptionButton*> _button_map;
    QString _button_text;
};


class DeleteButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit DeleteButtonDelegate(QObject *parent = 0);
    explicit DeleteButtonDelegate(QString text, QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
    void conference_delete(QAbstractItemModel *model,QModelIndex index);

public slots:

private:
    QMap<QModelIndex, QStyleOptionButton*> _button_map;
    QString _button_text;
};


#endif // BUTTONDELEGATE_H
