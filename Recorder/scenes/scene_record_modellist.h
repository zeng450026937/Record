#ifndef SCENE_RECORD_MODELLIST_H
#define SCENE_RECORD_MODELLIST_H

#include <QDialog>
#include <QtGui>

class RecorderShared;
class Scene_Record_AddInfo;

namespace Ui {
class Scene_Record_ModelList;
}

class Scene_Record_ModelList : public QDialog
{
    Q_OBJECT

public:
    explicit Scene_Record_ModelList(RecorderShared *sharedData, QWidget *parent = 0);
    ~Scene_Record_ModelList();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void modelList_selected(QVariant info);

private:
    void init_file_model();

private slots:
    void set_conference_info(QVariant info);

    void on_confirm_btn_clicked();

    void on_cancel_btn_clicked();

    void on_listView_clicked(const QModelIndex &index);

private:
    Ui::Scene_Record_ModelList *ui;
    RecorderShared *_sharedData;

    Scene_Record_AddInfo    *_scene_record_addinfo;

    QPoint  _movePos;
};

#endif // SCENE_RECORD_MODELLIST_H
