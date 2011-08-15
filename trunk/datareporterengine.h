/*******************************************************************************
* fred Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>             *
*                                                                              *
* Forensic Registry EDitor (fred) is a cross-platform M$ registry hive editor  *
* with special feautures useful during forensic analysis.                      *
*                                                                              *
* This program is free software: you can redistribute it and/or modify it      *
* under the terms of the GNU General Public License as published by the Free   *
* Software Foundation, either version 3 of the License, or (at your option)    *
* any later version.                                                           *
*                                                                              *
* This program is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
* more details.                                                                *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* this program. If not, see <http://www.gnu.org/licenses/>.                    *
*******************************************************************************/

#ifndef DATAREPORTERENGINE_H
#define DATAREPORTERENGINE_H

#include <QObject>
#include <QString>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptContext>

#include "registryhive.h"
#include "qtscript_types/bytearray.h"

class DataReporterEngine : public QScriptEngine {
  Q_OBJECT

  public:
    struct s_RegistryKeyValue {
      int type;
      int length;
      QByteArray value;
    };

    RegistryHive *p_registry_hive;
    QString report_content;

    DataReporterEngine(RegistryHive *p_hive);
    ~DataReporterEngine();

  private:
    ByteArray *p_type_byte_array;

    static QScriptValue Print(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue PrintLn(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue RegistryKeyValueToScript(QScriptEngine *engine,
                                                 const s_RegistryKeyValue &s);
    static void RegistryKeyValueFromScript(const QScriptValue &obj,
                                           s_RegistryKeyValue &s);
    static QScriptValue GetRegistryKeyValue(QScriptContext *context,
                                            QScriptEngine *engine);
    static QScriptValue RegistryKeyValueToString(QScriptContext *context,
                                                 QScriptEngine *engine);
    static QScriptValue RegistryKeyTypeToString(QScriptContext *context,
                                                QScriptEngine *engine);
};

Q_DECLARE_METATYPE(DataReporterEngine::s_RegistryKeyValue)

#endif // DATAREPORTERENGINE_H
