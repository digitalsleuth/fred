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

#include "reporttemplatexmlhandler.h"

ReportTemplateXmlHandler::ReportTemplateXmlHandler(bool only_get_info)
  : QXmlDefaultHandler()
{
  this->get_info=only_get_info;
}

bool ReportTemplateXmlHandler::startDocument() {
  this->report_category="";
  this->report_name="";
}

bool ReportTemplateXmlHandler::startElement(const QString &namespaceURI,
                                            const QString &localName,
                                            const QString &qName,
                                            const QXmlAttributes &atts)
{
  int i=0;

  // If we should only extract report info...
  if(this->get_info) {
    if(qName=="report") {
      this->report_category=atts.value(QString("category"));
      this->report_name=atts.value(QString("name"));
    }
    return true;
  }

  qDebug("%s",QString("--> %3").arg(qName).toAscii().constData());
  for(i=0;i<atts.count();i++) {
    qDebug("%s",QString("----> Name: '%1'', Value: '%2'").arg(atts.qName(i),atts.value(i)).toAscii().constData());
  }

  return true;
}

bool ReportTemplateXmlHandler::endElement(const QString &namespaceURI,
                                          const QString &localName,
                                          const QString &qName)
{
  return true;
}

bool ReportTemplateXmlHandler::error(const QXmlParseException &exception) {
  return false;
}

bool ReportTemplateXmlHandler::fatalError(const QXmlParseException &exception) {
  return false;
}

QString ReportTemplateXmlHandler::GetReportCategory() {
  return this->report_category;
}

QString ReportTemplateXmlHandler::GetReportName() {
  return this->report_name;
}
