#ifndef MODEL_TEXTDELEGATE_H
#define MODEL_TEXTDELEGATE_H

#include <QStringList>
#include <QVariant>
#include <QStandardItem>

class Model_TextDelegate
{
public:
    Model_TextDelegate(){}
    Model_TextDelegate(QStringList header){_header = header;}

    virtual QStringList get_header() {return _header;}

    virtual int get_header_count() {return _header.count();}

    virtual void set_text(QList<QStandardItem *> list, QVariantMap info) {
        Q_UNUSED(list);
        Q_UNUSED(info);
    }

private:
    QStringList _header;

};

class Template_TextDelegate : public Model_TextDelegate
{
public:
    Template_TextDelegate(QStringList header);

    QStringList get_header();
    int get_header_count();
    void set_text(QList<QStandardItem *> list, QVariantMap info);

private:
    QStringList _header;

};

class Device_TextDelegate : public Model_TextDelegate
{
public:
    Device_TextDelegate(QStringList header);

    QStringList get_header();
    int get_header_count();
    void set_text(QList<QStandardItem *> list, QVariantMap info);

private:
    QStringList _header;

};

class Conference_TextDelegate : public Model_TextDelegate
{
public:
    Conference_TextDelegate(QStringList header);

    QStringList get_header();
    int get_header_count();
    void set_text(QList<QStandardItem *> list, QVariantMap info);

private:
    QStringList _header;

};

class Mark_TextDelegate : public Model_TextDelegate
{
public:
    Mark_TextDelegate(QStringList header);

    QStringList get_header();
    int get_header_count();
    void set_text(QList<QStandardItem *> list, QVariantMap info);

private:
    QStringList _header;

};



#endif // MODEL_TEXTDELEGATE_H
