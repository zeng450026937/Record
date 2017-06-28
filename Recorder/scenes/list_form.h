#ifndef LIST_FORM_H
#define LIST_FORM_H

#include <QVariant>
#include <QWidget>
#include <QStandardItem>

namespace Ui {
class ListForm;
}

class ListForm : public QWidget {  // public RecordDownloadReceiver
  Q_OBJECT

 public:
  explicit ListForm(bool isDetail = false);
  ~ListForm();

 protected:
  virtual void mousePressEvent(QMouseEvent *event);

 signals:
  void itemClicked(const QVariantMap &info);

  public:

  void update_display(const QVariantMap &info);
  void CheckAndAliveData();

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
  bool _isDetail;

 
};

#endif  // LIST_FORM_H
