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

#include "datareporter.h"

#include <QDir>
#include <QTextStream>
#include <QtScript/QScriptEngine>
#include <QMessageBox>

DataReporter::DataReporter() {
  this->report_templates.clear();
  //this->p_report_engine=new DataReporterEngine();
}

DataReporter::~DataReporter() {
  //delete this->p_report_engine;
  qDeleteAll(this->report_templates);
}

void DataReporter::LoadReportTemplates(QString dir) {
  QString report_template="";
  int i=0;
  int ii=0;
  bool found=false;
  QString report_category="";
  QString report_name="";
  ReportTemplate *p_report;

  // Get all template files in report_templates directory
  QDir report_dir(dir);
  QStringList found_report_templates=report_dir.
    entryList(QStringList()<<"*.qs");

  for(i=0;i<found_report_templates.count();i++) {
    // Build complete path to template file
    report_template=report_dir.path();
    report_template.append(QDir::separator());
    report_template.append(found_report_templates.value(i));

    // Extract report category and name from file name (<category>_<name>.qs)
    report_category=found_report_templates.value(i).left(
      found_report_templates.value(i).indexOf("_"));
    report_name=found_report_templates.value(i).mid(
      found_report_templates.value(i).indexOf("_")+1);
    report_name=report_name.left(report_name.lastIndexOf("."));

    // Check if a report with the same category/name was already added
    found=false;
    for(ii=0;ii<this->report_templates.count();ii++) {
      if(this->report_templates.at(ii)->Category()==report_category &&
         this->report_templates.at(ii)->Name()==report_name)
      {
        found=true;
        break;
      }
    }

    if(!found) {
      // Add report to list
      p_report=new ReportTemplate(report_category,
                                  report_name,
                                  report_template);
      this->report_templates.append(p_report);
    } else {
      // Update report entry
      p_report=this->report_templates.at(ii);
      p_report->SetFile(report_template);
    }
  }
}

QStringList DataReporter::GetAvailableReportCategories() {
  QStringList ret;
  QString cat;
  int i=0;

  ret.clear();
  for(i=0;i<this->report_templates.count();i++) {
    cat=this->report_templates.value(i)->Category();
    if(!ret.contains(cat)) ret.append(cat);
  }
  ret.sort();

  return ret;
}

QStringList DataReporter::GetAvailableReports(QString category) {
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

QString DataReporter::GenerateReport(RegistryHive *p_hive,
                                     QString report_category,
                                     QString report_name)
{
  int i=0;
  ReportTemplate *p_report;
  DataReporterEngine engine(p_hive);
  QString report_code;
  //ReportData report_data;

  for(i=0;i<this->report_templates.count();i++) {
    p_report=this->report_templates.value(i);
    if(p_report->Category()!=report_category || p_report->Name()!=report_name) {
      continue;
    }

    QScriptValue hive_value=engine.newQObject(p_hive);
    engine.globalObject().setProperty("RegistryHive",hive_value);
    //QScriptValue return_value=engine.newQObject(&report_data);
    //engine.globalObject().setProperty("ReportData",return_value);

    // Open report template
    QFile template_file(p_report->File());
    if(!template_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug("Couldn't open file '%s'",p_report->File().toAscii().constData());
      return QString();
    }
    // Read template file
    QTextStream in(&template_file);
    while(!in.atEnd()) {
      report_code.append(in.readLine()).append("\n");
    }
    // Close report template file
    template_file.close();

    QScriptValue report_result=engine.evaluate(report_code,p_report->File());

    if (report_result.isError() || engine.hasUncaughtException()) {
      QMessageBox::critical(0,
                            "Report engine error",
                            QString::fromLatin1("%0:%1: %2")
                                   .arg(p_report->File())
                                   .arg(report_result.property("lineNumber").toInt32())
                                   .arg(report_result.toString()));
      return QString();
    }

    return engine.report_content;
  }

  return QString();
}
