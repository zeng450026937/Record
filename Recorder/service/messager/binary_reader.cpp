#include "binary_reader.h"
#include <QBuffer>
#include <QDebug>

BinaryReader::BinaryReader()
{
    type = 0;
    type_size = 4;
    sender = "";
    sender_size = 17;
    file_uuid = "";
    file_uuid_size = 32;
    meeting_time = 0;
    meeting_time_size = 4;
    file_startpos = 0;
    file_startpos_size = 4;
    file_status = 0;
    file_status_size = 4;
    data_size = 0;
    data_size_size = 4;

    total_size = type_size + sender_size + file_uuid_size +
                 meeting_time_size + file_startpos_size +
                 file_status_size + data_size_size;
}

void BinaryReader::Read(QByteArray& binary)
{
    QBuffer buffer(&binary);
    buffer.open(QIODevice::ReadOnly);

    buffer.seek(0);

    bool ok(false);

    type = buffer.read(type_size).toHex().toUInt(&ok,16);
    //qDebug()<<"type"<<type<<ok;

    sender = QString::fromLatin1(buffer.read(sender_size));
    //qDebug()<<"sender"<<sender;

    file_uuid = QString::fromLatin1(buffer.read(file_uuid_size));
    //qDebug()<<"file_uuid"<<file_uuid;

    meeting_time = buffer.read(meeting_time_size).toHex().toUInt(&ok,16);
    //qDebug()<<"meeting_time"<<meeting_time<<ok;

    file_startpos = buffer.read(file_startpos_size).toHex().toULong(&ok,16);
    //qDebug()<<"file_startpos"<<file_startpos<<ok;

    file_status = buffer.read(file_status_size).toHex().toULong(&ok,16);
    //qDebug()<<"file_status"<<file_status<<ok;

    data_size = buffer.read(data_size_size).toHex().toULong(&ok,16);
    //qDebug()<<"data_size"<<data_size<<ok;

    if(data_size >= 0){
        data = buffer.read(data_size);
    }

    if(file_status == 1){
        qDebug()<<"file"\
                <<"conference:"<<file_uuid<<"device:"<<sender\
                <<"download completed.";
    }
}
