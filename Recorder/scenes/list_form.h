#ifndef LIST_FORM_H
#define LIST_FORM_H

#include <QWidget>
#include <QVariant>

namespace Ui {
class ListForm;
}

class ListForm : public QWidget
{
    Q_OBJECT

public:
    explicit ListForm(QWidget *parent = 0);
    ~ListForm();

public slots:
    void update_display(const QVariantMap& info);

signals:
    void button_clicked(const QString& uuid);

private slots:
    void on_downloadButton_clicked();

    void on_downloadButton_pressed();

    void on_downloadButton_released();

private:
    Ui::ListForm *ui;
    QVariantMap _info;
};

#endif // LIST_FORM_H
