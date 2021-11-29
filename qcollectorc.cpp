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


#include "qcollectorc.h"

qcollectorc::qcollectorc(QSqlDatabase * _conn, QString &idd, QDateTime &_from_t, QDateTime &_to_t, QDateTime &_last_t, QString &uri, int &code, QString &token, QString &locality, double &msg_id, int &msg_id_out, QObject *parent = 0 ) : QObject(parent)
{
    m_conn = new QSqlDatabase(*_conn);

    m_conn->open();
    bool status = m_conn->open();
    if (!status)
    {
        //releaseModbus();

        QTextStream(stdout) << ( QString("Connection " + m_conn->connectionName() + " error: " + m_conn->lastError().text()).toLatin1().constData()) <<   " \n\r";

    } else {
        m_idd = idd;
        m_from_t = _from_t;
        m_to_t = _to_t;
        m_last_t = _last_t;
        m_uri = uri;
        m_code = code;
        m_token = token;
        m_locality = locality;
        m_msg_id = msg_id;
        m_msg_id_out = msg_id_out;
    }
}

qcollectorc::~qcollectorc()
{

}

void funcSuccess(const QJsonObject &_resp,const QString &uri,const QDateTime &_date_time,const QDateTime &_last_time,const int &_msg_id, QSqlDatabase * m_conn,const QString &idd)
{
    QDateTime _now = QDateTime::currentDateTime();
    bool _result = _resp.value("success").toBool();
    if (_result)
    {
        QString _update = QString("UPDATE injection set date_time = '"+ _last_time.toString("yyyy-MM-ddTHH:mm:ss") +
                                  "', last_time = '"+ _date_time.toString("yyyy-MM-ddTHH:mm:ss")+"', msg_id_out = "+
                                  QString::number(_msg_id) + ", msg_id = 0 WHERE idd = '" + idd +"'");

        QSqlQuery *query_inj = new QSqlQuery(*m_conn);

        query_inj->prepare(_update);
        query_inj->exec();

        if(!query_inj ->lastError().isValid())
        {
            query_inj->finish();

            QString _insert = QString("INSERT INTO injection_logs (date_time, transaction, uri, request_time, response_time, is_ok, errors, msg_id_out) VALUES ('"+
                                      _last_time.toString("yyyy-MM-ddTHH:mm:ss")+"', '"+_resp.value("transaction").toString()+ "','" + uri + "','"+
                                      _date_time.toString("yyyy-MM-ddTHH:mm:ss")+"', "+QString::number(_now.toMSecsSinceEpoch() -  _date_time.toMSecsSinceEpoch())+
                                      +",'true', '', " + QString::number(_msg_id)  +")");
            query_inj->prepare(_insert);
            query_inj->exec();
            query_inj->finish();

            _insert = QString("INSERT INTO injected (date_time, msg_id, uri, transaction, msg_time, idd, msg_id_out) VALUES ('"+
                              _last_time.toString("yyyy-MM-ddTHH:mm:ss")+"'," +QString::number(_msg_id) +",'" + uri + "','"+_resp.value("transaction").toString()+ "', '"+
                              _date_time.toString("yyyy-MM-ddTHH:mm:ss")+"', '"+ idd + "', " + QString::number(_msg_id)  +")");
            query_inj->prepare(_insert);
            query_inj->exec();

        }
        query_inj->finish();
        QTextStream(stdout) << "Success REST API response: Time is "+ _last_time.toString("yyyy-MM-dd HH:mm:ss") + " ID message = "  + QString::number(_msg_id) + " Response time = "<< QString::number(_now.toMSecsSinceEpoch() -  _date_time.toMSecsSinceEpoch()) <<" ms. \n\r";
        query_inj->clear();
    } else {

        QString _update = QString("UPDATE injection SET  last_time = '"+ _date_time.toString("yyyy-MM-ddTHH:mm:ss") + "', msg_id = " + QString::number(_msg_id) +" WHERE idd = '" + idd +"'");

        QSqlQuery *query_inj = new QSqlQuery(*m_conn);

        query_inj->prepare(_update);
        query_inj->exec();

        /* if(!query_inj ->lastError().isValid())
        {
            query_inj->finish();

            QString _insert = QString("INSERT INTO injection_logs (date_time, transaction, request_time, response_time, is_ok, errors, msg_id_out) VALUES ('"+
                                      _last_time.toString("yyyy-MM-ddTHH:mm:ss")+"', '"+_resp.value("transaction").toString()+ "', '"+
                                      _date_time.toString("yyyy-MM-ddTHH:mm:ss")+"', "+QString().arg(_now.toMSecsSinceEpoch() -  _date_time.toMSecsSinceEpoch())+
                                      +"'false'," +_resp.value("error").toString() + "', " + QString().arg(_msg_id)  +")");
            query_inj->prepare(_insert);
            query_inj->exec();
        }*/
        query_inj->finish();
        QTextStream(stdout) << "!!! Error REST API response: Time is "+ _date_time.toString("yyyy-MM-dd HH:mm:ss") + " ID message = "  + QString::number(_msg_id) + " \n\r" +
                               " Reason: " +  _resp.value("error").toString() + " \n\r Response time (ms):  "+ QString::number(_now.toMSecsSinceEpoch() -  _date_time.toMSecsSinceEpoch())+" ms\n\r";

        query_inj->clear();
    }

}

void funcError(const QJsonObject &_resp,const QString &uri,const QDateTime &_date_time,const QDateTime &_last_time,const int &_msg_id, QSqlDatabase * m_conn,const QString &idd)
{

    /* if(!query_inj ->lastError().isValid())
    {
        query_inj->finish();

        QString _insert = QString("INSERT INTO injection_logs (date_time, transaction, request_time, response_time, is_ok, errors, msg_id_out) VALUES ('"+
                                  _last_time.toString("yyyy-MM-ddTHH:mm:ss")+"', '"+_resp.value("transaction").toString()+ "', '"+
                                  _date_time.toString("yyyy-MM-ddTHH:mm:ss")+"', "+QString().arg(_now.toMSecsSinceEpoch() -  _date_time.toMSecsSinceEpoch())+
                                  +"'false'," +_resp.value("error").toString() + "', " + QString().arg(_msg_id)  +")");
        query_inj->prepare(_insert);
        query_inj->exec();
    }*/
    QTextStream(stdout) << "!!! Error REST API communication: Time is "+ _date_time.toString("yyyy-MM-dd HH:mm:ss") + " \n\r" +
                           " Reason caught: " +  _resp.value("QNetworkReply").toString() + " \n\r";

}

void qcollectorc::run ()

{
    qint64 i = 0;
    QString select_eq = QString("SELECT * FROM equipments WHERE is_present = true and idd = '" + m_idd +"'");
    QSqlQuery *query_eq = new QSqlQuery(*m_conn);
    QSqlRecord rs_sensor;
    QDateTime _begin_t = m_from_t;

    query_eq->prepare(select_eq);
    query_eq->exec();

    QString select_data = QString("SELECT * FROM sensors_data WHERE idd ='").append(m_idd).append("' AND date_time between '" + m_from_t.toString("yyyy-MM-ddTHH:mm:ss")+"' AND '" + m_to_t.toString("yyyy-MM-ddTHH:mm:ss") +"' order by date_time ASC");
    QSqlQuery *query_data = new QSqlQuery(*m_conn);

    query_data->prepare(select_data);
    query_data->exec();

    bool _go_out = true;
    QSqlRecord rs_data;

    if(!query_data->lastError().isValid())
    {
        if (query_data->size() > 0)
        {
            _go_out = false;
            query_data->first();
            rs_data = query_data->record();
            QDateTime _first_time = QDateTime::fromString(rs_data.field("date_time").value().toString().left(19),"yyyy-MM-ddTHH:mm:ss");
            if (_first_time > m_from_t)
                _begin_t = _first_time;
        }
    } else {
        _go_out = false;
    }

    //begin work
    if(!query_eq->lastError().isValid())
    {
        select_data = QString("SELECT * FROM sensors_data WHERE date_time between :_from AND :_to AND serialnum = :_serialnum ORDER BY date_time ASC");

        while (!_go_out) {

            QDateTime _from_t_local = _begin_t.addSecs(i*60);
            QDateTime _to_t_local = _begin_t.addMSecs(i*60000+59999 ); //add 999 mSec for 'between' sql frame
            if (_to_t_local > m_to_t)
                _go_out = true;

            QJsonArray _params;

            // query_eq->first();

            while (query_eq->next()) {
                rs_sensor = query_eq->record();
                if (!rs_sensor.field("typemeasure").value().toString().contains(QString("Напряжение")) && !rs_sensor.field("typemeasure").value().toString().contains(QString("ИБП")))
                {
                    query_data->finish();

                    query_data->prepare(select_data);
                    query_data->bindValue(":_from", _from_t_local.toString("yyyy-MM-ddTHH:mm:ss"));
                    query_data->bindValue(":_to", _to_t_local.toString("yyyy-MM-ddTHH:mm:ss"));
                    query_data->bindValue(":_serialnum", rs_sensor.field("serialnum").value().toString());
                    QString _t =  rs_sensor.field("serialnum").value().toString();
                    query_data->exec();
                    //query_data->first();
                    if(!query_data->lastError().isValid())
                    {
                        if (query_data->size() > 0){
                            QJsonObject _data_one;
                            double _measure = 0.0;
                            while (query_data->next()) {
                                rs_data = query_data->record();
                                _measure += rs_data.field("measure").value().toDouble();
                            }
                            _data_one = {{"date_time",rs_data.field("date_time").value().toString().append(QString("+").append((QDateTime::currentDateTime().offsetFromUtc()/3600 < 10) ? (QString("0").append(QString::number( QDateTime::currentDateTime().offsetFromUtc()/3600))) : (QString::number(QDateTime::currentDateTime().offsetFromUtc()/3600 ))))},{"unit",rs_sensor.field("unit_name").value().toString()},{"measure", _measure/query_data->size()}};
                            QJsonObject _chemical_one = {{aspiap_dir.value(rs_sensor.field("typemeasure").value().toString()),_data_one}};
                            _params.append(QJsonValue( _chemical_one));
                        }
                    }
                }
            }


            if (_params.size() > 0){
                Requester *_req = new Requester();

                _req->initRequester(m_uri,443, nullptr);

                QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
                QHttpPart _http_form;
                QByteArray _body_tmp;

                QDateTime _now = QDateTime::currentDateTime();

                QJsonObject _header = {{"token", m_token},{"message",QString().number( (m_msg_id_out+=1))},{"locality", m_locality},{"object", m_code},{"date_time", _now.toString("yyyy-MM-dd HH:mm:ss").append(QString("+").append((QDateTime::currentDateTime().offsetFromUtc()/3600 < 10) ? (QString("0").append(QString::number( QDateTime::currentDateTime().offsetFromUtc()/3600))) : (QString::number(QDateTime::currentDateTime().offsetFromUtc()/3600 ))))},
                                       {"params", _params}};

                QJsonDocument doc(_header);
                _body_tmp = doc.toJson();


                _http_form.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data;  name=\"data\""));
                _http_form.setBody(_body_tmp);

                multiPart->append(_http_form);
                multiPart->setBoundary("---");

                //try to send request
                //void (qcollectorc::*funcS)(const QJsonObject &, const QString &, const QDateTime &, const QDateTime &, const int &,  QSqlDatabase *, const QString &);
                //void (qcollectorc::*funcE)(const QJsonObject &, const QString &, const QDateTime &, const QDateTime &, const int &,  QSqlDatabase *, const QString &);

                // funcE = &qcollectorc::funcError;
                // funcS = &qcollectorc::funcSuccess;

                _req->sendRequest(funcSuccess, funcError, Requester::Type::POST, multiPart, m_uri, _now, _from_t_local, m_msg_id_out, m_conn, m_idd);

            }
            query_eq->exec();
            i++;
        }


    }
    query_eq->clear();
    query_data->clear();
}
