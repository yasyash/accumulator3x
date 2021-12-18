/*
 * Copyright © 2018-2021 Yaroslav Shkliar <mail@ilit.ru>
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

#include "serinus.h"
#include <math.h>

#include <QDebug>

#ifdef SERINUS_H
Serinus::Serinus(QObject *parent , QString *ip, quint16 *port) : QObject (parent)

{

    m_sock = new QTcpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    // connect(m_sock, SIGNAL(bytesWritten(qint64)), this, SLOT(writes()));

    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataIsReady(const QString)), this, SLOT(writes()) );

    changeInterface(*ip, *port);
    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    //qDebug() << "Socket " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);
    // m_sock->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 1024);
    // qDebug() << "Socket next " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);

    m_sock->setSocketOption(QAbstractSocket::TypeOfServiceOption, 64);

    measure = new  QMap<QString, float>;
    sample_t = new QMap<QString, int>;

    measure->insert("SO2", 0);
    measure->insert("H2S", 0);

    sample_t->insert("SO2", 0 );
    sample_t->insert("H2S", 0 );

    is_read = false;
    status = new     QMap<QString, _status>;

    connected = m_sock->state();

    qDebug() << "Serinus 51 measure equipment handling has been initialized.\n\r";

}

Serinus::Serinus(QObject *parent , QString *ip, quint16 *port, int type) : QObject (parent)

{

    m_sock = new QTcpSocket(this);

    connect(m_sock, SIGNAL(readyRead()), this, SLOT(readData()));
    // connect(m_sock, SIGNAL(bytesWritten(qint64)), this, SLOT(writes()));

    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    //connect(this, SIGNAL(dataIsReady(const QString)), this, SLOT(writes()) );

    changeInterface(*ip, *port);
    m_sock->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    //qDebug() << "Socket " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);
    // m_sock->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 1024);
    // qDebug() << "Socket next " << m_sock->socketOption(QAbstractSocket::SendBufferSizeSocketOption);

    m_sock->setSocketOption(QAbstractSocket::TypeOfServiceOption, 64);

    m_type = type;

    measure = new  QMap<QString, float>;
    sample_t = new QMap<QString, int>;

    switch (m_type) {
    case 51:

        measure ->insert("SO2", 0);
        measure->insert("H2S", 0);
        sample_t->insert("SO2", 0);
        sample_t->insert("H2S", 0);

        break;

    case 50:
        measure ->insert("SO2", 0);
        sample_t->insert("SO2", 0);

        break;
    case 55:
        measure->insert("H2S", 0);
        sample_t->insert("H2S", 0);

        break;
    case 44:

        measure ->insert("NO", 0);
        measure->insert("NO2", 0);
        measure ->insert("NOx", 0);
        measure->insert("NH3", 0);
        sample_t->insert("NO", 0);
        sample_t->insert("NO2", 0);
        sample_t->insert("NOx", 0);
        sample_t->insert("NH3", 0);
        break;

    case 30:

        measure ->insert("CO", 0);
        sample_t->insert("CO", 0);

        break;
    default:
        break;
    }
    is_read = false;
    status = new     QMap<QString, _status>;
    connected = m_sock->state();

    qDebug() << "Serinus" << type <<" measure equipment handling has been initialized.\n\r";

}

Serinus::~Serinus()
{
    m_sock->disconnectFromHost();
}



void Serinus::changeInterface(const QString &address, quint16 portNbr)
{
    m_sock->connectToHost(address, portNbr);
}





void Serinus::on_cbEnabled_clicked(bool checked)
{
    if (checked)
    {
    }
    else {
        m_sock->disconnectFromHost();
    }
    //emit tcpPortActive(checked);
}


void Serinus::readData()
{

    QStringList list;
    int ind;
    int running;
    QRegExp xRegExp("(-?\\d+(?:[\\.,]\\d+(?:e\\d+)?)?)");

    QByteArray _data = m_sock->readAll();
    QByteArray _status;
    //if (verbose)
    //qDebug() << "Serinus" << m_type << " buffer --- data: " << _data << " lenght - " << _data.length() << " \n\r";

    if (is_read)
    {switch (m_type) {
        case 51:

            measure ->insert("SO2", 0);
            measure->insert("H2S", 0);
            sample_t->insert("SO2", 0);
            sample_t->insert("H2S", 0);

            break;

        case 50:
            measure ->insert("SO2", 0);
            sample_t->insert("SO2", 0);

            break;
        case 55:
            measure->insert("H2S", 0);
            sample_t->insert("H2S", 0);

            break;
        case 44:

            measure ->insert("NO", 0);
            measure->insert("NO2", 0);
            measure ->insert("NOx", 0);
            measure->insert("NH3", 0);
            sample_t->insert("NO", 0);
            sample_t->insert("NO2", 0);
            sample_t->insert("NOx", 0);
            sample_t->insert("NH3", 0);
            break;

        case 30:

            measure ->insert("CO", 0);
            sample_t->insert("CO", 0);

            break;
        default:
            break;

        }

        is_read = false;
    }

    char sign;
    int expo;
    uint32_t mantissa;
    uint32_t ieee;
    int i, j, start;
    float result;
    int total_regs = 2;
    enum _status __status;

    data.append( _data); //copy to main buffer

    if (data.length() > 16)  //data buffer detection for fullness
    {
        start = data.indexOf(0x04);

        if (( int(data[2]) == 1 ) && (int(data[4]) == 5)) //if status request
        {
            if (int(data[5]) == 110)
            {
                switch (int(data[6])) {

                case 0 : __status = SAMPLE_FILL;
                    break;
                case 63 : __status = MEASURING;
                    break;
                case 28 : __status = ELECTRONIC_ZERO_ADJUST;
                    break;
                case 29 : __status = INSTRUMENT_WARM_UP;
                    break;
                default:
                    __status = UNKNOWN;
                    break;
                }
                if (verbose)
                    qDebug() <<  "Serinus"<< m_type <<" return status is " << int(data[6]);
            }
            if (start < data.length()-1){

                data = data.mid(start+1);
            }
        }



        if ((( int(data[2]) == 1 ) && (int(data[4]) == 10)) || (( int(data[2]) == 1 ) && (int(data[4]) == 15)) || (( int(data[2]) == 1 ) && (int(data[4]) == 20))) //detect right response for pri. and sec. gas request
        {
            if (( int(data[2]) == 1 ) && (int(data[4]) == 10))
                total_regs =2;

            if (( int(data[2]) == 1 ) && (int(data[4]) == 15))
                total_regs = 3;

            if (( int(data[2]) == 1 ) && (int(data[4]) == 20))
                total_regs = 4;
            for (j = 0; j < total_regs; j++) {



                sign = data [5*j + 6] >> 7;
                ieee = uint32_t((uchar(data [5*j + 6]) << 24 ) | (uchar(data [5*j + 7]) << 16) | (uchar(data[5*j + 8]) << 8) | uchar(data[5*j + 9]));
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

                switch (m_type) {
                case 51:

                    if (int(data[5*j + 5]) == 50){
                        if (result >= 0) { //negative value detection
                            sample_t->insert("SO2", sample_t->value("SO2") + 1);
                            measure->insert("SO2",  measure->value("SO2") + result);
                        }
                        else
                        {
                            sample_t->insert("SO2", sample_t->value("SO2") + 1);
                            measure->insert("SO2",  measure->value("SO2") + 0.00000000f);
                        }
                    }

                    if (int(data[5*j + 5]) == 51){
                        if (result >= 0) {//negative value detection
                            sample_t->insert("H2S", sample_t->value("H2S") + 1);
                            measure->insert("H2S", measure->value("H2S") + result);
                        }
                        else
                        {
                            sample_t->insert("H2S", sample_t->value("H2S") + 1);
                            measure->insert("H2S", measure->value("H2S") + 0.00000000f);
                        }
                    }
                    status->insert("SO2", __status);
                    status->insert("H2S", __status);

                    break;

                case 50:
                    if (int(data[5*j + 5]) == 50){
                        if (result >= 0) { //negative value detection
                            sample_t->insert("SO2", sample_t->value("SO2") + 1);
                            measure->insert("SO2",  measure->value("SO2") + result);
                        }
                        else
                        {
                            sample_t->insert("SO2", sample_t->value("SO2") + 1);
                            measure->insert("SO2",  measure->value("SO2") + 0.00000000f);
                        }}
                    status->insert("SO2", __status);

                    break;
                case 55:
                    if (int(data[5*j + 5]) == 50){
                        if (result >= 0) {//negative value detection
                            sample_t->insert("H2S", sample_t->value("H2S") + 1);
                            measure->insert("H2S", measure->value("H2S") + result);
                        }
                        else
                        {
                            sample_t->insert("H2S", sample_t->value("H2S") + 1);
                            measure->insert("H2S", measure->value("H2S") + 0.000000000f);
                        }}

                    status->insert("H2S", __status);

                    break;

                case 30:
                    if (int(data[5*j + 5]) == 50){
                        if (result >= 0) { //negative value detection
                            sample_t->insert("CO", sample_t->value("CO") + 1);
                            measure->insert("CO",  measure->value("CO") + result);
                        }
                        else
                        {
                            sample_t->insert("CO", sample_t->value("CO") + 1);
                            measure->insert("CO",  measure->value("CO") + 0.00000000f);
                        }}
                    status->insert("CO", __status);

                    break;

                case 44:

                    if (uint(data[5*j + 5]) == 50){
                        if (result >= 0) { //negative value detection
                            sample_t->insert("NO", sample_t->value("NO") + 1);
                            measure->insert("NO",  measure->value("NO") + result);
                        }
                        else
                        {
                            sample_t->insert("NO", sample_t->value("NO") + 1);
                            measure->insert("NO",  measure->value("NO") + 0.00000000f);
                        }
                    }

                    if (uint(data[5*j + 5]) == 51){
                        if (result >= 0) {//negative value detection
                            sample_t->insert("NOx", sample_t->value("NOx") + 1);
                            measure->insert("NOx", measure->value("NOx") + result);
                        }
                        else
                        {
                            sample_t->insert("NOx", sample_t->value("NOx") + 1);
                            measure->insert("NOx", measure->value("NOx") + 0.00000000f);
                        }
                    }

                    if (uint(data[5*j + 5]) == 52){
                        if (result >= 0) {//negative value detection
                            sample_t->insert("NO2", sample_t->value("NO2") + 1);
                            measure->insert("NO2", measure->value("NO2") + result);
                        }
                        else
                        {
                            sample_t->insert("NO2", sample_t->value("NO2") + 1);
                            measure->insert("NO2", measure->value("NO2") + 0.00000000f);
                        }
                    }


                    if (int(data[5*j + 5]) == -73){
                        if (result >= 0) {//negative value detection
                            sample_t->insert("NH3", sample_t->value("NH3") + 1);
                            measure->insert("NH3", measure->value("NH3") + result);
                        }
                        else
                        {
                            sample_t->insert("NH3", sample_t->value("NH3") + 1);
                            measure->insert("NH3", measure->value("NH3") + 0.00000000f);
                        }
                    }
                    status->insert("NOx", __status);
                    status->insert("NO2", __status);
                    status->insert("NO", __status);
                    status->insert("NH3", __status);
                    break;

                default:
                    break;

                }



            }
            is_read = false;

            if (verbose)
                qDebug() << "Serinus"<< m_type <<" measure equipment data: " << data << " lenght - " << data.length() << " \n\r";

            data.clear();
            emit dataIsReady(&is_read, measure, sample_t, status);

        }

        if (data.length() >39)//clear if wrong buffer overflow
            data.clear();



    }


    blockSize = 0;

}

void Serinus::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:

        qDebug()<<   "Serinus " << QString(m_type) << " measure equipment handling error: The host was not found. Please check the "
                                                      "host name and port settings.\n\r";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<< ("Serinus measure equipment handling error: The connection was refused by the peer. "
                    "Make sure the fortune server is running, "
                    "and check that the host name and port "
                    "settings are correct.\n\r");
        break;
    default:
        qDebug()<< ("Serinus measure equipment handling error: ") << (m_sock->errorString()) << "\n\r";
    }

    if (m_sock->isOpen())
        m_sock->close();
    connected = m_sock->state();

}

void Serinus::readGases(int qw)
{
    if (qw <3)
    {
        QByteArray ba;
        ba.resize(2);
        ba[0] = 50; //primary gas response
        ba[1] = 51; //secondary gas response
        sendData(1, &ba);
    }
    else {
        QByteArray ba;
        ba.resize(qw);
        ba[0] = 50; //primary gas response
        ba[1] = 51; //secondary gas response
        ba[2] = 52; //third gas response
        ba[3] = char(0xb7); //fourth gas response

        sendData(1, &ba);
    }

}

void Serinus::readStatus()
{
    QByteArray ba;
    ba.resize(1);
    ba[0] = 110; //state of equipment

    sendData(1, &ba);
}

void Serinus::sendData(int command, QByteArray *data)
{
    int checksum =  0; //id = 0
    QString _msg="";

    if (data->length() == 2){
        checksum = checksum ^ command ^ data->length() ^ data->at(0) ^ data->at(1);
    } else
    {
        checksum = checksum ^ command ^ data->length();
        for (int i =0; i < data->length(); i++)
        {
            checksum = checksum ^ (data->at(i) & 0xFF);
        }
        // checksum = checksum  ^ data->at(0) ^ data->at(1) ^ 0xb7;

    }
    if (data->length() == 2){
        _msg = QString(0x02) + QLatin1Char(0) + QLatin1Char(command) + QLatin1Char(0x03) + QString(data->length()) + QString(data->at(0)) + QString(data->at(1)) + QString(checksum) + QLatin1Char(0x04);
    } else
    {
        _msg = QString(0x02) + QLatin1Char(0) + QLatin1Char(command) + QLatin1Char(0x03) + QString(data->length());
        for (int i =0; i < data->length(); i++)
        {
            _msg = _msg + QString(data->at(i) & 0xFF);

        }
        _msg = _msg + QString(checksum) + QLatin1Char(0x04);

        //_msg = QString(0x02) + QLatin1Char(0) + QLatin1Char(command) + QLatin1Char(0x03) + QString(data->length()) + QString(data->at(0)) + QString(data->at(1))+ QLatin1Char(0xb7)+ QString(checksum) + QLatin1Char(0x04);
    }
    //strcat(str,  QLatin1Char(51));
    //strcat(str,  "\r");
    qint64 lnt = _msg.size();//qint64(strlen(str));

    lnt = m_sock->write(_msg.toLatin1(), lnt);
    lnt = m_sock->flush();

    //qDebug()<< "\n\rSerinus command: " << _msg <<"\n\r" ;
}

void Serinus::writes()
{
    if (verbose)
        qDebug()<< "written " ;
}
#endif
