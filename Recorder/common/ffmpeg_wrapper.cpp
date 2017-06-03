#include "ffmpeg_wrapper.h"
#include <QDebug>
#include <QCursor>

Ffmpeg_Wrapper::Ffmpeg_Wrapper(QObject *parent) :
    QObject(parent),
    _process(new QProcess(this))
{
    connect(_process,SIGNAL(readyReadStandardOutput()),this,SLOT(outputCommand()));
    connect(_process,SIGNAL(readyReadStandardError()),this,SLOT(outputCommand()));
    connect(_process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(on_finished(int,QProcess::ExitStatus)));
}

Ffmpeg_Wrapper::~Ffmpeg_Wrapper()
{

}

void Ffmpeg_Wrapper::outputCommand(){
    QByteArray cmdoutput = _process->readAllStandardOutput();
    char *t=cmdoutput.data();
    QString txtoutput=QString::fromLocal8Bit(t);
    qDebug()<<txtoutput;
    cmdoutput = _process->readAllStandardError();
    t=cmdoutput.data();
    txtoutput=QString::fromLocal8Bit(t);
    qDebug()<<txtoutput;
}

void Ffmpeg_Wrapper::on_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    //TBD
}

void  Ffmpeg_Wrapper::ClipFile(QString exe_path,QString input_file,QString output_file,int start_time,int duration)
{
    QStringList strlist = this->make_clip_command(input_file,output_file,start_time,duration);

    qDebug()<<strlist;

    _process->start(exe_path , strlist);

    _process->waitForFinished(3000);
}
void  Ffmpeg_Wrapper::CombineFile(QString exe_path,QStringList input_file_list,QString output_file)
{
    QStringList strlist = this->make_combine_command(input_file_list,output_file);

    qDebug()<<strlist;

    _process->start(exe_path , strlist);

    _process->waitForFinished(3000);
}


QStringList Ffmpeg_Wrapper::make_clip_command(QString input_file,QString output_file,int start_time,int duration)
{
    _input_file = input_file;
    _output_file = output_file;

    _start_time.setHMS((start_time/3600)%60, (start_time/60)%60, start_time%60, (start_time*1000)%1000);
    _duration.setHMS((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);

    QStringList str;

    str += "-y";
    //str += "-f";
    //str += _DEFAULT_FILE;
    str += "-i";
    str += _input_file;
    str += "-ss";
    str += _start_time.toString("hh:mm:ss");
    str += "-t";
    str += _duration.toString("hh:mm:ss");
    str += "-acodec";
    str += "copy";
    //str += "libamr_nb";
    str += _output_file;

    return str;
}
QStringList Ffmpeg_Wrapper::make_combine_command(QStringList input_file_list,QString output_file)
{
    QStringList str;

    str += "-y";
    //str += "-f";
    //str += _DEFAULT_FILE;
    str += "-i";
    str += "concat:" + input_file_list.join("|");
    str += "-acodec";
    str += "copy";
    //str += "libamr_nb";
    str += output_file;

    return str;
}
