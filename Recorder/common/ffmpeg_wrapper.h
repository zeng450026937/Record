#ifndef FFMPEG_WRAPPER_H
#define FFMPEG_WRAPPER_H

#include <QObject>
#include <QProcess>
#include <QTime>

class Ffmpeg_Wrapper : public QObject
{
    Q_OBJECT
public:
    Ffmpeg_Wrapper(QObject *parent = 0);
    ~Ffmpeg_Wrapper();

public slots:
    void    outputCommand();
    void    on_finished(int exitCode, QProcess::ExitStatus exitStatus);

    void    ClipFile(QString exe_path,QString input_file,QString output_file,int start_time,int duration);
    void    CombineFile(QString exe_path,QStringList input_file_list,QString output_file);

private:
    QStringList make_clip_command(QString input_file,QString output_file,int start_time,int duration);
    QStringList make_combine_command(QStringList input_file_list,QString output_file);

private:
    QString _input_file;
    QString _output_file;
    QTime _start_time;
    QTime _duration;
    QProcess *_process;
};

#endif // FFMPEG_WRAPPER_H
