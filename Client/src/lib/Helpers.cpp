#include "Helpers.h"

#include <QDebug>

#include "Exception.h"

void JsonHelpers::ensureFieldsInMap(QVariantMap map, AttributeList attributes)
{
    foreach(Attribute a, attributes)
    {
        bool ok;
        if(a.second == "string")
        {
            ok = map.contains(a.first);
        }

        if(a.second == "int")
        {
            map[a.first].toInt(&ok);
        }

        if( !ok )
        {
            ThrowFatal(
                    QObject::tr("Server is talking gibberish"),
                    QObject::tr("Response lacked property '%1' of type '%2'").arg(a.first).arg(a.second))
        }
    }
}
