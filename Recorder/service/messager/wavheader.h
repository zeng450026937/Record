#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <QByteArray>

class WavHeader
{
public:
    WavHeader();

    static QByteArray MakeWavHeader(int sample_rate, long audio_len){
        QByteArray header;

    }

    enum{SAMPLE_RATE = 44100};
    enum{CHANNAL = 1};
};

#endif // WAVHEADER_H
