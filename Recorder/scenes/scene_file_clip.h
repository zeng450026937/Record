#ifndef SCENE_FILE_CLIP_H
#define SCENE_FILE_CLIP_H

#include <QDialog>
#include <QProcess>
#include <QTime>

class RecorderShared;
class Ffmpeg_Wrapper;

namespace Ui {
class Scene_File_Clip;
}

class Scene_File_Clip : public QDialog
{
    Q_OBJECT
public:
    explicit Scene_File_Clip(RecorderShared *sharedData, QWidget *parent = 0);
    ~Scene_File_Clip();

    void set_clip(QVariantMap clip_info);

signals:
    void output_file(QString file, int start_time);

private slots:
    void on_cancel_btn_clicked();

    void on_confirm_btn_clicked();

private:
    Ui::Scene_File_Clip *ui;
    RecorderShared *_sharedData;

    QString _input_file;
    QString _output_name;
    int   _start_time;
    int   _duration;

    QString _file_extention;

    Ffmpeg_Wrapper      *_ffmpeg;
};

#endif // SCENE_FILE_CLIP_H
