#ifndef CONF_DETAIL_H
#define CONF_DETAIL_H

#include <QVariantMap>
#include <QWidget>

class ConferenceMode;
namespace Ui {
class ConfDetail;
}

class ConfDetail : public QWidget {
  Q_OBJECT

 public:
  explicit ConfDetail(QWidget *parent = 0);
  ~ConfDetail();

 public slots:
  void setInfo(const QVariantMap &info);

 signals:
  void goBack();

 private:
  Ui::ConfDetail *ui;
  QVariantMap _info;
  ConferenceMode *conf_mode;
};

#endif  // CONF_DETAIL_H
