#ifndef SCENE_RECORD_WARNING_H
#define SCENE_RECORD_WARNING_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Scene_Record_Warning;
}

class Scene_Record_Warning : public QDialog
{
    Q_OBJECT

public:
    static void ShowMessage(QPoint pos, QString message, int last_time = 1500);

    explicit Scene_Record_Warning(QWidget *parent = 0);
    ~Scene_Record_Warning();

    void set_content(QString &content);
    void set_last_time(int last_time = 1500);

    void showEvent(QShowEvent *event);

private slots:
    void on_close();


private:
    Ui::Scene_Record_Warning *ui;
    QTimer  *timer;
    int _last_time;
};

#endif // SCENE_RECORD_WARNING_H
