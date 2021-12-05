/*
 * Copyright © 2018-2019 Yaroslav Shkliar <mail@ilit.ru>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Research Laboratory of IT
 * www.ilit.ru on e-mail: mail@ilit.ru
 */

#ifndef MODBUSIP_H
#define MODBUSIP_H


#include <QUdpSocket>
#include <QTcpSocket>
#include <QRunnable>

class ModbusIP : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ModbusIP(QObject *parent, QString *ip, quint16 *port);
    ModbusIP(QObject *parent, QString *ip, quint16 *port, int type);
    void run ();

    virtual ~ModbusIP();
    int toAscii(int *_ch);
    void sendData(int &address, int &registers);
    void sendData(); //for use to sending in a separate thread

protected:
    void changeInterface(const QString& address, quint16 portNbr);

private slots:
    void on_cbEnabled_clicked(bool checked);
    void readData();
    void displayError(QAbstractSocket::SocketError socketError);
    void writes();


private:
    QTcpSocket *m_sock;
    quint32 blockSize;
    QString *m_ip;
    int *m_port;

public:
    //enum _status {Idle, Waiting, Running};
    int m_type = 485;

    enum _command {RDMN, MSTATUS, MSTART, MSTOP, RMMEAS};
    enum _status {MEASURING, DOWN, FAILURE, TEMP_NOT_READY, SENS_CHNG, SAMPLE_FILL, ELECTRONIC_ZERO_ADJUST, INSTRUMENT_WARM_UP, UNKNOWN, ABSENT};
    Q_ENUM(_status)

    QString model;
    QMap<QString, _status> *status;
    bool is_read;
    QMap<QString, int> *sample_t;
    QMap<QString, int> *measure;
    enum _command last_command;
    QAbstractSocket::SocketState connected = QAbstractSocket::UnconnectedState;
    // QDataStream *in_stream;
    int m_address;
    int m_registers;

signals:
    void tcpPortActive(bool val);
    void connectionError(const QString &msg);
    void dataIsReady( bool *is_read, QMap<QString, int> *_measure, QMap<QString, int> *_sample, QMap<QString, _status> *_status  );
};

#endif // MODBUSIP_H
