#include "tea.h"
#include <QString>
#include <QDebug>

Tea::Tea()
{
    T = 0;
    C = 0x9E3779B9;
    R = 64;
    K[0]=K[1]=K[2]=K[3]=0;
}

QString  Tea::name() { return QString("Tea"); }

QString Tea::author() { return QString("D.J.Wheeler");}

int Tea::keyLength() { return 128; }

int Tea::blockLength() { return 64; }

int Tea::speedLevel() { return 8; }

int Tea::safetyLevel() { return 4; }

int Tea::types() {return static_cast<int>(JobTypes::ECB_ENC|JobTypes::ECB_DEC);}

bool Tea::init(JobTypes::JobType t, QString k,QString s)
{
    qDebug()<<"[Tea say:] Start plugin init";

    qDebug()<<"[Tea say:] Set type of plugin";
    // set type
    this->type = t;

    // if key is empty return
    if (k.isEmpty()) return false;

    qDebug()<<"[Tea say:] Set key of plugin";
    // fill key
    // reuse k if it is small
    for(quint8 i=0,j=0; i<4; i++)
    {
        K[i]=k.at(j++).unicode()<<16;

        if (j<k.count()) K[i] |= k.at(j++).unicode();
        else
        {
            j=0;
            K[i] |= k.at(j++).unicode();
        }
    }

    qDebug()<<"[Tea say:] Set special args of plugin";
    // remove all spaces
    s.remove(QChar(' '));

    qint16 ind=0;

    bool ok[3]={true,true,true};
    // parse
    while ((ind=s.indexOf(QChar(' ')))!=-1)
    {
        QString temp = s.left(ind+1);

        if (temp.left(2)=="C=") this->C=temp.remove(0,2).toInt(&ok[0]);
        if (temp.left(2)=="T=") this->T=temp.remove(0,2).toInt(&ok[1]);
        if (temp.left(2)=="R=") this->R=temp.remove(0,2).toInt(&ok[2]);
    }

    qDebug()<<"[Tea say:] End plugin init";

    return (ok[0] && ok[1] && ok[2]);
}

void Tea::encode (char *in, char* out)
{
    quint32 A=0,B=0;

    for(quint8 i=0; i<4; i++) A = A<<8 | static_cast<quint8>(in[i]);
    for(quint8 i=0; i<4; i++) B = B<<8 | static_cast<quint8>(in[i+4]);

    T=0; // DANGER!!!

    for(quint8 i=0; i<R/2; i++)
    {
        T+=C;
        A+=( (B + T) XOR ((B<<4) + K[0]) XOR ((B>>5) + K[1]) );
        B+=( (A + T) XOR ((A<<4) + K[2]) XOR ((A>>5) + K[3]) );
    }

    out[0] = static_cast<char> ((A>>24) & 0xFF);
    out[1] = static_cast<char> ((A>>16) & 0xFF);
    out[2] = static_cast<char> ((A>> 8) & 0xFF);
    out[3] = static_cast<char> ((A    ) & 0xFF);

    out[4] = static_cast<char> ((B>>24) & 0xFF);
    out[5] = static_cast<char> ((B>>16) & 0xFF);
    out[6] = static_cast<char> ((B>> 8) & 0xFF);
    out[7] = static_cast<char> ((B    ) & 0xFF);

}

void Tea::decode (char *in, char* out)
{
    quint32 A=0,B=0;

    for(quint8 i=0; i<4; i++) A = A<<8 | static_cast<quint8>(in[i]);
    for(quint8 i=0; i<4; i++) B = B<<8 | static_cast<quint8>(in[i+4]);

    T+= C MUL32 (R/2);

    for(quint8 i=0; i<R/2; i++)
    {
        B-=( (A+T) XOR ((A<<4) + K[2]) XOR ((A>>5)+K[3]) );
        A-=( (B+T) XOR ((B<<4) + K[0]) XOR ((B>>5)+K[1]) );
        T-=C;
    }

    out[0] = static_cast<char> ((A>>24) & 0xFF);
    out[1] = static_cast<char> ((A>>16) & 0xFF);
    out[2] = static_cast<char> ((A>> 8) & 0xFF);
    out[3] = static_cast<char> ((A    ) & 0xFF);

    out[4] = static_cast<char> ((B>>24) & 0xFF);
    out[5] = static_cast<char> ((B>>16) & 0xFF);
    out[6] = static_cast<char> ((B>> 8) & 0xFF);
    out[7] = static_cast<char> ((B    ) & 0xFF);
}


PluginInterface* Tea::clone()
{
    return new Tea();
}
