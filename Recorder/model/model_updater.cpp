#include "model_updater.h"
#include <QStandardItemModel>
#include "model_editor.h"
#include "model_editorfactory.h"

QStandardItemModel* ModelUpdater::GetModel(ModelType type) {
  QStandardItemModel* model;

  switch (type) {
    case DeviceModel:

      model = _device_model;
      break;
    case ConferenceRecordInfoModel:

      model = _conference_model;
      break;
    case MarkModel:

      model = _mark_model;
      break;
    case TemplateModel:

      model = _template_model;
      break;

    case RecordMarkModel:

      model = _record_mark_model;
      break;
    default:
      return NULL;
  }

  return model;
}

int ModelUpdater::ListToModel(ModelType type, QVariantList& list) {
  Model_Editor* editor = GetEditor(type);
  QStandardItemModel* model = GetModel(type);
  if (!editor || !model) return -1;

  editor->make_model_from_list(model, list);

  return 0;
}
int ModelUpdater::ClearModel(ModelType type) {
  Model_Editor* editor(NULL);
  QStandardItemModel* model(NULL);

  editor = GetEditor(type);
  model = GetModel(type);

  if (!editor || !model) return -1;

  QVariantList null;

  editor->make_model_from_list(model, null);

  return 0;
}

int ModelUpdater::UpdateRow(ModelType type, int row, QVariantMap& info) {
  Model_Editor* editor(NULL);
  QStandardItemModel* model(NULL);

  editor = GetEditor(type);
  model = GetModel(type);

  if (!editor || !model) return -1;

  editor->update_model_row(model, row, info);

  return 0;
}
int ModelUpdater::AppendRow(ModelType type, QVariantMap& info) {
  Model_Editor* editor(NULL);
  QStandardItemModel* model(NULL);

  editor = GetEditor(type);
  model = GetModel(type);

  if (!editor || !model) return -1;

  editor->append_model_row(model, info);

  return 0;
}

int ModelUpdater::AppendList(ModelType type, QVariantList& list) {
  Model_Editor* editor(NULL);
  QStandardItemModel* model(NULL);

  editor = GetEditor(type);
  model = GetModel(type);

  if (!editor || !model) return -1;

  foreach (QVariant info, list) {
    editor->append_model_row(model, info.toMap());
  }

  return 0;
}
int ModelUpdater::RemoveRow(ModelType type, int row) {
  Model_Editor* editor(NULL);
  QStandardItemModel* model(NULL);

  editor = GetEditor(type);
  model = GetModel(type);

  if (!editor || !model) return -1;

  editor->delete_model_row(model, row);

  return 0;
}
int ModelUpdater::InsertRow(ModelType type, int row, QVariantMap& info) {
  Model_Editor* editor(NULL);
  QStandardItemModel* model(NULL);

  editor = GetEditor(type);
  model = GetModel(type);

  if (!editor || !model) return -1;

  editor->insert_model_row(model, row, info);

  return 0;
}

int ModelUpdater::RowCount(ModelType type) {
  int count(0);

  QStandardItemModel* model(NULL);
  model = GetModel(type);

  if (!model) return -1;

  count = model->rowCount();

  return count;
}

Model_Editor* ModelUpdater::GetEditor(ModelType type) {
  Model_Editor* editor(NULL);

  switch (type) {
    case DeviceModel:

      editor = _device;
      break;
    //     case PersonalModel:
    //
    //         editor = _personal;
    //         break;
    //     case AllConferenceModel:
    case ConferenceRecordInfoModel:

      editor = _conference;
      break;
    case RecordMarkModel:
    case MarkModel:

      editor = _mark;
      break;
    case TemplateModel:

      editor = _template;
      break;
    default:
      return NULL;
  }

  return editor;
}

ModelUpdater::ModelUpdater()
    : _device_model(new QStandardItemModel()),
      _conference_model(new QStandardItemModel()),
      _mark_model(new QStandardItemModel()),
      _template_model(new QStandardItemModel()),
      _record_mark_model(new QStandardItemModel()) {
  _device_header << "display";
  _device_header << "used";
  _device_header << "selected";
  _device_header << "name";
  _device_header << "batteryPercent";
  _device_header << "batteryTime";
  _device_header << "status";

  _device_model->setHorizontalHeaderLabels(_device_header);

  _conference_header << "display";
  _conference_header << "userId";
  _conference_header << "userName";
  _conference_header << "date";
  _conference_header << "time";
  _conference_header << "title";
  _conference_header << "recordType";
  _conference_header << "search";
  _conference_header << "createTime";
  _conference_header << "completed";

  _conference_model->setHorizontalHeaderLabels(_conference_header);

  _personal_header = _conference_header;
  // _personal_model->setHorizontalHeaderLabels(_personal_header);

  _mark_header << "index";
  _mark_header << "mark";
  _mark_header << "time";
  _mark_header << "clip";

  _mark_model->setHorizontalHeaderLabels(_mark_header);
  _record_mark_model->setHorizontalHeaderLabels(_mark_header);

  _template_header << "display";
  _template_header << "title";
  _template_header << "members";
  _template_header << "edit";
  _template_header << "delete";

  _template_model->setHorizontalHeaderLabels(_template_header);

  _device = ModelEditorFactory::CreateDeviceME(_device_header);
  _conference = ModelEditorFactory::CreateConferenceME(_conference_header);
  _mark = ModelEditorFactory::CreateMarkME(_mark_header);
  _template = ModelEditorFactory::CreateTemplateME(_template_header);
  _personal = ModelEditorFactory::CreateConferenceME(_personal_header);
}
ModelUpdater::~ModelUpdater() {
  delete _device;
  delete _conference;
  delete _mark;
  delete _template;
  delete _personal;

  delete _device_model;
  delete _conference_model;
  delete _mark_model;
  delete _template_model;
}
