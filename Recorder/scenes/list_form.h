#ifndef LIST_FORM_H
#define LIST_FORM_H

#include <QWidget>
#include <QVariant>

namespace Ui {
class ListForm;
}

class ListForm : public QWidget {// public RecordDownloadReceiver 
  Q_OBJECT

 public:
  explicit ListForm();
  ~ListForm();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);

 signals:
  void itemClicked(const QVariantMap &info);

 public slots:
  void update_display(const QVariantMap &info);

 private slots:

  void on_downloadButton_clicked();

  void on_downloadButton_pressed();

  void on_downloadButton_released();

  void onDownloadingTick(int iPercent, int iDownloadPerSecond);

  void onDownloadPrompt(QString qstrInfo);

 private:
  Ui::ListForm *ui;
  QVariantMap _info;
  int _download_status;
};

#endif  // LIST_FORM_H
