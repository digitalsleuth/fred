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

#ifndef REPORTS_H
#define REPORTS_H

#include "reporttemplate.h"
#include "reportengine.h"

#include <QList>
#include <QString>
#include <QStringList>

class Reports {
  public:
    Reports();
    ~Reports();

    void LoadReportTemplates(QString dir);
    QStringList GetAvailableReportCategories();
    QList<ReportTemplate*> GetAvailableReports(QString category);

    bool GenerateReport(RegistryHive *p_hive,
                        QString report_file,
                        QString &report_result,
                        bool console_mode=false);
    bool GenerateReport(RegistryHive *p_hive,
                        QList<ReportTemplate*> report_list,
                        QString &report_result,
                        bool console_mode=false);
  private:
    QList<ReportTemplate*> report_templates;
    ReportEngine *p_engine;
};

#endif // REPORTS_H
