#include "recorder.h"
#include <QApplication>
#include <QTextCodec>
#include <QStyleFactory>
#include <QStyle>
#include "recorder_shared.h"
#include "scenes/login_form.h"

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

    RecorderShared pRecorderShared;
	{
        LoginForm login(&pRecorderShared);
		if (login.exec() != QDialog::Accepted)
			return -1;
	}
	{
        Recorder w(&pRecorderShared);
		w.setFont(QFont("Consolas"));
		w.show();
	}

    return a.exec();
}
