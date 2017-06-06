#include "scene_file.h"
#include "ui_scene_file.h"
#include "qqslider.h"

#include <QTime>
#include <QMessageBox>
#include <QListView>

#include "recorder_shared.h"
#include "config.h"
#include "scene_buttondelegate.h"
#include "scene_file_clip.h"
#include "scene_file_dl.h"
#include "scene_record_warning.h"
#include "listitem_delegate.h"


Scene_File::Scene_File(RecorderShared *sharedData, QWidget *parent) :
    QGroupBox(parent),
    _sharedData(sharedData),
    ui(new Ui::Scene_File),
    filter_menu(new QMenu(this)),
    filter_action1(new QAction(filter_menu)),
    filter_action2(new QAction(filter_menu)),
    filter_action3(new QAction(filter_menu)),
    filter_action4(new QAction(filter_menu)),
    sortFilter_proxyModel(new QSortFilterProxyModel(this)),
    _player(new QtAV::AVPlayer(this)),
    _scene_file_clip(new Scene_File_Clip(sharedData)),
    _scene_file_dl(new Scene_File_DL),
    _time_unit(1000),
    _duration(0),
    _offset(0),
    _type(1)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);//去掉标题栏

    //disable show subvalue
    ui->time_slider->showSubValue(false);
    ui->MarkBox->combine_slider(ui->time_slider);

    ui->button_groupBox->hide();
    ui->time_groupBox->hide();

    QListView* listview = new QListView(ui->comboBox);
    listview->setAutoScroll(true);
    ui->comboBox->setView(listview);
    ui->comboBox->setEditable(false);

    Config* config = _sharedData->GetConfig();
    QStringList combo_list = config->_type_combo;

    QListView* typeCombo_listview = new QListView(ui->typeComboBox);
    typeCombo_listview->setAutoScroll(true);
    ui->typeComboBox->setView(typeCombo_listview);
    ui->typeComboBox->setEditable(false);
    ui->typeComboBox->addItems( combo_list );

    ui->sortBtn->setChecked(false);

    this->init_filter_menu();

    this->init_model();

    this->init_play_info();

    connect(_scene_file_clip,SIGNAL(output_file(QString,int)),this,SLOT(clip_finished(QString,int)));
    connect(_scene_file_dl,SIGNAL(redownload(QString)),this,SLOT(download_conference(QString)));

    connect(ui->time_slider,SIGNAL(select_value_changed(int)),this,SLOT(time_slider_selected_changed(int)));
    connect(ui->time_slider,SIGNAL(set_value_changed(int)),this,SLOT(time_slider_set_changed(int)));

    connect(_player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));
    connect(_player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    connect(_player,SIGNAL(mediaStatusChanged(QtAV::MediaStatus)),this,SLOT(mediaStatusChanged(QtAV::MediaStatus)));
    connect(_player,SIGNAL(stateChanged(QtAV::AVPlayer::State)),this, SLOT(statusChanged(QtAV::AVPlayer::State)));
    connect(_player,SIGNAL(startPositionChanged(qint64)),this,SLOT(startPositionChanged(qint64)));
    connect(_player,SIGNAL(stopPositionChanged(qint64)),this,SLOT(stopPositionChanged(qint64)));
}

Scene_File::~Scene_File()
{
    if(_scene_file_clip){
        delete _scene_file_clip;
    }

    if(_scene_file_dl){
        delete _scene_file_dl;
    }

    delete ui;
}

void Scene_File::init_filter_menu()
{
    filter_action1->setText("今天");
    filter_action1->setIcon(QIcon(":/resource/u2136.png"));
    filter_action2->setText("当月");
    filter_action2->setIcon(QIcon(":/resource/u2136.png"));
    filter_action3->setText("三个月内");
    filter_action3->setIcon(QIcon(":/resource/u2136.png"));
    filter_action4->setText("全部");
    filter_action4->setIcon(QIcon(":/resource/u2136.png"));

    filter_action1->setCheckable(true);
    filter_action2->setCheckable(true);
    filter_action3->setCheckable(true);
    filter_action4->setCheckable(true);

    filter_menu->addAction(filter_action1);
    filter_menu->addAction(filter_action2);
    filter_menu->addAction(filter_action3);
    filter_menu->addAction(filter_action4);


    filter_menu->setStyleSheet("QMenu{ font: 9pt;border:0;margin:0;padding:4px;}"
                               "QMenu::item {"
                               "    border: 1px solid transparent; "
                               "    padding: 4px 9px 4px 30px; " // Top Right Bottom Left
                               "}"
                               "QMenu::item:selected {"
                               "    color:rgb(85, 170, 255);    "
                               "    border-bottom: 1px solid #335EA8; "
                               "}"
                               );



    ui->filterBtn->setMenu(filter_menu);

    connect(filter_menu,SIGNAL(triggered(QAction*)),this,SLOT(menu_triggered(QAction*)));

    filter_action1->setChecked(true);
    filter_menu->triggered(filter_action1);
}

bool Scene_File::event(QEvent *e)
{
    switch(e->type()) {
    case QEvent::KeyPress:
        if (const QKeyEvent *key_event = static_cast<const QKeyEvent *>(e)){

            switch(key_event->key()){

            case Qt::Key_Return:
                if(this->focusWidget() == ui->mark_lineEdit){
                    this->on_add_mark(ui->mark_lineEdit->text());
                }
            }
        }
        break;

    case QEvent::Show:

        if(!ui->file_listView->currentIndex().isValid()){

            this->clear_file_info();
        }

        break;

    default:
        break;
    }

    return QGroupBox::event(e);
}

void Scene_File::init_model()
{
    ui->file_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->file_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->file_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->file_listView->setWrapping(false);
    ui->file_listView->setModel(sortFilter_proxyModel);

    ListItemDelegate* itemDelegate = new ListItemDelegate(this);
    ui->file_listView->setItemDelegateForColumn(0,itemDelegate);
    connect(itemDelegate,SIGNAL(download_item(QString)),this,SLOT(show_download_dlg(QString)));

    ui->file_mark_tableView->verticalHeader()->hide();
    ui->file_mark_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //no edit
    ui->file_mark_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->file_mark_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->file_mark_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //ui->file_mark_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->file_mark_tableView->horizontalHeader()->hide();

    //利用setModel()方法将数据模型与QTableView绑定
    ui->file_mark_tableView->setModel( _sharedData->GetModel(ModelUpdater::MarkModel) );
    ui->file_mark_tableView->setWordWrap(true);


    ClipButtonDelegate *_clip_button = new ClipButtonDelegate(QObject::tr("剪辑"),this);
    ui->file_mark_tableView->setItemDelegateForColumn(3,_clip_button);

    connect(_clip_button,SIGNAL(clip_file()),this,SLOT(clip_file_btn_clicked()));
}

void Scene_File::init_play_info()
{
    if(_player->isPlaying())
        _player->stop();

    _player->setFile("");

    _player->load();

    _total_time.setHMS(0,0,0,0);
    _current_time.setHMS(0,0,_offset,0);

    ui->total_time->setText( _total_time.toString("hh:mm:ss") );
    ui->current_time->setText( _current_time.toString("hh:mm:ss") );

    ui->time_slider->setValue(0);
    ui->time_slider->setRange(0,0);

    ui->start_time->blockSignals(true);
    ui->start_time->setText( _current_time.toString("hh:mm:ss") );
    ui->start_time->blockSignals(false);
    ui->end_time->blockSignals(true);
    ui->end_time->setText( _total_time.toString("hh:mm:ss") );
    ui->end_time->blockSignals(false);
}

void Scene_File::clear_file_info()
{
    this->init_play_info();
    ui->MarkBox->clear_marks();

    ui->file_mark_tableView->setEnabled(false);
    ui->stackedWidget->setEnabled(false);
    ui->comboBox->setEnabled(false);
}
void Scene_File::update_play_info(QString& file)
{
    if(_player->file() == file)
        return;

    _player->setFile(file);

    _player->load();
}
void Scene_File::update_file_list(QVariantList& list)
{
    QStringList filelist;
    int num(0);

    foreach (QVariant var, list) {
        QFileInfo fileinfo( var.toMap().value("path").toString() );

        int start_time = var.toMap().value("start_time").toInt();

        QString folder;
        folder = _sharedData->GetFolder(_type, _uuid);

        QString name;
        if( start_time < 0)
            name = folder + " : " + fileinfo.baseName();
        else
            name = QString("剪辑%1").arg(++num) + " : " + fileinfo.baseName();

        filelist << name;
    }

    ui->comboBox->blockSignals(true);
    ui->comboBox->clear();

    if( filelist.count() > 0){
        ui->comboBox->addItems(filelist);
        ui->comboBox->setCurrentIndex(0);

        ui->comboBox->setEnabled(true);
    }
    else{
        ui->comboBox->setEnabled(false);
    }
    ui->comboBox->blockSignals(false);

    if( filelist.count() > 0)
        ui->comboBox->currentIndexChanged(0);
}

void Scene_File::update_mark_model(QVariantList& list)
{
    _sharedData->ListToModel(ModelUpdater::MarkModel, list);
    if(list.count() > 0){
        ui->file_mark_tableView->resizeColumnToContents(0);
        ui->file_mark_tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
        ui->file_mark_tableView->resizeColumnToContents(2);
        ui->file_mark_tableView->setColumnWidth(3,60);
    }
}

void Scene_File::on_add_mark(QString text)
{
    //if(text.isEmpty())
        //return;

    QVariantMap mark_info = ui->MarkBox->add_mark(text);

    _sharedData->AddMark( RecorderShared::MarkModel,
                          _uuid,
                          mark_info);

    ui->mark_lineEdit->clear();

    _mark_list.append(mark_info);

    if(_mark_list.count() == 1){
        ui->file_mark_tableView->resizeColumnToContents(0);
        ui->file_mark_tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
        ui->file_mark_tableView->resizeColumnToContents(2);
        ui->file_mark_tableView->setColumnWidth(3,60);
    }
}

void Scene_File::syntaxChange(QString text,QRegExp::PatternSyntax psyntax)
{
    //在这里要根据组合框中的内容重新设置过滤器语法，因此要先将组何况中的数据去出来，然后构造setFilterRegExp（）的参数
    //也就是QRegExp 对象，而QRegExp 的第三个参数就是组合框中的语法

    //QRegExp::Wildcard //通配符
    //QRegExp::FixedString //字符串匹配
    //QRegExp::RegExp //正则表达式
    QRegExp syntax= QRegExp(text,Qt::CaseInsensitive, psyntax);
    sortFilter_proxyModel->setFilterRegExp(syntax);  //设置过滤语法
}

QString Scene_File::dateToReg(QDate startDate,QDate endDate)
{
    bool ok;

    int startyear = startDate.toString("yyyy").toInt(&ok,10);
    int startmonth = startDate.toString("MM").toInt(&ok,10);
    QString startmonthStr = startDate.toString("MM");
    int startday = startDate.toString("dd").toInt(&ok,10);

    int endyear = endDate.toString("yyyy").toInt(&ok,10);
    int endmonth = endDate.toString("MM").toInt(&ok,10);
    QString endmonthStr = endDate.toString("MM");
    int endday = endDate.toString("dd").toInt(&ok,10);

    QString RegText;
    if((endyear - startyear) > 0){
        if(startmonth < 9){
            RegText += "(";
            RegText += QString::number(startyear);
            RegText += "0[";
            RegText += QString::number(startmonth + 1);
            RegText += "-9])|";
            RegText += "(";
            RegText += QString::number(startyear);
            RegText += "1[0-2])|";
        }
        else if(startmonth != 12){
            RegText += "(";
            RegText += QString::number(startyear);
            RegText += "1[";
            RegText += QString::number(startmonth % 10 + 1);
            RegText += "-2])|";
            RegText += "(";
            RegText += QString::number(startyear);
            RegText += "1[0-2])|";
        }

        if(endmonth > 1 && endmonth < 11){
            RegText += "(";
            RegText += QString::number(endyear);
            RegText += "0[1-";
            RegText += QString::number(endmonth - 1);
            RegText += "])|";
        }
        else if(endmonth != 1){
            RegText += "(";
            RegText += QString::number(endyear);
            RegText += "[0-9]";
            RegText += ",";
            RegText += QString::number(endyear);
            RegText += "1[0-";
            RegText += QString::number(endmonth % 10 - 1);
            RegText += "])|";
        }

        RegText += differentMonthDayReg(startyear,startmonthStr,startday,endyear,endmonthStr,endday);
    }
    else{
        if((endmonth-startmonth) > 1){
            if(endmonth < 11){
                RegText += "(";
                RegText += QString::number(startyear);
                RegText += "0[";
                RegText += QString::number(startmonth);
                RegText += "-";
                RegText += QString::number(endmonth - 1);
                RegText += "])|";
            }
            else if(startmonth > 8){
                RegText += "(";
                RegText += QString::number(startyear);
                RegText += "1[";
                RegText += QString::number((startmonth+1)%10);
                RegText += "-";
                RegText += QString::number(endmonth % 10 - 1);
                RegText += "])|";
            }
            else{
                RegText += "(";
                RegText += QString::number(startyear);
                RegText += "0[";
                RegText += QString::number(startmonth + 1);
                RegText += "-9],";
                RegText += QString::number(startyear);
                RegText += "1[0-";
                RegText += QString::number(endmonth % 10 - 1);
                RegText += "])|";
            }
            RegText += differentMonthDayReg(startyear,startmonthStr,startday,endyear,endmonthStr,endday);
        }
        else if((endmonth-startmonth) == 1){
            RegText += differentMonthDayReg(startyear,startmonthStr,startday,endyear,endmonthStr,endday);
        }
        else{
            if((endday/10 -startday/10) > 1){
                RegText += "(";
                RegText += QString::number(startyear);
                RegText += "";
                RegText += startmonthStr;
                RegText += "[";
                RegText += QString::number(startday/10 + 1);
                RegText += "-";
                RegText += QString::number(endday/10 + 1);
                RegText += "]|";

                RegText += QString::number(startyear);
                RegText += "";
                RegText += startmonthStr;
                RegText += "";
                RegText += QString::number(startday/10);
                RegText += "[";
                RegText += QString::number(startday%10);
                RegText += "-9]|";

                RegText += QString::number(endyear);
                RegText += "";
                RegText += endmonthStr;
                RegText += "";
                RegText += QString::number(endday/10);
                RegText += "[0-";
                RegText += QString::number(startday%10);
                RegText += "]";
            }
            else if((endday/10 -startday/10) == 1){
                RegText += QString::number(startyear);
                RegText += "";
                RegText += startmonthStr;
                RegText += "";
                RegText += QString::number(startday/10);
                RegText += "[";
                RegText += QString::number(startday%10);
                RegText += "-9]|";

                RegText += QString::number(endyear);
                RegText += "";
                RegText += endmonthStr;
                RegText += "";
                RegText += QString::number(endday/10);
                RegText += "[0-";
                RegText += QString::number(startday%10);
                RegText += "]";
            }
            else{
                RegText += QString::number(startyear);
                RegText += "";
                RegText += startmonthStr;
                RegText += "";
                RegText += QString::number(startday/10);
                RegText += "[";
                RegText += QString::number(startday%10);
                RegText += "-";
                RegText += QString::number(endday%10);
                RegText += "]";
            }
        }
    }

    return RegText;
}

QString Scene_File::differentMonthDayReg(int startyear,QString startmonthStr,int startday,int endyear,QString endmonthStr,int endday)
{
    QString RegText;

    if(startday/10 < 3){
        RegText += "(";
        RegText += QString::number(startyear);
        RegText += "";
        RegText += startmonthStr;
        RegText += "[";
        RegText += QString::number(startday/10 + 1);
        RegText += "-3])|";

        RegText += "(";
        RegText += QString::number(startyear);
        RegText += "";
        RegText += startmonthStr;
        RegText += "";
        RegText += QString::number(startday/10);
        RegText += "[";
        RegText += QString::number(startday%10);
        RegText += "-9])|";
    }
    else{
        RegText += "(";
        RegText += QString::number(startyear);
        RegText += "";
        RegText += startmonthStr;
        RegText += "3[";
        RegText += QString::number(startday%10);
        RegText += "-1])|";
    }

    if(endday/10 > 0){
        RegText += "(";
        RegText += QString::number(endyear);
        RegText += "";
        RegText += endmonthStr;
        RegText += "[0-";
        RegText += QString::number(endday/10 + 1);
        RegText += "])|";

        RegText += "(";
        RegText += QString::number(endyear);
        RegText += "";
        RegText += endmonthStr;
        RegText += "";
        RegText += QString::number(endday/10);
        RegText += "[0-";
        RegText += QString::number(endday%10);
        RegText += "])";
    }
    else{
        RegText += "(";
        RegText += QString::number(endyear);
        RegText += "";
        RegText += endmonthStr;
        RegText += "";
        RegText += QString::number(endday/10);
        RegText += "[0-";
        RegText += QString::number(endday%10);
        RegText += "])";
    }
    return RegText;
}

void Scene_File::menu_triggered(QAction *action)
{
    sortFilter_proxyModel->setFilterKeyColumn(2);
    ui->lineEdit->blockSignals(true);
    ui->lineEdit->clear();
    ui->lineEdit->blockSignals(false);

    int count = filter_menu->actions().count();
    for(int i = 0; i < count; i++){
        if (filter_menu->actions().at(i) == action)
            continue;
        filter_menu->actions().at(i)->setChecked(false);
        filter_menu->actions().at(i)->setIconVisibleInMenu(false);
    }

    auto time = QDate::currentDate();

    action->setChecked(true);
    action->setIconVisibleInMenu(true);

    if (action == filter_action1){//今天
        this->syntaxChange(time.toString("yyyyMMdd"),QRegExp::FixedString);
    }
    if (action == filter_action2){//当月
        this->syntaxChange(time.toString("yyyyMM"),QRegExp::FixedString);
    }
    if (action == filter_action3){//三个月内
        this->syntaxChange(dateToReg(time.addMonths(-3),time),QRegExp::RegExp);
    }
    if (action == filter_action4){//全部
        this->syntaxChange("",QRegExp::FixedString);
    }
}

void Scene_File::on_clearBtn_clicked()
{
    ui->lineEdit->clear();

    sortFilter_proxyModel->setFilterKeyColumn(5);

    this->syntaxChange(ui->lineEdit->text(),QRegExp::FixedString);

    if (ui->lineEdit->text() == ""){
        filter_action4->setChecked(true);
        filter_action4->setIconVisibleInMenu(true);
    }else{
        int count = filter_menu->actions().count();
        for(int i = 0; i < count; i++){
            filter_menu->actions().at(i)->setChecked(false);
            filter_menu->actions().at(i)->setIconVisibleInMenu(false);
        }
    }
}

void Scene_File::show_download_dlg(const QString& uuid)
{
    if (_scene_file_dl == nullptr){
        _scene_file_dl = new Scene_File_DL();
    }

    QPoint pos = this->parentWidget()->parentWidget()->parentWidget()->geometry().topLeft() + ui->file_listView->geometry().center();
    _scene_file_dl->move(pos.x() - _scene_file_dl->width() / 2, pos.y() - _scene_file_dl->height() / 2);

    _scene_file_dl->show_dl(uuid);
}
void Scene_File::download_conference(const QString& uuid)
{
    _sharedData->DownLoadConference( _type ,uuid );
}

void Scene_File::clip_file_btn_clicked()
{
    ui->file_mark_tableView->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->file_listView->setEnabled(false);
    ui->mark_lineEdit->setEnabled(false);
    ui->add_time_btn->setEnabled(false);
    ui->sub_time_btn->setEnabled(false);
    ui->pre_mark_btn->setEnabled(false);
    ui->next_mark_btn->setEnabled(false);

    ui->button_groupBox->show();
    ui->time_groupBox->show();

    QVariantMap mark;

    mark = _mark_list.at( ui->file_mark_tableView->currentIndex().row() ).toMap();

    QString mark_time = mark.value("time").toString();

    QString sub_time = _sharedData->GetConfig()->_clip_prev;
    sub_time = sub_time.left(sub_time.length()-1);

    QString add_time = _sharedData->GetConfig()->_clip_next;
    add_time = add_time.left(add_time.length()-1);

    ui->time_slider->set_handle_value(mark_time.toInt() - sub_time.toInt(),
                                      mark_time.toInt() + add_time.toInt());

}

void Scene_File::on_file_open_btn_clicked()
{
    QString path=_sharedData->GetConfig()->_output_dir; //获取程序当前目录
    path.replace("/","\\");//将地址中的"/"替换为"\"，因为在Windows下使用的是"\"。
    QProcess::startDetached("explorer.exe "+path);//打开上面获取的目录
    qDebug()<<"open output dir:"<<path;
}

void Scene_File::on_play_btn_clicked(bool checked)
{
    Q_UNUSED(checked);

    if (!_player->isPlaying()) {
        _player->play();

        return;
    }
    _player->pause(!_player->isPaused());

}

void Scene_File::on_file_listView_clicked(const QModelIndex &index)
{
    this->clear_file_info();

    _conf = sortFilter_proxyModel->mapToSource(index).data(Qt::UserRole).toMap();

    if(_conf.contains("uuid")){
        _type = 1;
        _uuid = _conf.value("uuid").toString();
    }
    else if(_conf.contains("conference_uuid")){
        _type = 0;
        _uuid = _conf.value("conference_uuid").toString();
    }

    _file_list = _sharedData->GetFileList(_uuid);

    this->update_file_list( _file_list );

    _mark_list = _sharedData->GetMark(_uuid);
    //to avoid displaying the marks which are out of play range，update mark model in update_play_info()
    this->update_mark_model( _mark_list );
}

void Scene_File::on_cancel_btn_clicked()
{
    ui->file_mark_tableView->setEnabled(true);
    ui->comboBox->setEnabled(true);
    ui->file_listView->setEnabled(true);
    ui->mark_lineEdit->setEnabled(true);
    ui->add_time_btn->setEnabled(true);
    ui->sub_time_btn->setEnabled(true);
    ui->pre_mark_btn->setEnabled(true);
    ui->next_mark_btn->setEnabled(true);

    ui->button_groupBox->hide();
    ui->time_groupBox->hide();

    ui->time_slider->showSelectValue(false);

    _player->setTimeRange(0, _duration*_time_unit);
    _player->setPosition(0);
    _player->setStopPosition(std::numeric_limits<qint64>::max());
    _player->pause( true );
    this->positionChanged( 0 );
}

void Scene_File::on_confirm_btn_clicked()
{
    if (_scene_file_clip == nullptr){
        _scene_file_clip = new Scene_File_Clip(_sharedData);
    }

    QPoint pos = this->parentWidget()->parentWidget()->parentWidget()->geometry().topLeft() + ui->rightWidget->geometry().center();
    _scene_file_clip->move(pos.x() - _scene_file_clip->width() / 2, pos.y() - _scene_file_clip->height() / 2);

    QFileInfo fileinfo( _file.value("path").toString() );

    QString output_name = fileinfo.dir().dirName() +" - "+ _mark.value("mark").toString();

    QVariantMap clip_info;
    clip_info.insert("input_file",  fileinfo.absoluteFilePath());
    clip_info.insert("output_name", output_name);
    clip_info.insert("start_time",  ui->time_slider->get_start_value());
    clip_info.insert("duration",    ui->time_slider->get_select_value());

    _scene_file_clip->set_clip( clip_info );

    ui->rightWidget->setEnabled(false);

    if ( _scene_file_clip->exec() == QDialog::Accepted ) {

        ui->cancel_btn->clicked();
    }

    ui->rightWidget->setEnabled(true);
}

void Scene_File::on_add_time_btn_clicked()
{
    if(_player->state() == QtAV::AVPlayer::State::PlayingState ||
       _player->state() == QtAV::AVPlayer::State::PausedState ){

        qint64 _positon = _player->position() + 30 * 1000;
        if(_positon > _player->stopPosition()){
            _positon = _player->stopPosition();
        }

        _player->setPosition(_positon);
    }
}

void Scene_File::on_sub_time_btn_clicked()
{
    if(_player->state() == QtAV::AVPlayer::State::PlayingState ||
       _player->state() == QtAV::AVPlayer::State::PausedState ){

        qint64 _positon = _player->position() - 30 * 1000;
        if(_positon < _player->startPosition()){
            _positon = _player->startPosition();
        }

        _player->setPosition(_positon);
    }
}

void Scene_File::time_slider_selected_changed(int value)
{
    QTime time;
    time.setHMS((value/3600)%60, (value/60)%60, value%60, (value*1000)%1000);

    ui->selected_time->setText(time.toString("hh:mm:ss"));

    int seconds = ui->time_slider->get_start_value();

    time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);

    ui->start_time->blockSignals(true);
    ui->start_time->setText(time.toString("hh:mm:ss"));
    ui->start_time->blockSignals(false);

    seconds = ui->time_slider->get_end_value();

    time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);

    ui->end_time->blockSignals(true);
    ui->end_time->setText(time.toString("hh:mm:ss"));
    ui->end_time->blockSignals(false);

    _player->setTimeRange((ui->time_slider->get_start_value() - _offset) * _time_unit, (ui->time_slider->get_end_value() - _offset) * _time_unit);

    if( !_player->isPlaying() ){
         _player->play();
    }
    _player->setPosition((ui->time_slider->get_start_value() - _offset) * _time_unit);
    _player->pause( false );
}

void Scene_File::time_slider_set_changed(int value)
{
    QTime time;
    time.setHMS((value/3600)%60, (value/60)%60, value%60, (value*1000)%1000);
    ui->selected_time->setText(time.toString("hh:mm:ss"));

    int seconds = ui->time_slider->get_start_value();
    time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);

    ui->start_time->blockSignals(true);
    ui->start_time->setText(time.toString("hh:mm:ss"));
    ui->start_time->blockSignals(false);

    seconds = ui->time_slider->get_end_value();
    time.setHMS((seconds/3600)%60, (seconds/60)%60, seconds%60, (seconds*1000)%1000);

    ui->end_time->blockSignals(true);
    ui->end_time->setText(time.toString("hh:mm:ss"));
    ui->end_time->blockSignals(false);

    _player->pause( true );

    _player->setTimeRange((ui->time_slider->get_start_value() - _offset) * _time_unit, (ui->time_slider->get_end_value() - _offset) * _time_unit);

    _player->setPosition((ui->time_slider->get_start_value() - _offset) * _time_unit);
}

void Scene_File::durationChanged(qint64 duration)
{
    if (duration) {
        _duration = duration/_time_unit + _offset;

        ui->time_slider->setRange(_offset, _duration);

        _total_time.setHMS((_duration/3600)%60, (_duration/60)%60, _duration%60, (_duration*1000)%1000);

        ui->total_time->setText(_total_time.toString("hh:mm:ss"));

        //drop marks which are out of play range
        for(int i = 0; i < _mark_list.count(); i++){
            int time = _mark_list.at(i).toMap().value("time").toInt();
            if(time > _duration || time < _offset)
                _mark_list.removeAt(i);
        }
        //update mark
        this->update_mark_model( _mark_list );
        ui->MarkBox->add_mark(_mark_list);

    }else{

        this->positionChanged( _player->startPosition() );
    }
}

void Scene_File::positionChanged(qint64 progress)
{
    qint64 _progress = progress / 1000 + _offset;

    if (!ui->time_slider->isSliderDown()) {
        ui->time_slider->setValue( _progress );
    }

    _current_time.setHMS((_progress/3600)%60, (_progress/60)%60, _progress%60, (_progress*1000)%1000);

    ui->current_time->setText(_current_time.toString("hh:mm:ss"));
}

void Scene_File::mediaStatusChanged(QtAV::MediaStatus status)
{
    handleCursor(status);

    // handle status message
    switch (status) {

    case QtAV::MediaStatus::UnknownMediaStatus:
    case QtAV::MediaStatus::NoMedia:
    case QtAV::MediaStatus::LoadedMedia:    // if auto load and source is set. player is stopped state
    case QtAV::MediaStatus::BufferingMedia: // NOT IMPLEMENTED
    case QtAV::MediaStatus::BufferedMedia:  // when playing //NOT IMPLEMENTED
        setStatusInfo(QString());
        break;
    case QtAV::MediaStatus::LoadingMedia:   // when source is set
        setStatusInfo(tr("Loading..."));
        break;
    case QtAV::MediaStatus::StalledMedia:   // insufficient buffering or other interruptions (timeout, user interrupt)
        setStatusInfo(tr("Media Stalled"));
        break;
    case QtAV::MediaStatus::EndOfMedia: // Playback has reached the end of the current media. The player is in the StoppedState.
        ui->play_btn->setChecked(false);
        _player->setPosition(0);
        _player->pause(true);
        break;
    case QtAV::MediaStatus::InvalidMedia:   // what if loop > 0 or stopPosition() is not mediaStopPosition()?
        setStatusInfo(tr("InvalidMedia"));
        break;
    }
}


void Scene_File::statusChanged(QtAV::AVPlayer::State status)
{
    switch (status) {
    case QtAV::AVPlayer::State::StoppedState:
        ui->play_btn->setText(QObject::tr("播放"));
        break;
    case QtAV::AVPlayer::State::PlayingState: /// Start to play if it was stopped, or resume if it was paused
        ui->play_btn->setText(QObject::tr("暂停"));
        break;
    case QtAV::AVPlayer::State::PausedState:
        ui->play_btn->setText(QObject::tr("播放"));
        break;
    };
}

void Scene_File::startPositionChanged(qint64 position)
{  
    Q_UNUSED(position);
}
void Scene_File::stopPositionChanged(qint64 position)
{
    Q_UNUSED(position);
}

void Scene_File::handleCursor(QtAV::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QtAV::MediaStatus::LoadingMedia ||
        //status == QtAV::MediaStatus::BufferingMedia ||
        status == QtAV::MediaStatus::StalledMedia)
        setCursor(QCursor(Qt::WaitCursor));
    else
        unsetCursor();
#endif
}

void Scene_File::setStatusInfo(const QString &info)
{
    Q_UNUSED(info);
    /*
    _statusInfo = info;
    if (!_statusInfo.isEmpty())
        ui->mark_lineEdit->setText(QString("%1 | %2").arg(_trackInfo).arg(_statusInfo));
    else
        ui->mark_lineEdit->setText(_trackInfo);
    */
}

void Scene_File::on_next_mark_btn_clicked()
{
    if(ui->file_mark_tableView->model()->rowCount() <= 0)
        return;

    int current(0);

    if(!ui->file_mark_tableView->currentIndex().isValid()){
        current = 0;
    }
    else{
        current = ui->file_mark_tableView->currentIndex().row();
        current = current >= (_mark_list.count()-1)?
                  (_mark_list.count()-1) : (current+1);
    }

    ui->file_mark_tableView->selectRow(current);

    this->on_file_mark_tableView_clicked( ui->file_mark_tableView->model()->index(current,0) );
}

void Scene_File::on_pre_mark_btn_clicked()
{
    if(ui->file_mark_tableView->model()->rowCount() <= 0)
        return;

    int current(0);

    if(!ui->file_mark_tableView->currentIndex().isValid()){
        current = 0;
    }
    else{
        current = ui->file_mark_tableView->currentIndex().row();
        current = current <= 0? 0 : (current-1);
    }

    ui->file_mark_tableView->selectRow(current);

    this->on_file_mark_tableView_clicked( ui->file_mark_tableView->model()->index(current,0) );
}

void Scene_File::on_file_mark_tableView_clicked(const QModelIndex &index)
{
    _mark = _mark_list.at(index.row()).toMap();

    //clip button is clicked
    //to avoid auto play or play position changed
    if(_player->stopPosition() <= _duration * _time_unit)
        return;

    int position = _mark.value("time").toInt();
    //qDebug()<<position;
    position -= _offset;
    //qDebug()<<position;

    if (!_player->isPlaying()){
        _player->play();
    }

    _player->setPosition(position * _time_unit);

    _player->pause( false );

    if (!ui->file_mark_tableView->hasFocus())
        ui->file_mark_tableView->setFocus();
}

void Scene_File::on_comboBox_currentIndexChanged(int index)
{
    _file = _file_list.at(index).toMap();

    _offset = _file.value("start_time").toInt();

    if(_offset < 0){
        _offset = 0;
    }

    QString path = _file.value("path").toString();

    qDebug()<<path;

    if(QFile(path).exists() && QFile(path).size() > 0){

        this->update_play_info( path );

        ui->file_mark_tableView->setEnabled(true);
        ui->stackedWidget->setEnabled(true);
    }else{

        ui->file_mark_tableView->setEnabled(false);
        ui->stackedWidget->setEnabled(false);

        QFileInfo fileinfo( _file.value("fullpath").toString() );
        QPoint pos = this->parentWidget()->parentWidget()->parentWidget()->geometry().topLeft() + this->geometry().center();
        Scene_Record_Warning::ShowMessage(pos, QString("%1 文件丢失").arg(fileinfo.baseName()));

        if(_file.value("start_time").toInt() < 0){
            if(_type == 1){
                _sharedData->CheckConferenceFile(_conf);
            }
            else if(_type == 0){
                _sharedData->CheckPersonalFile(_conf);
            }
        }

        _player->stop();
    }
}

void Scene_File::clip_finished(QString output_file, int start_time)
{
    QFileInfo fileinfo( output_file );
    QVariantMap file;
    file.insert("path", fileinfo.absoluteFilePath());
    file.insert("uuid", _uuid);
    file.insert("identity", _file.value("identity").toString());
    file.insert("start_time", start_time);

    _sharedData->AddClipFile(file);

    _file_list = _sharedData->GetFileList( _uuid );

    this->update_file_list(_file_list);
}


void Scene_File::on_start_time_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    QTime start_time = QTime::fromString(ui->start_time->text(),"hh:mm:ss");
    QTime end_time = QTime::fromString(ui->end_time->text(),"hh:mm:ss");

    if( start_time.isValid() && end_time.isValid())    {

        ui->time_slider->set_handle_value(QTime(0,0,0,0).secsTo(start_time),
                                          QTime(0,0,0,0).secsTo(end_time));
    }
}

void Scene_File::on_end_time_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    QTime start_time = QTime::fromString(ui->start_time->text(),"hh:mm:ss");
    QTime end_time = QTime::fromString(ui->end_time->text(),"hh:mm:ss");

    if( start_time.isValid() && end_time.isValid())    {
    ui->time_slider->set_handle_value(QTime(0,0,0,0).secsTo(start_time),
                                      QTime(0,0,0,0).secsTo(end_time));
    }
}


void Scene_File::on_sortBtn_clicked(bool checked)
{
    if(checked)
        sortFilter_proxyModel->sort(2,Qt::AscendingOrder);
    else
        sortFilter_proxyModel->sort(2,Qt::DescendingOrder);
}

void Scene_File::on_typeComboBox_currentIndexChanged(int index)
{
    switch(index){
    /*
    case 0:
        sortFilter_proxyModel->setSourceModel( _sharedData->GetModel(ModelUpdater::AllConferenceModel) );
        break;
        */
    case 0:
        sortFilter_proxyModel->setSourceModel( _sharedData->GetModel(ModelUpdater::ConferenceModel) );
        _type = 1;
        break;
    case 1:
        sortFilter_proxyModel->setSourceModel( _sharedData->GetModel(ModelUpdater::PersonalModel) );
        _type = 0;
        break;
    default:
        break;
    }
    sortFilter_proxyModel->setFilterKeyColumn(2);
    sortFilter_proxyModel->sort(2,Qt::DescendingOrder);

    ui->sortBtn->setChecked(false);
}


void Scene_File::on_searchButton_clicked()
{
    sortFilter_proxyModel->setFilterKeyColumn(5);

    this->syntaxChange(ui->lineEdit->text(),QRegExp::FixedString);

    if (ui->lineEdit->text() == ""){
        filter_action4->setChecked(true);
        filter_action4->setIconVisibleInMenu(true);
    }else{
        int count = filter_menu->actions().count();
        for(int i = 0; i < count; i++){
            filter_menu->actions().at(i)->setChecked(false);
            filter_menu->actions().at(i)->setIconVisibleInMenu(false);
        }
    }
}
