#ifndef HELPERS_H
#define HELPERS_H

#include <QPair>
#include <QVariantMap>

typedef QPair<QString,QString> Attribute;
typedef QList<Attribute> AttributeList;
namespace JsonHelpers
{
    void ensureFieldsInMap(QVariantMap map, AttributeList attributes);
}


#endif // HELPERS_H

