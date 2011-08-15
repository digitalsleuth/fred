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

#include "datareporterengine.h"

#include <QString>

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
  // GetRegistryKeyValue
  QScriptValue func_get_key_value=this->newFunction(this->GetRegistryKeyValue);
  func_get_key_value.setData(this->newQObject(this->p_registry_hive));
  this->globalObject().setProperty("GetRegistryKeyValue",func_get_key_value);
  // RegistryKeyValueToString
  QScriptValue func_value_to_string=
    this->newFunction(this->RegistryKeyValueToString);
  this->globalObject().setProperty("RegistryKeyValueToString",
                                   func_value_to_string);
  // RegistryKeyTypeToString
  QScriptValue func_type_to_string=
    this->newFunction(this->RegistryKeyTypeToString);
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
    if(i>0) content.append(" ");
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
    if(i>0) content.append(" ");
    content.append(context->argument(i).toString());
  }

  qobject_cast<DataReporterEngine*>(engine)->
    report_content.append(content).append("\n");

  return engine->undefinedValue();
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
  // TODO: Don't know if this works
  s.value=obj.property("value").toVariant().toByteArray();
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
  if(p_hive->Error() || key_length==-1) {
    // Get error message ro clear error state
    p_hive->GetErrorMsg();
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

  // TODO: Does not work!!
  //key_value=qscriptvalue_cast<QByteArray>(context->argument(0));
  key_value=context->argument(0).toVariant().toByteArray();
  ret=RegistryHive::KeyValueToString(key_value,
                                     hive_t_REG_SZ /*context->argument(1).toInteger()*/);

  qDebug("Type: %u Sring: %c",
         context->argument(1).toInteger(),
         ret.toAscii().constData());

  return engine->newVariant(ret);
}

QScriptValue DataReporterEngine::RegistryKeyTypeToString(
  QScriptContext *context,
  QScriptEngine *engine)
{
  // This function needs one arguments, key type
  if(context->argumentCount()!=2) return engine->undefinedValue();
}
