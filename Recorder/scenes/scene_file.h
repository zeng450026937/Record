#ifndef SCENE_FILE_H
#define SCENE_FILE_H

#include <QtAV>
#include <QGroupBox>
#include <QtGui>
#include <QMenu>
#include <QAction>

class RecorderShared;
class Scene_File_Clip;
class Scene_File_DL;

namespace Ui {
class Scene_File;
}

class Scene_File : public QGroupBox
{
    Q_OBJECT

public:
    explicit Scene_File(RecorderShared *sharedData, QWidget *parent = 0);
    ~Scene_File();

    bool event(QEvent *e);

private:
    void init_filter_menu();
    void init_model();
    void init_play_info();
    void clear_file_info();
    void update_play_info(QString& file);
    void update_file_list(QVariantList& list);
    void update_mark_model(QVariantList& list);

    void on_add_mark(QString text);

    void syntaxChange(QString text,QRegExp::PatternSyntax);
    QString dateToReg(QDate startDate,QDate endDate);
    QString differentMonthDayReg(int startyear,QString startmonthStr,int startday,int endyear,QString endmonthStr,int endday);

private slots:
    void show_download_dlg(const QString& uuid);
    void download_conference(const QString& uuid);

    void clip_file_btn_clicked();

    void menu_triggered(QAction *action);

    void on_clearBtn_clicked();
    void on_searchButton_clicked();

    void on_file_open_btn_clicked();

    void on_play_btn_clicked(bool checked);

    void on_file_listView_clicked(const QModelIndex &index);

    void on_cancel_btn_clicked();

    void on_confirm_btn_clicked();

    void on_add_time_btn_clicked();

    void on_sub_time_btn_clicked();

    void time_slider_selected_changed(int value);
    void time_slider_set_changed(int value);

    void on_next_mark_btn_clicked();

    void on_pre_mark_btn_clicked();

    void on_file_mark_tableView_clicked(const QModelIndex &index);

    void on_comboBox_currentIndexChanged(int index);

    void on_typeComboBox_currentIndexChanged(int index);

    void clip_finished(QString output_file, int start_time);

    void on_start_time_textChanged(const QString &arg1);

    void on_end_time_textChanged(const QString &arg1);

    void on_sortBtn_clicked(bool checked);

private slots:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);

    void mediaStatusChanged(QtAV::MediaStatus status);
    void statusChanged(QtAV::AVPlayer::State status);

    void startPositionChanged(qint64 position);
    void stopPositionChanged(qint64 position);

private:
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QtAV::MediaStatus status);

private:
    Ui::Scene_File *ui;
    RecorderShared *_sharedData;

    Scene_File_Clip *_scene_file_clip;
    Scene_File_DL   *_scene_file_dl;

    QMenu       *filter_menu;
    QAction     *filter_action1;
    QAction     *filter_action2;
    QAction     *filter_action3;
    QAction     *filter_action4;

    QSortFilterProxyModel *sortFilter_proxyModel;

    int             _type;
    QString         _uuid;
    QVariantMap     _conf;
    QVariantMap     _file;
    QVariantMap     _mark;
    QVariantList    _file_list;
    QVariantList    _mark_list;

    int             _time_unit;
    int             _duration;
    QtAV::AVPlayer  *_player;

    int             _offset;
    QTime           _current_time;
    QTime           _total_time;
};

#endif // SCENE_FILE_H
