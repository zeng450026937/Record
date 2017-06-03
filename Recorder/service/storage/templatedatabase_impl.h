#ifndef TEMPLATEDATABASE_IMPL_H
#define TEMPLATEDATABASE_IMPL_H

#include "include/templatedatabase.h"

class SharedData;

class TemplateDatabase_Impl : public TemplateDatabase
{
public:
    int AddTemplate(QVariantMap& Template) override;
    QVariantMap TemplateInfo(QString uuid) override;
    QVariantMap TemplateInfo(int row) override;

    QVariantList TemplateList() override;

    int TemplateCount() override; 
    int TemplateCount(QString uuid) override;

    int DelTemplate(QString uuid) override;
    int DelTemplate(int row) override;

    int TemplateRow(QString uuid) override;

    int SetTemplateTitle(QString uuid, QString title) override;
    QString TemplateTitle(QString uuid) override;

    int SetTemplateContent(QString uuid, QString content) override;
    QString TemplateContent(QString uuid) override;

    int SetTemplateMember(QString uuid, QString member) override;
    QString TemplateMember(QString uuid) override;

protected:
    TemplateDatabase_Impl(SharedData* shared):_shared(NULL){ _shared = shared; }
    ~TemplateDatabase_Impl() override {}

private:
    SharedData* _shared;
};

#endif // TEMPLATEDATABASE_IMPL_H
