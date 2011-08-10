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

#include <QDir>

#include "datareporter.h"
#include "reporttemplatexmlhandler.h"

DataReporter::DataReporter() {
  this->report_templates.clear();
}

DataReporter::~DataReporter() {
  qDeleteAll(this->report_templates);
}

void DataReporter::LoadReportTemplates() {
  QString report_template="";
  QXmlSimpleReader xml_parser;
  int i=0;
  ReportTemplate *p_report;

  QDir report_dir("../trunk/report_templates/");
  QStringList report_templates=report_dir.entryList(QStringList()<<"*.fred");

  for(i=0;i<report_templates.count();i++) {
    report_template=report_dir.path();
    report_template.append(QDir::separator());
    report_template.append(report_templates.value(i));

    QFile *p_report_template_file=new QFile(report_template);
    QXmlInputSource *p_xml_file=new QXmlInputSource(p_report_template_file);
    ReportTemplateXmlHandler *p_report_handler=new ReportTemplateXmlHandler();

    xml_parser.setContentHandler(p_report_handler);
    xml_parser.parse(p_xml_file);

    p_report=new ReportTemplate(p_report_handler->GetReportCategory(),
                                p_report_handler->GetReportName(),
                                report_template);
    this->report_templates.append(p_report);

    delete p_report_handler;
    delete p_xml_file;
    delete p_report_template_file;
  }
}

QStringList DataReporter::GetAvailableReportCategories() {
  QStringList ret;
  QString cat;
  int i=0;

  ret.clear();
  for(i=0;i<this->report_templates.count();i++) {
    cat=this->report_templates.value(i)->Category();
    if(ret.indexOf(cat)==-1) ret.append(cat);
  }

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

  return ret;
}

QString DataReporter::GenerateReport(hive_h *hhive,
                                     QString report_category,
                                     QString report_name)
{

}
