#include "file_list_form.h"
#include <QSortFilterProxyModel>
#include "common/config.h"
#include "conf_detail.h"
#include "listitem_delegate.h"
#include "ui_file_list_form.h"

FileListForm::FileListForm(QWidget* parent)
    : QWidget(parent), ui(new Ui::FileListForm), proxyModel(Q_NULLPTR) {
  ui->setupUi(this);

  Config* config = Config::GetInstance();
  QStringList combo_list = config->_type_combo;

  QListView* typeCombo_listview = new QListView(ui->typeComboBox);
  typeCombo_listview->setAutoScroll(true);
  ui->typeComboBox->setView(typeCombo_listview);
  ui->typeComboBox->setEditable(false);
  ui->typeComboBox->addItems(combo_list);

  ui->sortBtn->setChecked(false);

  ConfDetail* detailWidget = new ConfDetail(this);
  ui->stackedWidget->addWidget(detailWidget);

  ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->listView->setWrapping(false);
  ui->listView->setModel(proxyModel);

  ListItemDelegate* listDelegate = new ListItemDelegate(this);
  ui->listView->setItemDelegateForColumn(0, listDelegate);

  //    QObject::connect(listDelegate, &ListItemDelegate::itemClicked, [=](){
  //        ui->stackedWidget->setCurrentIndex(
  //        ui->stackedWidget->currentIndex()+1 );
  //    });

  QObject::connect(detailWidget, &ConfDetail::goBack, [=]() {
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex() - 1);
  });
}

FileListForm::~FileListForm() { delete ui; }
