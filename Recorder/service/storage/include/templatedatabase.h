#ifndef TEMPLATEDATABASE_H
#define TEMPLATEDATABASE_H

#include <QVariant>

class DataBase;

class TemplateDatabase {
public:
    static TemplateDatabase* GetInterface(DataBase* db);
    virtual int Release() = 0;

    virtual int AddTemplate(QVariantMap& Template) = 0;
    virtual QVariantMap TemplateInfo(QString uuid) = 0;
    virtual QVariantMap TemplateInfo(int row) = 0;

    virtual QVariantList TemplateList() = 0;

    virtual int TemplateCount() = 0;
    virtual int TemplateCount(QString uuid) = 0;

    virtual int DelTemplate(QString uuid) = 0;
    virtual int DelTemplate(int row) = 0;

    virtual int TemplateRow(QString uuid) = 0;

    virtual int SetTemplateTitle(QString uuid, QString title) = 0;
    virtual QString TemplateTitle(QString uuid) = 0;

    virtual int SetTemplateContent(QString uuid, QString content) = 0;
    virtual QString TemplateContent(QString uuid) = 0;

    virtual int SetTemplateMember(QString uuid, QString member) = 0;
    virtual QString TemplateMember(QString uuid) = 0;

protected:
    TemplateDatabase(){}
    virtual ~TemplateDatabase(){}

};

#endif // TEMPLATEDATABASE_H
