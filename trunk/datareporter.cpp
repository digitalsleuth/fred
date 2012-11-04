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

  // Search report template
  for(i=0;i<this->report_templates.count();i++) {
    p_report=this->report_templates.value(i);
    if(p_report->Category()!=report_category || p_report->Name()!=report_name) {
      continue;
    }
    // Report template was found, now generate report and return result
    return this->GenerateReport(p_hive,p_report->File());
  }

  // Report template couldn't be found
  QMessageBox::critical(0,
                        "Report engine error",
                        QString("Unable to find report with name '%1' in category '%2'!")
                          .arg(report_name)
                          .arg(report_category));
  return QString();
}

QString DataReporter::GenerateReport(RegistryHive *p_hive,
                                     QString report_template,
                                     bool console_mode)
{
  QString report_code;

  // Init data reporter engine
  DataReporterEngine engine(p_hive);
  QScriptValue hive_value=engine.newQObject(p_hive);
  engine.globalObject().setProperty("RegistryHive",hive_value);

  // Open report template
  QFile template_file(report_template);
  if(!template_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    if(!console_mode) {
      QMessageBox::critical(0,
                            "Report engine error",
                            QString("Couldn't open report template file '%1'!")
                                   .arg(report_template));
    } else {
      printf("ERROR: Couldn't open report template file '%s'!\n",
             report_template.toAscii().constData());
    }
    return QString();
  }

  // Read template file
  QTextStream in(&template_file);
  while(!in.atEnd()) report_code.append(in.readLine()).append("\n");

  // Close report template file
  template_file.close();

  // Execute report template script
  QScriptValue report_result=engine.evaluate(report_code,report_template);
  if (report_result.isError() || engine.hasUncaughtException()) {
    if(!console_mode) {
      QMessageBox::critical(0,
                            "Report engine error",
                            QString::fromLatin1("File: %0\n Line: %1\nError: %2")
                                   .arg(report_template)
                                   .arg(report_result.property("lineNumber")
                                        .toInt32())
                                   .arg(report_result.toString()));
    } else {
      printf("ERROR: %s:%u: %s\n",
             report_template.toAscii().constData(),
             report_result.property("lineNumber").toInt32(),
             report_result.toString().toAscii().constData());
    }
    return QString();
  }

  return engine.report_content;
}
