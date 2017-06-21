#ifndef CONF_DETAIL_H
#define CONF_DETAIL_H

#include <QWidget>

namespace Ui {
class ConfDetail;
}

class ConfDetail : public QWidget
{
    Q_OBJECT

public:
    explicit ConfDetail(QWidget *parent = 0);
    ~ConfDetail();

public slots:
    void setUuid(const QString &uuid);

signals:
    void goBack();

private:
    Ui::ConfDetail *ui;
};

#endif // CONF_DETAIL_H
