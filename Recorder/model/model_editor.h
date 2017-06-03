#ifndef MODEL_EDITOR_H
#define MODEL_EDITOR_H

#include <QStringList>
#include <QVariant>
#include <QStandardItem>

class Model_TextDelegate;

class Model_Editor
{
public:
    Model_Editor(Model_TextDelegate *textDelegate);
    ~Model_Editor();

    void make_model_from_list(QStandardItemModel* model, QVariantList &list);
    QSharedPointer<QStandardItemModel> make_model_from_list(QVariantList &list);

    void reset_model_row(QStandardItemModel* model, int row, QVariantMap &info);
    void update_model_row(QStandardItemModel* model, int row, QVariantMap &info);
    void insert_model_row(QStandardItemModel* model, int row, QVariantMap &info);
    void append_model_row(QStandardItemModel* model, QVariantMap &info);
    void delete_model_row(QStandardItemModel* model, int row);

    //外部释放
    QList<QStandardItem *>  get_item_list_from_list(int row, QVariantList &list);
    //外部释放
    QList<QStandardItem *>  get_item_list_from_map(QVariantMap &info);
    //内部释放
    QList<QStandardItem *>  get_item_list_from_model(int row, QStandardItemModel* model);

private:
    Model_TextDelegate *_textDelegate;


};

#endif // MODEL_EDITOR_H
