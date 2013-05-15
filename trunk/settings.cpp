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
    #define SYSTEM_REPORT_TEMPLATE_DIR "/usr/share/fred/report_templates"
  #else
    #define SYSTEM_REPORT_TEMPLATE_DIR ".\\report_templates\\"
  #endif
#endif

#define APP_ORGANIZATION "pinguin.lu"
#define APP_NAME "fred"

/*******************************************************************************
 * Public
 ******************************************************************************/

Settings::Settings(QObject *p_parent) : QObject(p_parent) {
  // Init vars
  this->p_settings=NULL;
  this->initialized=false;
  this->user_settings_dir=QDir::homePath()
    .append(QDir::separator()).append(".fred");




  this->user_report_template_dir=QString(this->user_settings_dir)
                                   .append(QDir::separator())
                                   .append("report_templates");

  // Make sure config dirs exist
  if(!QDir(this->user_settings_dir).exists()) {
    // User config dir does not exists, try to create it
    if(!QDir().mkpath(this->user_settings_dir)) {
      // TODO: Maybe warn user
      return;
    }
  }
  if(!QDir(this->user_report_template_dir).exists()) {
    // Create config dir sub folder for report templates
    if(!QDir().mkpath(this->user_report_template_dir)) {
      // TODO: Maybe warn user
      return;
    }
  }

  // Create / open settings
#ifndef __MINGW32__
  // On any Unix-like OS, settings should be saved in the .fred folder
  this->p_settings=new QSettings(QString(this->user_settings_dir)
                                   .append(QDir::separator())
                                   .append("fred.conf"),
                                 QSettings::NativeFormat,
                                 this);
#else
  // On Windows, it can be stored inside registry
  this->p_settings=new QSettings(QSettings::NativeFormat,
                                 QSettings::UserScope,
                                 APP_ORGANIZATION,
                                 APP_NAME,
                                 this);
#endif
  if(this->p_settings->status()!=QSettings::NoError ||
     !this->p_settings->isWritable())
  {
    return;
  }

  this->initialized=true;
}

void Settings::Reset() {

}

void Settings::SetReportTemplateDirs(QStringList &dirs) {
  this->p_settings->setValue("ReportTemplateDirs",dirs);
}

QStringList Settings::GetReportTemplateDirs() {
  if(!this->initialized) return QStringList();
  return this->p_settings->value("ReportTemplateDirs",
                                 QStringList()<<SYSTEM_REPORT_TEMPLATE_DIR
                                              <<this->user_report_template_dir
                                ).toStringList();
}

void Settings::SaveWindowGeometry(QString window_name, QByteArray geometry) {
  if(!this->initialized) return;
  this->p_settings->setValue(QString("WindowGeometry_%1").arg(window_name),
                             geometry);
}

QByteArray Settings::GetWindowGeometry(QString window_name) {
  if(!this->initialized) return QByteArray();
  return this->p_settings->value(QString("WindowGeometry_%1")
                                   .arg(window_name)).toByteArray();
}

void Settings::AddRecentFile(QString file) {
  // Get recent files
  QStringList recent_files=this->GetRecentFiles();

  if(recent_files.contains(file)) {
    // File already exists in recent list. Simply move it to top
    recent_files.move(recent_files.indexOf(file),0);
  } else {
    // We only save 5 files at max
    if(recent_files.count()==5) recent_files.removeLast();
    recent_files.prepend(file);
  }

  this->p_settings->setValue("RecentFiles",recent_files);
}

QStringList Settings::GetRecentFiles() {
  return this->p_settings->value("RecentFiles").toStringList();
}
