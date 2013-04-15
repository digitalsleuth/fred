/*******************************************************************************
* fred Copyright (c) 2011-2013 by Gillen Daniel <gillen.dan@pinguin.lu>        *
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

#include "settings.h"

#include <QDir>

#ifndef FRED_REPORT_TEMPLATE_DIR
  #ifndef __MINGW32__
    #define SYSTEM_REPORT_TEMPLATE_DIR "/usr/share/fred/report_templates/"
  #else
    #define SYSTEM_REPORT_TEMPLATE_DIR ".\\report_templates\\"
  #endif
#endif

#define APP_ORGANIZATION "pinguin.lu"
#define APP_NAME "fred"

Settings::Settings(QObject *p_parent) : QObject(p_parent) {
  // Init vars
  this->p_settings=NULL;
  this->user_settings_dir=QDir::homePath()
    .append(QDir::separator()).append(".fred");
  this->user_report_template_dir=QString(this->user_settings_dir)
                                   .append(QDir::separator())
                                   .append("report_templates");
}

bool Settings::Init() {
  // Make sure config dirs exist
  if(!QDir(this->user_settings_dir).exists()) {
    // User config dir does not exists, try to create it
    if(!QDir().mkpath(this->user_settings_dir)) {
      // TODO: Maybe warn user
      return false;
    }
  }
  if(!QDir(this->user_report_template_dir).exists()) {
    // Create config dir sub folder for report templates
    if(!QDir().mkpath(this->user_report_template_dir)) {
      // TODO: Maybe warn user
      return false;
    }
  }

#ifndef __MINGW32__
  // On any Unix-like OS, settings should be saved in the .fred folder
  QSettings::setPath(QSettings::NativeFormat,
                     QSettings::UserScope,
                     this->user_settings_dir);
#endif

  // Create / open settings
  this->p_settings=new QSettings(QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 APP_ORGANIZATION,
                                 APP_NAME,
                                 this);
  if(this->p_settings->status()!=QSettings::NoError ||
     !this->p_settings->isWritable())
  {
    return false;
  }

  return true;
}

QStringList Settings::GetReportTemplateDirs() {
  return QStringList()<<SYSTEM_REPORT_TEMPLATE_DIR
                     <<this->user_report_template_dir;
}

