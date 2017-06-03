#include "model_editorfactory.h"
#include "model_editor.h"
#include "model_textdelegate.h"

ModelEditorFactory::ModelEditorFactory()
{

}

Model_Editor* ModelEditorFactory::CreateDefaultME()
{
    Model_Editor* me = new Model_Editor(new Model_TextDelegate());
    return me;
}
Model_Editor* ModelEditorFactory::CreateDeviceME(QStringList header)
{
    Model_Editor* me = new Model_Editor(new Device_TextDelegate(header));
    return me;
}
Model_Editor* ModelEditorFactory::CreateConferenceME(QStringList header)
{
    Model_Editor* me = new Model_Editor(new Conference_TextDelegate(header));
    return me;
}
Model_Editor* ModelEditorFactory::CreateMarkME(QStringList header)
{
    Model_Editor* me = new Model_Editor(new Mark_TextDelegate(header));
    return me;
}
Model_Editor* ModelEditorFactory::CreateTemplateME(QStringList header)
{
    Model_Editor* me = new Model_Editor(new Template_TextDelegate(header));
    return me;
}
