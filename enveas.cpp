/*
 * Copyright © 2021 Yaroslav Shkliar <mail@ilit.ru>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Research Laboratory of IT
 * www.ilit.ru on e-mail: mail@ilit.ru
 * Also you сould open support domain www.cleenair.ru or write to e-mail: mail@cleenair.ru
 */

#include "enveas.h"
#include <math.h>

enveas::enveas(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{

    m_sock = new QUdpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_sock, SIGNAL(bytesWritten(qint64)), this, SLOT(writes()));

    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataReady(QByteArray&)), this, SLOT(setData(QByteArray&)) );

    changeInterface(*ip, *port);
    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);

    measure = new  QMap<QString, float>;

    measure->insert("SO2", 0);
    measure->insert("H2S", 0);
    measure->insert("NO", 0);
    measure->insert("NO2", 0);
    measure->insert("NOx", 0);
    measure->insert("NH3", 0);
    measure->insert("PM1", 0);
    measure->insert("PM2.5", 0);
    measure->insert("PM4", 0);
    measure->insert("PM10", 0);
    measure->insert("PM", 0);

    is_read = false;
    status = UNKNOWN;
    connected = m_sock->state();

    qDebug() << "The Envea measure equipment handling has been initialized.";

}

enveas::enveas(QObject *parent, QString *ip, quint16 *port, QString *name, int __type) : QObject (parent)
{
    m_sock = new QUdpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_sock, SIGNAL(bytesWritten(qint64)), this, SLOT(writes()));

    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataReady(QByteArray&)), this, SLOT(setData(QByteArray&)) );

    changeInterface(*ip, *port);
    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);

    measure = new  QMap<QString, float>;
    sample_t = new QMap<QString, int>;

    is_read = false;
    status = UNKNOWN;
    connected = m_sock->state();

    model = *name;
    type = _type(__type);

    switch (type) {
    case SO2_H2S:

        measure ->insert("SO2", 0);
        measure->insert("H2S", 0);

        sample_t->insert("SO2", 0);
        sample_t->insert("H2S", 0);

        break;

    case SO2:
        measure ->insert("SO2", 0);
        sample_t->insert("SO2", 0);

        break;
    case H2S:
        measure->insert("H2S", 0);
        sample_t->insert("H2S", 0);

        break;
    case NOx_NH3:

        measure ->insert("NO", 0);
        measure->insert("NO2", 0);
        measure ->insert("NOx", 0);
        measure->insert("NH3", 0);

        sample_t->insert("NO", 0);
        sample_t->insert("NO2", 0);
        sample_t->insert("NOx", 0);
        sample_t->insert("NH3", 0);

        break;

    case NOx:

        measure ->insert("NO", 0);
        measure->insert("NO2", 0);
        measure ->insert("NOx", 0);

        sample_t->insert("NO", 0);
        sample_t->insert("NO2", 0);
        sample_t->insert("NOx", 0);
        break;

    case CO:

        measure ->insert("CO", 0);
        sample_t->insert("CO", 0);

        break;

    case PM:

        measure ->insert("PM", 0);
        measure ->insert("PM1", 0);
        measure->insert("PM2.5", 0);
        measure ->insert("PM4", 0);
        measure->insert("PM10", 0);

        sample_t ->insert("PM", 0);
        sample_t ->insert("PM1", 0);
        sample_t->insert("PM2.5", 0);
        sample_t ->insert("PM4", 0);
        sample_t->insert("PM10", 0);

        break;
    default:
        break;
    }
    qDebug() << "The Envea " << QVariant::fromValue(type).toString() <<" measure equipment handling has been initialized.";

}

enveas::~enveas()
{
    //sendData("MSTOP");
    //sendData("MSTOP");

    m_sock->disconnectFromHost();
    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();
}



void enveas::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void enveas::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void enveas::readData()
{
    QStringList list;
    int ind;
    float _measure = 0.0f;

    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray data = m_sock->readAll();
    switch (last_command) {
    case READMEASURE : {
        list = QString(QString(data).remove(QRegExp("[\r\n{\r\n}]"))).split(QRegExp(" "));
        ind = xRegExp.indexIn(list.at(0));

        if (type != NOx_NH3 ){
            if ( QString(data.mid(13)).indexOf("M") != -1)
                status = MEASURING;

            if ( QString(data.mid(13)).indexOf("P") != -1)
                status = INSTRUMENT_WARM_UP;

            if ( QString(data.mid(13)).indexOf("T") != -1)
                status = STANDBYORTEST;

            if ( QString(data.mid(13)).indexOf("R") != -1)
                status = ZEROREF;

            if ( QString(data.mid(13)).indexOf("Z") != -1)
                status = ZERO;

            if ( QString(data.mid(13)).indexOf("S") != -1)
                status = SPAN;

            if ( QString(data.mid(13)).indexOf("C") != -1)
                status = SPANAUTO;

            if ( QString(data.mid(13)).indexOf("N") != -1)
                status = INVALIDDATA;
        } else { //for NOx_NH3 measure should use another function and parcing procedure

            if ( uchar(data[11]) == 0x2)
                status = MEASURING;
            if ( uchar(data[12]) == 0x80)
                status = INSTRUMENT_WARM_UP;
            if ( uchar(data[12]) == 0x8)
                status = ZERO;
            if (uchar(data[12]) == 0x10)
                status = SPAN;
            if ( uchar(data[12]) == 0x40)
                status = INVALIDDATA;
        }

        if (status == MEASURING)
        {
            switch (type) {
            case SO2_H2S:
                measure->insert("SO2", list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("SO2") :  measure->value("SO2"));
                sample_t->insert("SO2", sample_t->value("SO2")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;

                measure->insert("H2S",  list.at(2).toFloat() > 0 ? list.at(2).toFloat() + measure->value("H2S") : measure->value("H2S"));
                sample_t->insert("H2S", sample_t->value("H2S")+1);
                _measure = list.at(2).toFloat() < 0 ? list.at(2).toFloat() : _measure;

                break;

            case SO2:
                measure->insert("SO2", list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("SO2") :  measure->value("SO2"));
                sample_t->insert("SO2", sample_t->value("SO2")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;

                break;
            case H2S:
                measure->insert("H2S", list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("H2S") :  measure->value("H2S"));
                sample_t->insert("H2S", sample_t->value("H2S")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;

                break;
            case NOx_NH3:{

                /* measure->insert("NO", list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("NO") :  measure->value("NO"));
                sample_t->insert("NO", sample_t->value("NO")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;


                measure->insert("NOx", list.at(2).toFloat() > 0 ? list.at(2).toFloat() + measure->value("NOx") :  measure->value("NOx"));
                sample_t->insert("NOx", sample_t->value("NOx")+1);
                _measure = list.at(2).toFloat() < 0 ? list.at(2).toFloat() : _measure;

                measure->insert("NO2", list.at(3).toFloat() > 0 ? list.at(3).toFloat() + measure->value("NO2") :  measure->value("NO2"));
                sample_t->insert("NO2", sample_t->value("NO2")+1);
                _measure = list.at(3).toFloat() < 0 ? list.at(3).toFloat() : _measure;


                measure->insert("NH3", list.at(4).toFloat() > 0 ? list.at(4).toFloat() + measure->value("NH3") :  measure->value("NH3"));
                sample_t->insert("NH3", sample_t->value("NH3")+1);
                _measure = list.at(4).toFloat() < 0 ? list.at(4).toFloat() : _measure;*/

                char sign;
                int expo;
                uint32_t mantissa;
                uint32_t ieee;
                int i, j, start;
                float result;
                int total_regs = 2;

                //parcing IEEE float point format
                if (data.length() == 44)  //data buffer detection for fullness
                {
                    start = 13;
                    total_regs =5;
                    data = data.mid(13);

                    for (j = 0; j < total_regs; j++) {



                        sign = data [6*j + 6-6] >> 7;
                        ieee = uint32_t((uchar(data [6*j + 6-6]) << 24 ) | (uchar(data [6*j + 7-6]) << 16) | (uchar(data[6*j + 8-6]) << 8) | uchar(data[6*j + 9-6]));
                        expo = (ieee & 0x7F800000) >> 23;
                        expo -= 127;

                        mantissa = (ieee & 0x7FFFFF);
                        float dec = 1.0f;
                        for(i=0; i<=22; i++) {
                            if(((mantissa >> (22 - i)) & 1) != 0) {
                                dec += float(pow(2, -i-1));
                            }
                        }

                        result = float( pow(2,expo));

                        if(sign)
                            result = -1*result * dec;
                        else
                            result = result * dec;

                        //data fill according sequence
                        switch (j) {
                        case 0:
                            if (result >= 0) { //negative value detection
                                sample_t->insert("NO", sample_t->value("NO") + 1);
                                measure->insert("NO",  measure->value("NO") + result);
                            }
                            else
                            {
                                sample_t->insert("NO", sample_t->value("NO") + 1);
                                measure->insert("NO",  measure->value("NO") + 0.0f);
                            }
                            _measure = result;
                            break;
                        case 1:
                            if (result >= 0) {//negative value detection
                                sample_t->insert("NOx", sample_t->value("NOx") + 1);
                                measure->insert("NOx", measure->value("NOx") + result);
                            }
                            else
                            {
                                sample_t->insert("NOx", sample_t->value("NOx") + 1);
                                measure->insert("NOx", measure->value("NOx") + 0.0f);
                            }
                            _measure = result;
                            break;
                        case 2:
                            if (result >= 0) {//negative value detection
                                sample_t->insert("NO2", sample_t->value("NO2") + 1);
                                measure->insert("NO2", measure->value("NO2") + result);
                            }
                            else
                            {
                                sample_t->insert("NO2", sample_t->value("NO2") + 1);
                                measure->insert("NO2", measure->value("NO2") + 0.00000000f);
                            }
                            _measure = result;
                            break;
                        case 3:
                            if (result >= 0) {//negative value detection
                                sample_t->insert("Ny", sample_t->value("Ny") + 1);
                                measure->insert("Ny", measure->value("Ny") + result);
                            }
                            else
                            {
                                sample_t->insert("Ny", sample_t->value("Ny") + 1);
                                measure->insert("Ny", measure->value("Ny") + 0.00000000f);
                            }
                            _measure= result;
                            break;
                        case 4:
                            if (result >= 0) {//negative value detection
                                sample_t->insert("NH3", sample_t->value("NH3") + 1);
                                measure->insert("NH3", measure->value("NH3") + result);
                            }
                            else
                            {
                                sample_t->insert("NH3", sample_t->value("NH3") + 1);
                                measure->insert("NH3", measure->value("NH3") + 0.00000000f);
                            }
                            _measure = result;
                            break;
                        default: break;
                        }
                    }
                }
            }

                break;

            case NOx:
                measure->insert("NO", list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("NO") :  measure->value("NO"));
                sample_t->insert("NO", sample_t->value("NO")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;


                measure->insert("NO2", list.at(3).toFloat() > 0 ? list.at(3).toFloat() + measure->value("NO2") :  measure->value("NO2"));
                sample_t->insert("NO2", sample_t->value("NO2")+1);
                _measure = list.at(3).toFloat() < 0 ? list.at(3).toFloat() : _measure;

                measure->insert("NOx", list.at(2).toFloat() > 0 ? list.at(2).toFloat() + measure->value("NOx") :  measure->value("NOx"));
                sample_t->insert("NOx", sample_t->value("NOx")+1);
                _measure = list.at(2).toFloat() < 0 ? list.at(2).toFloat() : _measure;

                break;

            case CO:

                measure->insert("CO",  list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("CO") :  measure->value("CO"));
                sample_t->insert("CO", sample_t->value("CO")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;

                break;

            case PM:

                measure->insert("PM1",   list.at(1).toFloat() > 0 ? list.at(1).toFloat() + measure->value("PM1") :  measure->value("PM1"));
                sample_t->insert("PM1", sample_t->value("PM1")+1);
                _measure = list.at(1).toFloat() < 0 ? list.at(1).toFloat() : _measure;

                measure->insert("PM2.5",   list.at(2).toFloat() > 0 ? list.at(2).toFloat() + measure->value("PM2.5") :  measure->value("PM2.5"));
                sample_t->insert("PM2.5", sample_t->value("PM2.5")+1);
                _measure = list.at(2).toFloat() < 0 ? list.at(2).toFloat() : _measure;

                measure->insert("PM10",  list.at(3).toFloat() > 0 ? list.at(3).toFloat() + measure->value("PM10") :  measure->value("PM10"));
                sample_t->insert("PM10", sample_t->value("PM10")+1);
                _measure = list.at(3).toFloat() < 0 ? list.at(3).toFloat() : _measure;

                measure->insert("PM",   list.at(4).toFloat() > 0 ? list.at(4).toFloat() + measure->value("PM") :  measure->value("PM"));
                sample_t->insert("PM", sample_t->value("PM")+1);
                _measure = list.at(4).toFloat() < 0 ? list.at(4).toFloat() : _measure;


                break;

            default:
                break;
            }

        }
    }
        break;

    default: break;
    }

    if (((status == INVALIDDATA) || (status == MEASURING) ) && (last_command != ZEROREFERENCE) && (_measure < 0))
        zero_ref();

    if (verbose){
        if (type == NOx_NH3)
        {
            qDebug() << "\n\rENVEA parced response: NO = "<<measure->value("NO") << " NOx = "<< measure->value("NOx")<< " NO2 = " << measure->value("NO2")<< " Ny = "<< measure->value("Ny") << " NH3 = "<< measure->value("NH3") << " \n\r Status: " << QVariant::fromValue(status).toString() << " \n\r";

        }
        else {
            qDebug() << "ENVEA "<< QVariant::fromValue(type).toString() << "measure data: " << data << " \n\r Status: " << QVariant::fromValue(status).toString() << " \n\r";
        }
    }

    this->is_read = true;

    QString _status_local = QVariant::fromValue(status).toString();
    emit dataIsReady(&is_read, measure, sample_t, &_status_local);

    blockSize = 0;

}

void enveas::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<   "!!! ENVEA measure " << QVariant::fromValue(type).toString() << " equipment handling error: The host was not found. Please check the "
                                                                                     "host name and port settings.\n\r";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< "!!! ENVEA measure equipment "<< QVariant::fromValue(type).toString() << " handling error: The connection was refused by the peer. "
                                                                                            "Make sure the fortune server is running, "
                                                                                            "and check that the host name and port "
                                                                                            "settings are correct.\n\r";
        break;
    default:
        qDebug()<< "!!! ENVEA measure equipment " << QVariant::fromValue(type).toString() << " handling error: " << (m_sock->errorString())<<"\n\r";
    }

    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();

}

void enveas::readGases(int qw)
{

    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }
    ba[i] = 49; //instantaneous measure with floating point - 16 command
    ba[i+1] = 54;
    sendData(1, &ba);

    last_command = READMEASURE;

}

void enveas::readGasesEx()
{

    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }
    ba[i] = 50; //instantaneous measure with ieee mode - 24 01 command
    ba[i+1] = 52;
    ba[i+2] = 48;
    ba[i+3] = 49;

    sendData(1, &ba);

    last_command = READMEASURE;

}

void enveas::readStatus()
{
    QByteArray ba;
    ba.resize(1);
    ba[0] = 110; //state of equipment

    sendData(1, &ba);
}

void enveas::readAbout()
{
    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }
    ba[i] = 49; //soft identification
    ba[i+1] = 50;
    sendData(1, &ba);

    last_command = ABOUTSFTWR;
}

void enveas::standby()
{
    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }

    ba[i] = 49; //standby mode 14 command
    ba[i+1] = 52;
    sendData(1, &ba);

    last_command = STANDBY;
}

void enveas::start()
{
    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }
    ba[i] = 48; //measure 09 command
    ba[i+1] = 57;
    sendData(1, &ba);

    last_command = START;
}
void enveas::sync()
{
    QDateTime _now = QDateTime::currentDateTime();

    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }
    ba[i] = 48; //sync 01 command
    ba[i+1] = 49;
    ba[i+2] = char(toAscii( _now.toString("yyyy").mid(2,1).toInt()));
    ba[i+3] = char(toAscii( _now.toString("yyyy").mid(3,1).toInt()));
    ba[i+4] = char(toAscii( _now.toString("MM").mid(0,1).toInt()));
    ba[i+5] = char(toAscii( _now.toString("MM").mid(1,1).toInt()));
    ba[i+6] = char(toAscii( _now.toString("dd").mid(0,1).toInt()));
    ba[i+7] = char(toAscii( _now.toString("dd").mid(1,1).toInt()));
    ba[i+8] = char(toAscii( _now.toString("HH").mid(0,1).toInt()));
    ba[i+9] = char(toAscii( _now.toString("HH").mid(1,1).toInt()));
    ba[i+10] = char(toAscii( _now.toString("mm").mid(0,1).toInt()));
    ba[i+11] = char(toAscii( _now.toString("mm").mid(1,1).toInt()));
    ba[i+12] = char(toAscii( _now.toString("ss").mid(0,1).toInt()));
    ba[i+13] = char(toAscii( _now.toString("ss").mid(1,1).toInt()));

    sendData(1, &ba);

    last_command = SYNC;
}

void enveas::zero_ref()
{
    QByteArray ba;
    int i = 0;
    ba.resize(2 + model.length());
    for (i =0; i< model.length(); i++)
    {
        ba[i]=  (model.toLatin1().at(i));
    }
    ba[i] = 48; //zero referencing command is 05
    ba[i+1] = 53;
    sendData(1, &ba);

    last_command = ZEROREFERENCE;
}

void enveas::sendData(int command, QByteArray *data)
{
    int checksum =  0; //id = 0
    QString _msg="";

    if (data->length() == 2){
        checksum = checksum ^  data->length() ^ data->at(0) ^ data->at(1);
    } else
    {
        //checksum = checksum ^ command ^ data->length();
        for (int i =0; i < data->length(); i++)
        {
            checksum = checksum ^ (data->at(i) & 0xFF);
        }
        // checksum = checksum  ^ data->at(0) ^ data->at(1) ^ 0xb7;

    }
    if (data->length() == 2){
        _msg = QString(0x02) +QString(data->at(0)) + QString(data->at(1)) + QString(checksum) + QLatin1Char(0x04);
    } else
    {
        _msg = QString(0x02) ;
        for (int i =0; i < data->length(); i++)
        {
            _msg = _msg + QString(data->at(i) & 0xFF);

        }
        //QByteArray _crc = crc();
        _msg = _msg + QString(toAscii((checksum >> 4) & 0xF )) +  QString(toAscii((checksum) & 0xF) )+ QLatin1Char(0x03);

        //_msg = QString(0x02) + QLatin1Char(0) + QLatin1Char(command) + QLatin1Char(0x03) + QString(data->length()) + QString(data->at(0)) + QString(data->at(1))+ QLatin1Char(0xb7)+ QString(checksum) + QLatin1Char(0x04);
    }
    //strcat(str,  QLatin1Char(51));
    //strcat(str,  "\r");
    qint64 lnt = _msg.size();//qint64(strlen(str));

    lnt = m_sock->write(_msg.toLatin1(), lnt);
    lnt = m_sock->flush();

    if (verbose)
        qDebug()<< "\n\rEnvea command: " << _msg <<"\n\r" ;
}

QByteArray enveas::crc(QByteArray command)
{
    QByteArray crc, _crc;
    int sum = 0;

    for (int i = 0; i < command.length();i++)
    {
        sum += int( command[i]);
    }
    _crc.setNum(sum, 16); // sum to bytes array
    crc.append(int(_crc[_crc.length()-2])); //dd two last digits in ASCII code
    crc.append(int(_crc[_crc.length()-1]));

    return crc;
}

int enveas::toAscii(int _ch)
{
    if (_ch == 0)
        return 48;
    if (_ch == 1)
        return 49;
    if (_ch == 2)
        return 50;
    if (_ch == 3)
        return 51;
    if (_ch == 4)
        return 52;
    if (_ch == 5)
        return 53;
    if (_ch == 6)
        return 54;
    if (_ch == 7)
        return 55;
    if (_ch == 8)
        return 56;
    if (_ch == 9)
        return 57;
    if (_ch == 10)
        return 65;
    if (_ch == 11)
        return 66;
    if (_ch == 12)
        return 67;
    if (_ch == 13)
        return 68;
    if (_ch == 14)
        return 69;
    if (_ch == 15)
        return 70;
}


void enveas::writes()
{
    //qDebug()<< "written " ;
}
