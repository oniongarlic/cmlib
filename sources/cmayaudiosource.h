#ifndef CMAYAUDIOSOURCE_H
#define CMAYAUDIOSOURCE_H

#include <QObject>

#include "cmbaseaudiosource.h"

#include "ayemu.h"
#include "ayemu_vtxfile.h"

class CMAYAudioSource : public CMBaseAudioSource
{
    Q_OBJECT
    Q_CLASSINFO("mime", "audio/x-vtx")
    Q_CLASSINFO("extensions", "*.vtx")
public:
    CMAYAudioSource(QObject *parent=0);

    static QStringList extensions();

protected:
    qint64 generateData(qint64 maxlen);

private:
    ayemu_vtx_t *m_vtx;
    ayemu_ay_t m_ay;
    ayemu_ay_reg_frame_t m_regs;
    size_t m_aypos;
    size_t m_fsize;
    QByteArray m_frame;

    // QIODevice interface
public:
    bool open(OpenMode mode);
    void close();

    // QIODevice interface
public:
    bool reset();
};

#endif // CMAYAUDIOSOURCE_H
