#ifndef RECORDER_H
#define RECORDER_H

#include <QWidget>
#include <QtGui>

class RecorderShared;
class Scene_Record_Warning;
class Scene_Setting;
class Scene_Record;
class Scene_File;

namespace Ui {
class Recorder;
}

class Recorder : public QWidget
{
    Q_OBJECT

public:
    explicit Recorder(QWidget *parent = 0);
    ~Recorder();

    bool event(QEvent *event);
    bool eventFilter(QObject *obj, QEvent *ev);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void closeEvent(QCloseEvent *event);

private slots:
    void on_close_btn_clicked();

    void on_max_btn_clicked();

    void on_min_btn_clicked();

    void on_record_btn_clicked(bool checked);

    void on_file_btn_clicked(bool checked);

    void on_setting_btn_clicked(bool checked);

private:
    Ui::Recorder *ui;

    Scene_File      *_scene_file;
    Scene_Record    *_scene_record;
    Scene_Setting   *_scene_setting;

    QPoint  _movePos;

    RecorderShared     *_sharedData;
};

#endif // RECORDER_H
