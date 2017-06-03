#ifndef BINARYREADER_H
#define BINARYREADER_H

#include <QByteArray>
#include <QString>

class BinaryReader
{
public:
    BinaryReader();

    void Read(QByteArray& binary);

    int type;
    int type_size;
    QString sender;
    int sender_size;
    QString file_uuid;
    int file_uuid_size;
    int meeting_time;
    int meeting_time_size;
    int file_startpos;
    int file_startpos_size;
    int file_status;
    int file_status_size;
    int data_size;
    int data_size_size;

    int total_size;

    QByteArray data;
};

#endif // BINARYREADER_H
