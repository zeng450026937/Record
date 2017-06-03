#include "recorder.h"
#include <QApplication>
#include <QTextCodec>
#include <QStyleFactory>
#include <QStyle>
//#include "recorder_splash.h"

# pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QApplication::setStyle(QStyleFactory::create("fusion")); // styleName
    //QApplication::setPalette(QApplication::style()->standardPalette()); // 选择风格默认颜色

    QApplication a(argc, argv);

    //QScopedPointer<RecorderSplash> s(new RecorderSplash);
    //QTime time = QTime::currentTime();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    //禁止休眠
    ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

    //s->DiplayMessage(QString("Loading..."));
    //a.processEvents();
    Recorder w;
    w.setFont(QFont("Consolas"));//

    //while(time.msecsTo(QTime::currentTime()) <= 1300){
        //a.processEvents();
    //}
    //a.processEvents();

    w.show();

    //s->finish(&w);
    //s.reset();

    return a.exec();
}
