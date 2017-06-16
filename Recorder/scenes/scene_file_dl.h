#ifndef SCENE_FILE_DL_H
#define SCENE_FILE_DL_H

#include <QDialog>

namespace Ui {
class Scene_File_DL;
}

class Scene_File_DL : public QDialog
{
    Q_OBJECT

public:
    explicit Scene_File_DL(QWidget *parent = 0);
    ~Scene_File_DL();

    void showEvent(QShowEvent *event);

// public slots:
//     void show_dl(const QString& uuid);
// 
// signals:
//     void redownload(const QString& uuid);

private slots:
    void on_close();

    void on_confirm_btn_clicked();

private:
    Ui::Scene_File_DL *ui;
    QString _uuid;
};

#endif // SCENE_FILE_DL_H
