#ifndef MODULEUPDATER_H
#define MODULEUPDATER_H

#include <QString>
#include <QVariant>

class QStandardItemModel;
class Model_Editor;

class ModelUpdater
{
public:
    enum ModelType{
        DeviceModel = 0,
        ConferenceModel = 1,
        MarkModel = 2,
        TemplateModel = 3,
        RecordMarkModel = 4,
        PersonalModel = 5,
        AllConferenceModel = 6
    };

    QStandardItemModel* GetModel(ModelType type);
    int             ListToModel(ModelType type, QVariantList& list);
    int             ClearModel(ModelType type);

protected:
    int             UpdateRow(ModelType type, int row, QVariantMap& info);
    int             AppendRow(ModelType type, QVariantMap& info);
    int             AppendList(ModelType type, QVariantList& list);
    int             RemoveRow(ModelType type, int row);
    int             InsertRow(ModelType type, int row, QVariantMap& info);
    int             RowCount(ModelType type);

protected:
    ModelUpdater();
    ~ModelUpdater();

    Model_Editor*   GetEditor(ModelType type);

    QStringList         _device_header;
    QStringList         _conference_header;
    QStringList         _mark_header;
    QStringList         _template_header;
    QStringList         _personal_header;

    QStandardItemModel* _device_model;
    QStandardItemModel* _conference_model;
    QStandardItemModel* _mark_model;
    QStandardItemModel* _template_model;
    QStandardItemModel* _record_mark_model;
    QStandardItemModel* _personal_model;
    QStandardItemModel* _all_conference_model;

    Model_Editor*       _device;
    Model_Editor*       _conference;
    Model_Editor*       _mark;
    Model_Editor*       _template;
    Model_Editor*       _personal;
};

#endif // MODULEUPDATER_H
