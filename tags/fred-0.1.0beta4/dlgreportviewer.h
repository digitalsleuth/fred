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

#ifndef DLGREPORTVIEWER_H
#define DLGREPORTVIEWER_H

#include <QMainWindow>
#include <QEventLoop>
#include <QCloseEvent>

namespace Ui {
  class DlgReportViewer;
}

class DlgReportViewer : public QMainWindow {
  Q_OBJECT

  public:
    explicit DlgReportViewer(QString &report_data,
                             QWidget *p_parent=0);
    ~DlgReportViewer();

    void exec();

  protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *event);

  private slots:
    void on_action_Print_triggered();
    void on_action_Close_triggered();

private:
    Ui::DlgReportViewer *ui;
    QEventLoop *p_local_event_loop;

};

#endif // DLGREPORTVIEWER_H
