/*******************************************************************************
* fred Copyright (c) 2011-2012 by Gillen Daniel <gillen.dan@pinguin.lu>        *
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

#include "datareporterengine.h"

#include <QString>
#include <QMap>
#include <QMapIterator>
#include <QStringList>
#include <QDateTime>

#include <stdio.h>

DataReporterEngine::DataReporterEngine(RegistryHive *p_hive) : QScriptEngine() {
  // Init vars
  this->p_registry_hive=p_hive;
  this->report_content="";

  // Add our types to engine
  qScriptRegisterMetaType<s_RegistryKeyValue>(this,
                                              this->RegistryKeyValueToScript,
                                              this->RegistryKeyValueFromScript);
  this->p_type_byte_array=new ByteArray(this);
  this->globalObject().setProperty("ByteArray",
                                   this->p_type_byte_array->constructor());

  // Add our functions
  // print
  QScriptValue func_print=this->newFunction(this->Print);
  this->globalObject().setProperty("print",func_print);
  // println
  QScriptValue func_println=this->newFunction(this->PrintLn);
  this->globalObject().setProperty("println",func_println);
  // GetRegistryNodes
  QScriptValue func_get_nodes=this->newFunction(this->GetRegistryNodes,1);
  func_get_nodes.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryNodes",func_get_nodes);
  // GetRegistryKeys
  QScriptValue func_get_keys=this->newFunction(this->GetRegistryKeys,1);
  func_get_keys.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryKeys",func_get_keys);
  // GetRegistryKeyValue
  QScriptValue func_get_key_value=this->newFunction(this->GetRegistryKeyValue,
                                                    2);
  func_get_key_value.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryKeyValue",func_get_key_value);
  // GetRegistryKeyModTime
  QScriptValue func_get_key_modt=this->newFunction(this->GetRegistryKeyModTime,
                                                    2);
  func_get_key_modt.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryKeyModTime",func_get_key_modt);
  // RegistryKeyValueToString
  QScriptValue func_value_to_string=
    this->newFunction(this->RegistryKeyValueToString,2);
  this->globalObject().setProperty("RegistryKeyValueToString",
                                   func_value_to_string);
  // RegistryKeyValueToVariant
  QScriptValue func_value_to_variant=
    this->newFunction(this->RegistryKeyValueToVariant);
  this->globalObject().setProperty("RegistryKeyValueToVariant",
                                   func_value_to_variant);
  // RegistryKeyTypeToString
  QScriptValue func_type_to_string=
    this->newFunction(this->RegistryKeyTypeToString,1);
  this->globalObject().setProperty("RegistryKeyTypeToString",
                                   func_type_to_string);
}

DataReporterEngine::~DataReporterEngine() {
  delete this->p_type_byte_array;
}

QScriptValue DataReporterEngine::Print(QScriptContext *context,
                                       QScriptEngine *engine)
{
  int i;
  QString content;

  // Append all arguments to content
  for(i=0;i<context->argumentCount();++i) {
    //if(i>0) content.append(" ");
    content.append(context->argument(i).toString());
  }

  //QScriptValue calleeData=context->callee().data();
  //DataReporterEngine *engine=
  //  qobject_cast<DataReporterEngine*>(calleeData.toQObject());
  qobject_cast<DataReporterEngine*>(engine)->report_content.append(content);

  return engine->undefinedValue();
}

QScriptValue DataReporterEngine::PrintLn(QScriptContext *context,
                                         QScriptEngine *engine)
{
  int i;
  QString content;

  // Append all arguments to content
  for(i=0;i<context->argumentCount();++i) {
    //if(i>0) content.append(" ");
    content.append(context->argument(i).toString());
  }

  qobject_cast<DataReporterEngine*>(engine)->
    report_content.append(content).append("\n");

  return engine->undefinedValue();
}

/*
 * GetRegistryNodes
 */
QScriptValue DataReporterEngine::GetRegistryNodes(QScriptContext *context,
                                                  QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  QMap<QString,int> nodes;
  QScriptValue ret_nodes;
  int ii=0;

  // This function needs one argument, parent node path
  if(context->argumentCount()!=1) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  // Get nodes
  nodes=p_hive->GetNodes(context->argument(0).toString());
  if(p_hive->Error()) {
    // Clear error state
    p_hive->GetErrorMsg();
    return engine->undefinedValue();
  }

  // Build script array
  ret_nodes=engine->newArray(nodes.count());
  QMapIterator<QString,int> i(nodes);
  while(i.hasNext()) {
    i.next();
    ret_nodes.setProperty(ii++,QScriptValue(i.key()));
  }

  return ret_nodes;
}

/*
 * GetRegistryKeys
 */
QScriptValue DataReporterEngine::GetRegistryKeys(QScriptContext *context,
                                                 QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  QMap<QString,int> keys;
  QScriptValue ret_keys;
  int ii=0;

  // This function needs one argument, parent node path
  if(context->argumentCount()!=1) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  // Get keys
  keys=p_hive->GetKeys(context->argument(0).toString());
  if(p_hive->Error()) {
    // Clear error state
    p_hive->GetErrorMsg();
    return engine->undefinedValue();
  }

  //qDebug(QString("P: %1 A: %2").arg(context->argument(0).toString()).arg(keys.count()).toAscii().constData());

  // Build script array
  ret_keys=engine->newArray(keys.count());
  QMapIterator<QString,int> i(keys);
  while(i.hasNext()) {
    i.next();
    ret_keys.setProperty(ii++,QScriptValue(i.key()));
  }

  return ret_keys;
}

/*
 * RegistryKeyValueToScript
 */
QScriptValue DataReporterEngine::RegistryKeyValueToScript(QScriptEngine *engine,
                                                          const
                                                            s_RegistryKeyValue
                                                              &s)
{
  QScriptValue obj=engine->newObject();
  obj.setProperty("type",s.type);
  obj.setProperty("length",s.length);
  ByteArray *p_byte_array=new ByteArray(engine);
  obj.setProperty("value",p_byte_array->newInstance(s.value));
  return obj;
}

/*
 * RegistryKeyValueFromScriptValue
 */
void DataReporterEngine::RegistryKeyValueFromScript(const QScriptValue &obj,
                                                    s_RegistryKeyValue &s)
{
  s.type=obj.property("type").toInt32();
  s.length=obj.property("length").toInt32();
  // TODO: Don't know if this works, but it probably does ;)
  s.value=qvariant_cast<QByteArray>(obj.property("value").data().toVariant());
}

QScriptValue DataReporterEngine::GetRegistryKeyValue(QScriptContext *context,
                                                     QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  QByteArray key_value;
  int key_type=0;
  size_t key_length=0;
  s_RegistryKeyValue script_key_value;

  // This function needs two arguments, key path and key name
  if(context->argumentCount()!=2) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  // Get key value
  key_value=p_hive->GetKeyValue(context->argument(0).toString(),
                                context->argument(1).toString(),
                                &key_type,
                                &key_length);
  if(p_hive->Error()) {
    // Get error message to clear error state
    p_hive->GetErrorMsg();
//    printf("\nError: %s\n",p_hive->GetErrorMsg().toAscii().constData());
    return engine->undefinedValue();
  }

  // Save key value to s_RegistryKeyValue struct
  script_key_value.type=key_type;
  script_key_value.length=key_length;
  script_key_value.value=key_value;

  return DataReporterEngine::RegistryKeyValueToScript(engine,script_key_value);
}

QScriptValue DataReporterEngine::RegistryKeyValueToString(
  QScriptContext *context,
  QScriptEngine *engine)
{
  QByteArray key_value;
  QString ret="";

  // This function needs two arguments, key value and value type
  if(context->argumentCount()!=2) return engine->undefinedValue();

  // Cast ByteArray argument to QByteArray and convert
  key_value=qvariant_cast<QByteArray>(context->argument(0).data().toVariant());
  ret=RegistryHive::KeyValueToString(key_value,
                                     context->argument(1).toInt32());

  return engine->newVariant(ret);
}

QScriptValue DataReporterEngine::RegistryKeyValueToVariant(
  QScriptContext *context,
  QScriptEngine *engine)
{
  int offset=0;
  int length=-1;
  bool little_endian=true;
  QByteArray key_value;
  QString format="";
  QString ret="";

  // This function needs at least two arguments, key value and variant type,
  // and may have three optional arguments, offset, length and little_endian
  if(context->argumentCount()<2 || context->argumentCount()>5) {
    return engine->undefinedValue();
  }
  if(context->argumentCount()==3) {
    offset=context->argument(2).toInt32();
  }
  if(context->argumentCount()==4) {
    offset=context->argument(2).toInt32();
    length=context->argument(3).toInt32();
  }
  if(context->argumentCount()==5) {
    offset=context->argument(2).toInt32();
    length=context->argument(3).toInt32();
    little_endian=(context->argument(4).toInt32()==1);
  }

  // Cast ByteArray argument to QByteArray
  key_value=qvariant_cast<QByteArray>(context->argument(0).data().toVariant());
  format=context->argument(1).toString();

  ret=RegistryHive::KeyValueToString(key_value,format,offset,length,little_endian);

  return engine->newVariant(ret);
}

QScriptValue DataReporterEngine::RegistryKeyTypeToString(
  QScriptContext *context,
  QScriptEngine *engine)
{
  QString ret="";

  // This function needs one argument, key type
  if(context->argumentCount()!=1) return engine->undefinedValue();

  ret=RegistryHive::KeyTypeToString(context->argument(0).toInt32());

  return engine->newVariant(ret);
}

QScriptValue DataReporterEngine::GetRegistryKeyModTime(
  QScriptContext *context,
  QScriptEngine *engine)
{
  QScriptValue calleeData;
  RegistryHive *p_hive;
  int64_t mod_time=0;

  // This function needs two argument, key path and key name
  if(context->argumentCount()!=2) return engine->undefinedValue();

  // Get calle data (Pointer to RegistryHive class)
  calleeData=context->callee().data();
  p_hive=qobject_cast<RegistryHive*>(calleeData.toQObject());

  mod_time=p_hive->GetKeyModTime(context->argument(0).toString(),
                                 context->argument(1).toString());
  if(p_hive->Error()) {
    // Get error message to clear error state
    p_hive->GetErrorMsg();
    return engine->undefinedValue();
  }

  QDateTime date_time;
  date_time.setTimeSpec(Qt::UTC);
  date_time.setTime_t(RegistryHive::FiletimeToUnixtime(mod_time));

  return engine->newVariant(date_time.toString("yyyy/MM/dd hh:mm:ss"));
}
