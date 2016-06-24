#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QVariantMap>

#include "Account.h"

class HttpRequest
{
public:
    explicit HttpRequest(pAccount a);
    ~HttpRequest();

    QVariantMap doGet(const QString &path);
    QVariantMap doPut(const QString &path, QVariantMap data);
    QVariantMap doPost(const QString &path, QVariantMap data);
    void doDelete(const QString &path);

private:
    HttpRequest();
    class HttpRequestPrivate *d;
};


#endif // HTTPREQUEST_H

