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

#include "registrykey.h"

RegistryKey::RegistryKey(const QList<QVariant> &data) {
  this->key_data=data;
}

RegistryKey::~RegistryKey() {
  qDeleteAll(this->keys);
}

void RegistryKey::Append(RegistryKey *p_key) {
  this->keys.append(p_key);
}

RegistryKey* RegistryKey::Key(uint64_t row) {
  return this->keys.value(row);
}

uint64_t RegistryKey::RowCount() {
  return this->keys.count();
}

QVariant RegistryKey::Data(uint64_t column) const {
  if(column>=0 && column<3) {
    return this->key_data.value(column);
  } else {
    return QVariant();
  }
}

uint64_t RegistryKey::Row() const {
  return this->keys.indexOf(const_cast<RegistryKey*>(this));
}

