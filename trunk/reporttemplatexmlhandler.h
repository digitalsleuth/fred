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

#ifndef REPORTTEMPLATEXMLHANDLER_H
#define REPORTTEMPLATEXMLHANDLER_H

#include <QtXml/QXmlDefaultHandler>
#include <QVariant>

#include <hivex.h>

class ReportTemplateData {
  public:
    ReportTemplateData() {
      this->vars.clear();
      this->data.clear();
    }
    ~ReportTemplateData() {
      qDeleteAll(this->data);
    }

    QList<QVariant> vars;
    QList<ReportTemplateData*> data;
};

class ReportTemplateXmlHandler : public QXmlDefaultHandler {

  public:
    explicit ReportTemplateXmlHandler(hive_h *hive_handle=NULL,
                                      bool only_get_info=true);
    ~ReportTemplateXmlHandler();

    bool startDocument();
    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);
    bool error(const QXmlParseException &exception);
    bool fatalError(const QXmlParseException &exception);

    QString GetReportCategory();
    QString GetReportName();
    QString ReportData();

  private:
    hive_h *hhive;
    bool get_info;
    QString report_category;
    QString report_name;
    QString report_content;
    QList<ReportTemplateData*> report_data;

    bool ProcessForEach(QString path,
                        QString vars,
                        hive_node_h cur_hive_node,
                        bool iterate=false);
    bool ProcessParagraph();
    bool ProcessValue();
};

#endif // REPORTTEMPLATEXMLHANDLER_H
