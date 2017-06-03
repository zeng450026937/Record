#ifndef MODELEDITORFACTORY_H
#define MODELEDITORFACTORY_H

#include <QStringList>
class Model_Editor;

class ModelEditorFactory
{
public:
    ModelEditorFactory();

    static Model_Editor* CreateDefaultME();
    static Model_Editor* CreateDeviceME(QStringList header);
    static Model_Editor* CreateConferenceME(QStringList header);
    static Model_Editor* CreateMarkME(QStringList header);
    static Model_Editor* CreateTemplateME(QStringList header);

};

#endif // MODELEDITORFACTORY_H
