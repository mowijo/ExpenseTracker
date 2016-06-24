#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>

class Exception
{

public:

    Exception();
    virtual ~Exception();
    void setFile(const QString &file);
    void setLine(int line);
    void setHeadLine(const QString &headline);
    void setBody(const QString &body);
    void setRemedy(const QString &remedy);

    QString file() const;
    int line() const;
    QString headLine() const;
    QString body() const;
    QString remedy() const;
    virtual bool isFatal() const;

private:
    Exception(const Exception &other);
    Exception(const Exception &&other);
    Exception& operator=(Exception &rhs);
    friend class ExceptionPrivate;
    class ExceptionPrivate *d;
};


class FatalException : public Exception
{
public:
    FatalException();
    bool isFatal() const;
};


#define ThrowFatal(headline, body) { FatalException *e = new FatalException; e->setFile(__FILE__); e->setLine(__LINE__); e->setHeadLine(headline); e->setBody(body); throw e; }
#define Throw(headline, body, remedy) { Exception *e = new Exception; e->setFile(__FILE__); e->setLine(__LINE__); e->setHeadLine(headline); e->setBody(body); e->setRemedy(remedy); throw e; }


#endif // EXCEPTION_H

