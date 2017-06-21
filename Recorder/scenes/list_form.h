#ifndef LIST_FORM_H
#define LIST_FORM_H

#include <QWidget>
#include <QVariant>
#include <service/command/RecordDownloadReceiver.h>


namespace Ui {
class ListForm;
}

class ListForm :public RecordDownloadReceiver
{
    Q_OBJECT

public:
    explicit ListForm();
    ~ListForm();

public slots:
    void update_display(const QVariantMap& info);

private slots:

    void on_downloadButton_clicked();

    void on_downloadButton_pressed();

    void on_downloadButton_released();

    void onDownloadingTick(int iPercent,int iDownloadPerSecond);

protected:
    void onDownloadPrompt(QString qstrInfo) override;
private:
    Ui::ListForm *ui;
    QVariantMap _info;
    int         _download_status;
protected:

};

#endif // LIST_FORM_H
