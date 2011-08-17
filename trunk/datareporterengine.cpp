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
#include <QMap>
#include <QMapIterator>
#include <QStringList>
#include <QDateTime>

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

/*
  // Add RegistryHive object
  QScriptValue obj_registry_hive=this->newQObject(this->p_registry_hive);
  this->globalObject().setProperty("RegistryHive",obj_registry_hive);
*/
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
  QByteArray key_value;
  QString variant_type;
  int remaining_data_len;
  const char *p_data;
  QString ret="";

  // This function needs at least two arguments, key value and variant type,
  // and may have an optional third argument specifying an offset
  if(context->argumentCount()<2 || context->argumentCount()>3)
    return engine->undefinedValue();
  if(context->argumentCount()==3) offset=context->argument(2).toInt32();

  // Cast ByteArray argument to QByteArray
  key_value=qvariant_cast<QByteArray>(context->argument(0).data().toVariant());
  variant_type=context->argument(1).toString();

  // Calculate how many bytes are remainig after specified offset
  remaining_data_len=key_value.size()-offset;
  if(!remaining_data_len>0) {
    // Nothing to show
    return engine->undefinedValue();
  }

  // Get pointer to data at specified offset
  p_data=key_value.constData();
  p_data+=offset;

  // Convert
  if(variant_type=="int8" && remaining_data_len>=1) {
    ret=QString().sprintf("%d",*(int8_t*)p_data);
  } else if(variant_type=="uint8" && remaining_data_len>=1) {
    ret=QString().sprintf("%u",*(uint8_t*)p_data);
  } else if(variant_type=="int16" && remaining_data_len>=2) {
    ret=QString().sprintf("%d",*(int16_t*)p_data);
  } else if(variant_type=="uint16" && remaining_data_len>=2) {
    ret=QString().sprintf("%u",*(uint16_t*)p_data);
  } else if(variant_type=="int32" && remaining_data_len>=4) {
    ret=QString().sprintf("%d",*(int32_t*)p_data);
  } else if(variant_type=="uint32" && remaining_data_len>=4) {
    ret=QString().sprintf("%d",*(uint32_t*)p_data);
  } else if(variant_type=="unixtime" && remaining_data_len>=4) {
    if(*(uint32_t*)p_data==0) {
      ret="n/a";
    } else {
      QDateTime date_time;
      date_time.setTime_t(*(uint32_t*)p_data);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(variant_type=="filetime" && remaining_data_len>=8) {
    if(*(uint64_t*)p_data==0) {
      ret="n/a";
    } else {
      // TODO: Warn if >32bit
      QDateTime date_time;
      date_time.setTime_t((*(uint64_t*)p_data-116444736000000000)/10000000);
      ret=date_time.toString("yyyy/MM/dd hh:mm:ss");
    }
  } else if(variant_type=="ascii") {
    // TODO: This fails bad if the string is not null terminated!! It might be
    // wise checking for a null char here
    ret=QString().fromAscii((char*)p_data);
  } else if(variant_type=="utf16" && remaining_data_len>=2) {
    ret=QString().fromUtf16((ushort*)p_data);
  } else {
    // Unknown variant type or another error
    return engine->undefinedValue();
  }

  return engine->newVariant(ret);
}

QScriptValue DataReporterEngine::RegistryKeyTypeToString(
  QScriptContext *context,
  QScriptEngine *engine)
{
  QString ret="";

  // This function needs one arguments, key type
  if(context->argumentCount()!=1) return engine->undefinedValue();

  ret=RegistryHive::KeyTypeToString(context->argument(0).toInt32());

  return engine->newVariant(ret);
}
