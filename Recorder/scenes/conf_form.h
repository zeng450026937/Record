#ifndef CONF_FORM_H
#define CONF_FORM_H

#include <QPushButton>
#include <QVariantMap>

namespace Ui {
class ConfForm;
}

class ConfForm : public QPushButton {
    Q_OBJECT

public:
    explicit ConfForm(QWidget* parent = 0);
    ~ConfForm();

public slots:
    void update_display(const QVariantMap& info);

signals:
    void clicked(const QString& uuid);

private:
    Ui::ConfForm* ui;
};

#endif // CONF_FORM_H
