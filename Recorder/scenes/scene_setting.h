#ifndef SCENE_SETTING_H
#define SCENE_SETTING_H

#include <QGroupBox>
#include <QtGui>

class SettingControl;
class RecorderShared;
class Scene_Record_AddInfo;

namespace Ui {
class Scene_Setting;
}

class Scene_Setting : public QGroupBox
{
    Q_OBJECT

public:
    explicit Scene_Setting(RecorderShared *sharedData, QWidget *parent = 0);
    ~Scene_Setting();


private slots:
    //from _shareddata
    void receive_conference_notify(int state, bool result, QVariantMap conf);
    void receive_device_select_stateChanged(bool checked, QVariantMap device);

    void receive_editTemplate(QVariantMap info);
    void receive_deleteTemplate(QString uuid);

    void on_fresh_btn_clicked();

    void on_add_btn_clicked();

    void on_select_folder_btn_clicked();

    void setting_changed(QString text);

    void on_save_btn_clicked();

    void on_cancel_btn_clicked();

private:
    void initialize();

private:
    Ui::Scene_Setting *ui;
    RecorderShared *_sharedData;
    SettingControl  *m_pSettingControl;

    Scene_Record_AddInfo    *_scene_record_addinfo;
};

#endif // SCENE_SETTING_H
