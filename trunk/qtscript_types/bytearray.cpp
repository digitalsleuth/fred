/*******************************************************************************
* Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>                  *
*                                                                              *
* Derived from code by Nokia Corporation and/or its subsidiary(-ies) under a   *
* compatible license:                                                          *
*                                                                              *
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).            *
* All rights reserved.                                                         *
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

#include <QtScript/QScriptEngine>

#include "bytearray.h"
#include "bytearrayiterator.h"
#include "bytearrayprototype.h"

#include <stdlib.h>

Q_DECLARE_METATYPE(QByteArray*)
Q_DECLARE_METATYPE(ByteArray*)

static qint32 toArrayIndex(const QString &str) {
  QByteArray bytes = str.toUtf8();
  char *eptr;

  quint32 pos = strtoul(bytes.constData(), &eptr, 10);
  if((eptr == bytes.constData() + bytes.size()) &&
     (QByteArray::number(pos) == bytes))
  {
    return pos;
  }

  return -1;
}

ByteArray::ByteArray(QScriptEngine *engine)
  : QObject(engine), QScriptClass(engine)
{
  qScriptRegisterMetaType<QByteArray>(engine,
                                      this->toScriptValue,
                                      this->fromScriptValue);

  length=engine->toStringHandle(QLatin1String("length"));

  this->proto=engine->newQObject(new ByteArrayPrototype(this),
                                 QScriptEngine::QtOwnership,
                                 QScriptEngine::SkipMethodsInEnumeration |
                                   QScriptEngine::ExcludeSuperClassMethods |
                                   QScriptEngine::ExcludeSuperClassProperties);
  QScriptValue global=engine->globalObject();
  this->proto.setPrototype(global.property("Object").property("prototype"));

  this->ctor=engine->newFunction(construct);
  this->ctor.setData(qScriptValueFromValue(engine,this));
}

ByteArray::~ByteArray() {}

QScriptClass::QueryFlags ByteArray::queryProperty(const QScriptValue &object,
                                                  const QScriptString &name,
                                                  QueryFlags flags,
                                                  uint *id)
{
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object.data());

  if(!ba) return 0;
  if(name!=length) {
    qint32 pos=toArrayIndex(name);
    if(pos==-1) return 0;
    *id=pos;
    if((flags & HandlesReadAccess) && (pos>=ba->size()))
      flags &= ~HandlesReadAccess;
  }

  return flags;
}

QScriptValue ByteArray::property(const QScriptValue &object,
                                 const QScriptString &name,
                                 uint id)
{
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object.data());
  if(!ba) return QScriptValue();
  if(name==length) return ba->length();
  else {
    qint32 pos=id;
    if((pos < 0) || (pos >= ba->size())) return QScriptValue();
    return uint(ba->at(pos)) & 255;
  }

  return QScriptValue();
}

void ByteArray::setProperty(QScriptValue &object,
                            const QScriptString &name,
                            uint id,
                            const QScriptValue &value)
{
  QByteArray *ba=qscriptvalue_cast<QByteArray*>(object.data());
  if(!ba) return;
  if(name==length) ba->resize(value.toInt32());
  else {
    qint32 pos=id;
    if(pos<0) return;
    if(ba->size()<=pos) ba->resize(pos + 1);
    (*ba)[pos]=char(value.toInt32());
  }
}

QScriptValue::PropertyFlags ByteArray::propertyFlags(const QScriptValue &object,
                                                     const QScriptString &name,
                                                     uint id)
{
  Q_UNUSED(object);
  Q_UNUSED(id);

  if(name==length) {
    return QScriptValue::Undeletable | QScriptValue::SkipInEnumeration;
  }
  return QScriptValue::Undeletable;
}

QScriptClassPropertyIterator *ByteArray::newIterator(const QScriptValue &object)
{
  return new ByteArrayIterator(object);
}

QString ByteArray::name() const {
  return QLatin1String("ByteArray");
}

QScriptValue ByteArray::prototype() const {
  return this->proto;
}

QScriptValue ByteArray::constructor() {
  return this->ctor;
}

QScriptValue ByteArray::newInstance(int size) {
  return newInstance(QByteArray(size,0));
}

QScriptValue ByteArray::newInstance(const QByteArray &ba) {
  QScriptValue data=engine()->newVariant(qVariantFromValue(ba));
  return engine()->newObject(this,data);
}

QScriptValue ByteArray::construct(QScriptContext *ctx, QScriptEngine *) {
  ByteArray *cls=qscriptvalue_cast<ByteArray*>(ctx->callee().data());
  if(!cls) return QScriptValue();
  int size=ctx->argument(0).toInt32();
  return cls->newInstance(size);
}

QScriptValue ByteArray::toScriptValue(QScriptEngine *eng, const QByteArray &ba)
{
  QScriptValue ctor=eng->globalObject().property("ByteArray");
  ByteArray *cls=qscriptvalue_cast<ByteArray*>(ctor.data());
  if(!cls) return eng->newVariant(qVariantFromValue(ba));
  return cls->newInstance(ba);
}

void ByteArray::fromScriptValue(const QScriptValue &obj, QByteArray &ba) {
  ba=qscriptvalue_cast<QByteArray>(obj.data());
}
