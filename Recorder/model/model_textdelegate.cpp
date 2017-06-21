#include "model_textdelegate.h"
#include <QDebug>
#include <QTime>

Template_TextDelegate::Template_TextDelegate(QStringList header)
    : _header(header) {}

QStringList Template_TextDelegate::get_header() { return _header; }
int Template_TextDelegate::get_header_count() { return _header.count(); }
void Template_TextDelegate::set_text(QList<QStandardItem *> list,
                                     QVariantMap info) {
  QString text;

  for (int j = 0; j < _header.count() - 2; j++) {
    text.clear();

    QStandardItem *item = list.at(j);
    item->setTextAlignment(Qt::AlignCenter);

    if (_header.at(j) == "members") {
      text.append("参会人员:");
      // item->setTextAlignment(Qt::AlignVCenter);
    }
    text.append(info.value(_header.at(j)).toString());

    item->setText(text);

    if (j == 0) {
      item->setData(info, Qt::UserRole);
    }
  }
}

Device_TextDelegate::Device_TextDelegate(QStringList header)
    : _header(header) {}

QStringList Device_TextDelegate::get_header() { return _header; }
int Device_TextDelegate::get_header_count() { return _header.count(); }
void Device_TextDelegate::set_text(QList<QStandardItem *> list,
                                   QVariantMap info) {
  QString text;

  bool select_flag = info.value("selected").toBool();
  bool use_flag = info.value("used").toBool();

  for (int j = 0; j < _header.count(); j++) {
    text.clear();

    QStandardItem *item = list.at(j);

    item->setTextAlignment(Qt::AlignCenter);

    if (_header.at(j) == "selected") {
      item->setText("");
      item->setCheckable(true);
      item->setData(info, Qt::UserRole);
      item->setFlags((Qt::ItemFlag)(item->flags() | ~Qt::ItemIsUserCheckable |
                                    ~Qt::ItemIsUserTristate));

      if (select_flag) {
        item->setCheckState(Qt::Checked);
      } else {
        item->setCheckState(Qt::Unchecked);
      }
    } else if (_header.at(j) == "used") {
      item->setText("");
      item->setCheckable(true);
      item->setData(info, Qt::UserRole);
      item->setFlags((Qt::ItemFlag)(item->flags() | ~Qt::ItemIsUserCheckable |
                                    ~Qt::ItemIsUserTristate));

      if (use_flag) {
        item->setCheckState(Qt::Checked);
      } else {
        item->setCheckState(Qt::Unchecked);
      }
    } else if (_header.at(j) == "name") {
      text = info.value(_header.at(j)).toString();
      item->setTextAlignment(Qt::AlignVCenter);
      item->setText(text);
    } else if (_header.at(j) == "battery") {
      text = info.value(_header.at(j)).toString() + "%";
      item->setText(text);
    } else if (_header.at(j) == "battery_time") {
      int seconds = info.value(_header.at(j)).toString().toInt() * 60;
      QScopedPointer<QTime> time(new QTime((seconds / 3600) % 60,
                                           (seconds / 60) % 60, seconds % 60,
                                           (seconds * 1000) % 1000));
      text.append(time->toString("h小时mm分钟"));
      item->setText(text);
    } else {
      text = info.value(_header.at(j)).toString();
      item->setText(text);
    }

    if (j == 0) {
      item->setData(info, Qt::UserRole);
    }
  }
}

Conference_TextDelegate::Conference_TextDelegate(QStringList header)
    : _header(header) {}

QStringList Conference_TextDelegate::get_header() { return _header; }
int Conference_TextDelegate::get_header_count() { return _header.count(); }
void Conference_TextDelegate::set_text(QList<QStandardItem *> list,
                                       QVariantMap info) {
  QString text;

  for (int j = 0; j < _header.count(); j++) {
    text.clear();

    QStandardItem *item = list.at(j);

    item->setTextAlignment(Qt::AlignCenter);

    if (_header.at(j) == "search") {
      text += info.value("title").toString();
      text += info.value("content").toString();
      text += info.value("members").toString();
      text += info.value("date").toString();
      text += info.value("time").toString();

      text += info.value("userId").toString();
      text += info.value("userName").toString();
      text += info.value("gpsAddress").toString();
      text += info.value("tag").toString();

      item->setText(text);
    } else {
      text = info.value(_header.at(j)).toString();
      item->setText(text);
    }

    if (j == 0) {
      item->setData(info, Qt::UserRole);
      item->setText(" ");
    }
  }
}

Mark_TextDelegate::Mark_TextDelegate(QStringList header) : _header(header) {}

QStringList Mark_TextDelegate::get_header() { return _header; }
int Mark_TextDelegate::get_header_count() { return _header.count(); }
void Mark_TextDelegate::set_text(QList<QStandardItem *> list,
                                 QVariantMap info) {
  QString text;

  for (int j = 0; j < _header.count(); j++) {
    text.clear();

    QStandardItem *item = list.at(j);

    item->setTextAlignment(Qt::AlignCenter);

    if (_header.at(j) == "time") {
      int seconds = info.value(_header.at(j)).toString().toInt();
      QScopedPointer<QTime> time(new QTime((seconds / 3600) % 60,
                                           (seconds / 60) % 60, seconds % 60,
                                           (seconds * 1000) % 1000));
      text = time->toString("hh:mm:ss");
      item->setText(text);

      item->setData(info, Qt::UserRole);  // for sorting
    } else {
      text = info.value(_header.at(j)).toString();
      item->setText(text);
    }

    if (j == 0) {
      item->setData(info, Qt::UserRole);
    }
  }
}
