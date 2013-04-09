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
    #define FRED_REPORT_TEMPLATE_DIR "/usr/share/fred/report_templates/"
  #else
    #define FRED_REPORT_TEMPLATE_DIR ".\\report_templates\\"
  #endif
#endif

#define APP_ORGANIZATION "pinguin.lu"
#define APP_NAME "fred"

settings::settings(QObject *p_parent) : QObject(p_parent) {
  // Init vars
  this->p_settings=NULL;
  this->user_settings_dir=QDir::homePath()
    .append(QDir::separator()).append(".fred");
}

bool settings::Init() {
  // Make sure config dir exists
  if(!QDir(this->user_settings_dir).exists()) {
    // User config dir does not exists, try to create it
    if(!QDir().mkpath(this->user_settings_dir)) {
      // TODO: Maybe warn user
      return false;
    }
    // Create config dir sub folder for report templates
    user_config_dir.append(QDir::separator()).append("report_templates");
    if(!QDir().mkpath(user_config_dir)) {
      // TODO: Maybe warn user
      return false;
    }
  }

  this->p_settings=new QSettings(APP_ORGANIZATION,APP_NAME,this);
}


