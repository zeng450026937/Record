#ifndef TEMPLATE_LIST_ITEM_H
#define TEMPLATE_LIST_ITEM_H

#include <QWidget>
#include <QVariantMap>

namespace Ui {
class TemplateListItem;
}

class TemplateListItem : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateListItem(QWidget *parent = 0);
    ~TemplateListItem();

    void update_display(QVariantMap info);

signals:
    void editButton(QVariantMap info);
    void deleteButton(QString uuid);

private slots:
    void on_editButton_clicked();

    void on_deleteButton_clicked();

private:
    Ui::TemplateListItem *ui;
    QVariantMap        _info;
};

#endif // TEMPLATE_LIST_ITEM_H
