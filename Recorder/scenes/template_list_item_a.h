#ifndef TEMPLATE_LIST_ITEM_A_H
#define TEMPLATE_LIST_ITEM_A_H

#include <QWidget>
#include <QVariantMap>

namespace Ui {
class TemplateListItemA;
}

class TemplateListItemA : public QWidget
{
    Q_OBJECT

public:
    explicit TemplateListItemA(QWidget *parent = 0);
    ~TemplateListItemA();

    void update_display(QVariantMap info);

private:
    Ui::TemplateListItemA *ui;
    QVariantMap        _info;
    QString            _prefix;
};

#endif // TEMPLATE_LIST_ITEM_A_H
