#include "HttpRequest.h"

#include <QtNetwork>

#include "Exception.h"

class HttpRequestPrivate
{
public:
    pAccount account;

    QUrl createQUrlFor(const QString path)
    {
        QUrl url("https://"+account->host()+":"+QString::number(account->port())+path);
        url.setPassword(account->password());
        url.setUserName(account->username());
        return url;
    }

    QVariantMap parseReply(const QByteArray &reply)
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply);
        QVariantMap response = doc.toVariant().toMap();
        if ( ! response["success"].toBool() )
        {
            bool ok;
            int errorcode = response["statuscode"].toInt(&ok);
            if( ! ok )
            {
                ThrowFatal(QObject::tr("Server is talking Gibberish"), QObject::tr("Statuscode was not returned along with an error"));
            }
            switch(errorcode)
            {
            case 401: Throw(
                            QObject::tr("Bad Credentials"),
                            QObject::tr("Your email and password could not be authenticated."),
                            QObject::tr("Check that you typed the corredt email and password. It the problem persists, contact your administrator."));
            default:
                throw errorcode;
                break;
            }
        }

        QVariantMap data = response["data"].toMap();
        return data;
    }


};



HttpRequest::HttpRequest(pAccount a)
{
    d = new HttpRequestPrivate;
    d->account = a;
}

HttpRequest::~HttpRequest()
{
}

QVariantMap HttpRequest::doGet(const QString &path)
{
    QNetworkAccessManager nam;
    QNetworkRequest request;
    request.setUrl(d->createQUrlFor(path));
    request.setRawHeader("Host","sync.data");
    QNetworkReply *reply = nam.get(request);


    QObject::connect(reply,
            static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](){
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=](){
        reply->ignoreSslErrors();
    });

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    return d->parseReply(reply->readAll());

}


void HttpRequest::doDelete(const QString &path)
{
    QNetworkAccessManager nam;
    QNetworkRequest request;
    request.setUrl(d->createQUrlFor(path));
    request.setRawHeader("Host","sync.data");
    QNetworkReply *reply = nam.deleteResource(request);


    QObject::connect(reply,
            static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](){
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=](){
        reply->ignoreSslErrors();
    });

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    //We parese the reply in order to catch exceptions.
    d->parseReply(reply->readAll());
}

QVariantMap HttpRequest::doPut(const QString &path, QVariantMap data)
{
    QNetworkAccessManager nam;
    QNetworkRequest request;
    request.setUrl(d->createQUrlFor(path));
    request.setRawHeader("Host","sync.data");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");


    QJsonDocument doc = QJsonDocument::fromVariant(data);
    QNetworkReply *reply = nam.put(request, doc.toJson());


    QObject::connect(reply,
            static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](){
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=](){
        reply->ignoreSslErrors();
    });

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray b = reply->readAll();
    return d->parseReply(b);

}

QVariantMap HttpRequest::doPost(const QString &path, QVariantMap data)
{
    QNetworkAccessManager nam;
    QNetworkRequest request;
    request.setUrl(d->createQUrlFor(path));
    request.setRawHeader("Host","sync.data");

    QByteArray json = QJsonDocument::fromVariant(data).toJson().toBase64();
    QNetworkReply *reply = nam.post(request, json);


    QObject::connect(reply,
            static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            [=](){
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, [=](){
        reply->ignoreSslErrors();
    });

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    return d->parseReply(reply->readAll());

}

