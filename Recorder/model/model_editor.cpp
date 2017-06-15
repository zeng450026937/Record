#include "model_editor.h"
#include "model_textdelegate.h"

Model_Editor::Model_Editor(Model_TextDelegate *textDelegate) :
    _textDelegate(textDelegate)
{
}

Model_Editor::~Model_Editor()
{
    if(_textDelegate){
        delete _textDelegate;
    }
}

void Model_Editor::make_model_from_list(QStandardItemModel* model, QVariantList &list)
{
    if(model == nullptr)
        return;

    model->removeRows(0,model->rowCount());

    if(list.count() <= 0)
        return;

    QList<QStandardItem *> item_list;

    for(int i = 0; i < list.count();i++){

        item_list.clear();
        item_list = this->get_item_list_from_list(i, list);
        _textDelegate->set_text(item_list,list.at(i).toMap());

        model->appendRow(item_list);
    }

    return;
}

QSharedPointer<QStandardItemModel> Model_Editor::make_model_from_list(QVariantList &list)
{
    QSharedPointer<QStandardItemModel> model(new QStandardItemModel());

    if(list.count() <= 0)
        return model;

    QList<QStandardItem *> item_list;

    for(int i = 0; i < list.count();i++){

        item_list.clear();
        item_list = this->get_item_list_from_list(i, list);
        _textDelegate->set_text(item_list,list.at(i).toMap());

        model->appendRow(item_list);
    }

    return model;
}

void Model_Editor::reset_model_row(QStandardItemModel* model, int row, QVariantMap &info)
{
    if(row < model->rowCount() && row >= 0){

        QList<QStandardItem *> item_list = this->get_item_list_from_map(info);
        _textDelegate->set_text(item_list,info);

        if(row <= model->rowCount()){
            model->removeRow(row);
            model->insertRow(row,item_list);
        }
    }
}
void Model_Editor::update_model_row(QStandardItemModel* model, int row, QVariantMap &info)
{
    if(row < model->rowCount() && row >= 0)
    {
        QList<QStandardItem *> item_list = this->get_item_list_from_model(row, model);
        _textDelegate->set_text(item_list,info);
    }
}
void Model_Editor::insert_model_row(QStandardItemModel* model, int row, QVariantMap &info)
{
    if(row < model->rowCount() && row >= 0){

        QList<QStandardItem *> item_list = this->get_item_list_from_map(info);
        _textDelegate->set_text(item_list,info);

        model->insertRow(row,item_list);
    }
}
void Model_Editor::append_model_row(QStandardItemModel* model, QVariantMap &info)
{
    QList<QStandardItem *> item_list = this->get_item_list_from_map(info);
    _textDelegate->set_text(item_list,info);
    model->appendRow(item_list);
}
void Model_Editor::delete_model_row(QStandardItemModel* model, int row)
{
    model->removeRow(row);
}


QList<QStandardItem *>  Model_Editor::get_item_list_from_list(int row, QVariantList &list)
{
    QList<QStandardItem *> item_list;

    QVariantMap info_map = list.at(row).toMap();

    int count = _textDelegate->get_header_count();

    if(count == 0){
        count = info_map.size();
    }

    for(int j = 0 ;j < count; j++){

        QStandardItem *item = new QStandardItem(info_map.value(_textDelegate->get_header().at(j)).toString());

        item_list.append(item);
    }

    return item_list;
}
QList<QStandardItem *>  Model_Editor::get_item_list_from_map(QVariantMap &info)
{
    QList<QStandardItem *> item_list;

    QVariantMap info_map = info;

    int count = _textDelegate->get_header_count();

    if(count == 0){
        count = info_map.size();
    }

    for(int j = 0;j < count;j++){

        QStandardItem *item = new QStandardItem(info_map.value(_textDelegate->get_header().at(j)).toString());

        item_list.append(item);
    }

    return item_list;
}
QList<QStandardItem *>  Model_Editor::get_item_list_from_model(int row, QStandardItemModel* model)
{
    QList<QStandardItem *> item_list;

    for(int i = 0; i < model->columnCount(); i++){
        item_list.append(model->item(row,i));
    }

    return item_list;
}
