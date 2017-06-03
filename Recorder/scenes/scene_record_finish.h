#ifndef SCENE_RECORD_FINISH_H
#define SCENE_RECORD_FINISH_H

#include <QDialog>

namespace Ui {
class Scene_Record_Finish;
}

class Scene_Record_Finish : public QDialog
{
    Q_OBJECT

public:
    explicit Scene_Record_Finish(QWidget *parent = 0);
    ~Scene_Record_Finish();

    void set_file_name(QString file_name);

private slots:
    void on_cancel_btn_clicked();

    void on_confirm_btn_clicked();

private:
    Ui::Scene_Record_Finish *ui;
};

#endif // SCENE_RECORD_FINISH_H
