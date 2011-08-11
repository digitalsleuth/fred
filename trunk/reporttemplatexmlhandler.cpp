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

#include <stdlib.h>

#include "reporttemplatexmlhandler.h"

ReportTemplateXmlHandler::ReportTemplateXmlHandler(hive_h *hive_handle,
                                                   bool only_get_info)
  : QXmlDefaultHandler()
{
  this->hhive=hive_handle;
  this->get_info=only_get_info;
}

ReportTemplateXmlHandler::~ReportTemplateXmlHandler() {
  qDeleteAll(this->report_data);
}

bool ReportTemplateXmlHandler::startDocument() {
  this->report_category="";
  this->report_name="";
  this->report_content="";
  this->report_data.clear();
  return true;
}

bool ReportTemplateXmlHandler::startElement(const QString &namespaceURI,
                                            const QString &localName,
                                            const QString &qName,
                                            const QXmlAttributes &atts)
{
  Q_UNUSED(namespaceURI);
  Q_UNUSED(localName);
  int i=0;
  bool ok;

  // If we should only extract report info...
  if(this->get_info) {
    if(qName=="report") {
      this->report_category=atts.value(QString("category"));
      this->report_name=atts.value(QString("name"));
    }
    return true;
  }

  // Check element
  if(qName=="foreach") {
    ok=this->ProcessForEach(atts.value(QString("path")),
                            atts.value(QString("vars")),
                            hivex_root(this->hhive));
    if(!ok) return false;
  } else if (qName=="paragraph") {

  } else if (qName=="value") {

  }

  /*
  qDebug("%s",QString("--> %3").arg(qName).toAscii().constData());
  for(i=0;i<atts.count();i++) {
    qDebug("%s",QString("----> Name: '%1'', Value: '%2'").arg(atts.qName(i),atts.value(i)).toAscii().constData());
  }
  */

  return true;
}

bool ReportTemplateXmlHandler::endElement(const QString &namespaceURI,
                                          const QString &localName,
                                          const QString &qName)
{
  Q_UNUSED(namespaceURI);
  Q_UNUSED(localName);
  if(this->get_info) return true;

  if(qName=="foreach") {

  } else if (qName=="paragraph") {

  } else if (qName=="value") {

  }

  return true;
}

bool ReportTemplateXmlHandler::error(const QXmlParseException &exception) {
  qDebug("error: %u: %s",exception.lineNumber(),exception.message().toAscii().constData());
  return false;
}

bool ReportTemplateXmlHandler::fatalError(const QXmlParseException &exception) {
  qDebug("fatalerror: %u: %s",exception.lineNumber(),exception.message().toAscii().constData());
  return false;
}

QString ReportTemplateXmlHandler::GetReportCategory() {
  return this->report_category;
}

QString ReportTemplateXmlHandler::GetReportName() {
  return this->report_name;
}

QString ReportTemplateXmlHandler::ReportData() {
  return QString();
}

bool ReportTemplateXmlHandler::ProcessForEach(QString path,
                                              QString vars,
                                              hive_node_h cur_hive_node,
                                              bool iterate) {
  int i=0,ii=0;
  bool ok;

  if(!iterate) {
    QStringList nodes=path.split('/');
    if(cur_hive_node==0) return false;

    for(i=0;i<nodes.count();i++) {
      if(nodes.value(i)!="*") {
        cur_hive_node=hivex_node_get_child(this->hhive,
                                           cur_hive_node,
                                           nodes.value(i).toAscii().
                                             constData());
        if(cur_hive_node==0) return false;
      } else {
        QString new_path="";
        for(ii=i+1;ii<nodes.count();ii++) {
          new_path.append(nodes.value(ii)).append("/");
        }
        new_path.chop(1);
        ok=this->ProcessForEach(new_path,vars,cur_hive_node,true);
        if(!ok) return false;
      }
    }
  } else {
    hive_node_h *p_child_nodes=hivex_node_children(this->hhive,cur_hive_node);
    i=0;
    while(p_child_nodes[i]) {
      // Save iterator node
      ReportTemplateData *p_data;
      char *p_node_name=hivex_node_name(this->hhive,p_child_nodes[i]);
      // TODO: Check for NULL value
      p_data->vars.append(p_node_name);
      // TODO: Add custom vars
      QStringList var_pairs=vars.split(';');
      for(ii=0;ii<var_pairs.count();ii++) {
        QString var_name=var_pairs.value(ii).split(',').value(0);
        QString var_type=var_pairs.value(ii).split(',').value(1);
        QVariant var_content="";

        if(var_name=="(default)") var_name="";

        int type=0;
        size_t len=0;

        hive_value_h hive_key=hivex_node_get_value(this->hhive,
                                                   cur_hive_node,
                                                   var_name.toAscii().constData());

        if(var_type=="type") {
          char *data=hivex_value_value(this->hhive,hive_key,(hive_type*)&type,&len);
          var_content=QString().sprintf("%08u",type);

          qDebug("Save var '%s' with content '%08u'",var_name.toAscii().constData(),type);

          free(data);
        } else if(var_type=="value") {
          char *data=hivex_value_value(this->hhive,hive_key,(hive_type*)&type,&len);
          var_content=QByteArray(data);
        }
      }


      this->report_data.append(p_data);
      i++;
    }
    free(p_child_nodes);
  }

  return true;
}

bool ReportTemplateXmlHandler::ProcessParagraph() {
  return true;
}

bool ReportTemplateXmlHandler::ProcessValue() {
  return true;
}
