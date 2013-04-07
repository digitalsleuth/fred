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

#ifndef DLGREPORTCHOOSER_H
#define DLGREPORTCHOOSER_H

#include "reports.h"

#include <QDialog>
#include <QTreeWidgetItem>

namespace Ui {
  class DlgReportChooser;
}

class DlgReportChooser : public QDialog {
  Q_OBJECT

  public:
    explicit DlgReportChooser(Reports *p_reps, QWidget *p_parent=0);
    ~DlgReportChooser();

  protected:
    void changeEvent(QEvent *e);

  private slots:
    void on_BtnCancel_clicked();

    void on_TrReports_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

  private:
    Ui::DlgReportChooser *ui;
    Reports *p_reports;
};

#endif // DLGREPORTCHOOSER_H
