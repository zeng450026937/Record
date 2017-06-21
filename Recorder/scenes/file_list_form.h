#ifndef FILE_LIST_FORM_H
#define FILE_LIST_FORM_H

#include <QWidget>
class QSortFilterProxyModel;

namespace Ui {
class FileListForm;
}

class FileListForm : public QWidget {
    Q_OBJECT

public:
    explicit FileListForm(QWidget* parent = 0);
    ~FileListForm();

private:
    Ui::FileListForm* ui;
    QSortFilterProxyModel* proxyModel;
};

#endif // FILE_LIST_FORM_H
