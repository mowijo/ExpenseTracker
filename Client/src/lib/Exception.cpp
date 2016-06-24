#include "Exception.h"

class ExceptionPrivate
{

public:

    QString file;
    int line;
    QString headline;
    QString body;
    QString remedy;
};

Exception::Exception()
{
    d = new ExceptionPrivate;
}


Exception::~Exception()
{
    delete d;
}

void Exception::setFile(const QString &file)
{
    d->file = file;
}

void Exception::setLine(int line)
{
    d->line = line;
}

void Exception::setHeadLine(const QString &headline)
{
    d->headline = headline;
}

void Exception::setBody(const QString &body)
{
    d->body = body;
}

void Exception::setRemedy(const QString &remedy)
{
    d->remedy = remedy;
}

QString Exception::file() const
{
    return d->file;
}

int Exception::line() const
{
    return d->line;
}

QString Exception::headLine() const
{
    return d->headline;
}

QString Exception::body() const
{
    return d->body;
}

QString Exception::remedy() const
{
    return d->remedy;
}

bool Exception::isFatal() const
{
    return false;
}


FatalException::FatalException()
    : Exception()
{
}

bool FatalException::isFatal() const
{
    return true;
}
