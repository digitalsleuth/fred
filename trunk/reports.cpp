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

#include "reports.h"

#include <QListIterator>
#include <QDir>
#include <QMap>

#include <QDebug>

Reports::Reports() {
  this->p_engine=new ReportEngine(NULL);
  this->report_templates.clear();
}

Reports::~Reports() {
  qDeleteAll(this->report_templates);
  delete this->p_engine;
}

void Reports::LoadReportTemplates(QString dir) {
  QString report_template="";
  QString report_category,report_name,report_author,report_desc,report_hive;
  bool found;
  int i;
  ReportTemplate *p_report;

  // Get all template files in report_templates directory
  QDir report_dir(dir);
  QStringList found_report_templates=report_dir.
    entryList(QStringList()<<"*.qs");

  QListIterator<QString> it(found_report_templates);
  while(it.hasNext()) {
    // Build path to template file
    report_template=report_dir.path();
    report_template.append(QDir::separator());
    report_template.append(it.next());

    // Get report info
    QMap<QString,QVariant> report_info=this->p_engine->
      GetReportTemplateInfo(report_template);
    if(report_info.contains("error")) {
      // TODO: Inform user
      qDebug()<<"Error in report '"<<report_template<<"'\n"
        <<report_info["error"].toString();
      continue;
    }

    // Make sure report is compatible with current API
    if(report_info.value("fred_api",QVariant(0)).toInt()>
       FRED_REPORTENGINE_API_VERSION)
    {
      // TODO: Inform user
      qDebug()<<"Report '"<<report_template<<"' is not compatible with current API!";
    }

    // Extract info values. If a value isn't present, it will be set to ""
    report_category=report_info.value("report_cat").toString();
    report_name=report_info.value("report_name").toString();
    report_author=report_info.value("report_author").toString();
    report_desc=report_info.value("report_desc").toString();
    report_hive=report_info.value("report_hive").toString();

    // Check if a report with the same category and name was already added
    found=false;
    for(i=0;i<this->report_templates.count();i++) {
      if(this->report_templates.at(i)->Category()==report_category &&
         this->report_templates.at(i)->Name()==report_name)
      {
        found=true;
        break;
      }
    }

    // Add to or update report template list
    if(!found) {
      // Add report to list
      p_report=new ReportTemplate(report_template,
                                  report_category,
                                  report_name,
                                  report_author,
                                  report_desc,
                                  report_hive);
      this->report_templates.append(p_report);
    } else {
      // Update report entry
      p_report=this->report_templates.at(i);
      p_report->SetFile(report_template);
      p_report->SetAuthor(report_author);
      p_report->SetDescription(report_desc);
    }
  }
}

QStringList Reports::GetAvailableReportCategories() {
  QStringList ret;
  QString cat;
  int i;

  ret.clear();
  for(i=0;i<this->report_templates.count();i++) {
    cat=this->report_templates.value(i)->Category();
    if(!ret.contains(cat)) ret.append(cat);
  }
  ret.sort();

  return ret;
}


QStringList Reports::GetAvailableReports(QString category) {
  QStringList ret;
  QString cat;
  int i=0;

  ret.clear();
  for(i=0;i<this->report_templates.count();i++) {
    cat=this->report_templates.value(i)->Category();
    if(cat==category) ret.append(this->report_templates.value(i)->Name());
  }
  ret.sort();

  return ret;
}

QMap<QString,QString> Reports::GetReportInfo(QString category, QString name) {
  QMap<QString,QString> ret;
  int i=0;

  // Search requested report
  for(i=0;i<this->report_templates.count();i++) {
    if(this->report_templates.value(i)->Category()==category &&
       this->report_templates.value(i)->Name()==name)
    {
      ret["report_author"]=this->report_templates.value(i)->Author();
      ret["report_desc"]=this->report_templates.value(i)->Description();
      break;
    }
  }

  return ret;
}
