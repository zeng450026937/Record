#include "conf_detail.h"
#include <QDebug>
#include "conf_form.h"
#include "list_form.h"
#include "recorder_shared.h"
#include "service/command/ConferenceMode.h"
#include "service/conf_service_impl.h"
#include "service/service_thread.h"
#include "ui_conf_detail.h"

ConfDetail::ConfDetail(QWidget *parent)
    : QWidget(parent), ui(new Ui::ConfDetail), conf_mode(Q_NULLPTR) {
  ui->setupUi(this);

  QObject::connect(ui->backButton, &QPushButton::clicked, this,
                   &ConfDetail::goBack);

  conf_mode = ServiceThread::GetInstance()->GetConferenceMode();
  QObject::connect(
      conf_mode, &ConferenceMode::getConferenceFiles, this,
      [=](int type, const QVariantList &list) {

        ui->listWidget->clear();
        foreach (QVariant item, list) {
          QVariantMap itemMap = item.toMap();
          QString qstrCreateTime = itemMap["createTime"].toString();
          itemMap.insert(
              "date",
              qstrCreateTime.left(10));  // 10 == strlen("yyyy-MM-dd")
          itemMap.insert("time",
                         qstrCreateTime.right(8));  // 8 == strlen("hh:mm:ss")
          itemMap.insert("recordType", type);
          itemMap.insert("title", this->_info.value("title"));
          QListWidgetItem *listItem = new QListWidgetItem(
              item.toMap().value("uuid").toString(), ui->listWidget);
          ListForm *listItemWidget = new ListForm(true);
          listItem->setSizeHint(QSize(ui->listWidget->width() - 5,
                                      listItemWidget->size().height()));
          ui->listWidget->addItem(listItem);
          ui->listWidget->setItemWidget(listItem, listItemWidget);
          listItemWidget->update_display(itemMap);
          listItemWidget->CheckAndAliveData();

          QObject::connect(listItemWidget, &ListForm::itemClicked, this,
                           &ConfDetail::selectFile);
          qDebug() << item;
        }
      });
}

ConfDetail::~ConfDetail() { delete ui; }

void ConfDetail::setInfo(const QVariantMap &info) {
  _info = info;
  ui->listWidget->clear();
  QString title = _info.value("title").toString();

  QString qstrCreateTime = info["createTime"].toString();
  ui->dateLabel->setText(qstrCreateTime.left(10));
  ui->timeLabel->setText(qstrCreateTime.right(8));
  if (conf_mode) {
    switch (_info.value("recordType").toInt()) {
      case RecorderShared::RT_PERSONAL:

        break;
      case RecorderShared::RT_CONFERENCE:
        if (title.isEmpty()) title = tr("会议录音");
        conf_mode->GetConferenceFiles(_info.value("conferenceUuid").toString());
        break;
      case RecorderShared::RT_MOBILE:
        if (title.isEmpty()) title = tr("移动会议");
        conf_mode->GetMobileConferenceFiles(
            _info.value("conferenceUuid").toString());
        break;
      default:
        break;
    }
  }
  ui->titleLabel->setText(title);
}
