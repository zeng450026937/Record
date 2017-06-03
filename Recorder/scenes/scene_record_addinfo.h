#ifndef SCENE_RECORD_ADDINFO_H
#define SCENE_RECORD_ADDINFO_H

#include <QDialog>
#include <QtGui>

namespace Ui {
class Scene_Record_AddInfo;
}

class Scene_Record_AddInfo : public QDialog
{
    Q_OBJECT

public:
    explicit Scene_Record_AddInfo(QWidget *parent = 0);
    ~Scene_Record_AddInfo();

    void setTemplateInfo(QVariantMap& info);
    QVariantMap getTemplateInfo();

signals:
    void add_conference_info(QVariant info);

private slots:
    void on_confirm_btn_clicked();
    void on_cancel_btn_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::Scene_Record_AddInfo *ui;
    QPoint  movePos;
    QVariantMap _info;
};

#endif // SCENE_RECORD_ADDINFO_H
