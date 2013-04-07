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

#include "dlgreportchooser.h"
#include "ui_dlgreportchooser.h"

#include <QString>
#include <QStringList>
#include <QListIterator>

DlgReportChooser::DlgReportChooser(Reports *p_reps, QWidget *p_parent)
  : QDialog(p_parent), ui(new Ui::DlgReportChooser)
{
  QTreeWidgetItem *p_category;
  QTreeWidgetItem *p_category_report;

  this->ui->setupUi(this);

  // Save values for later use
  this->p_reports=p_reps;

  // Populate tree with reports
  QStringList report_cats=this->p_reports->GetAvailableReportCategories();
  QListIterator<QString> cat_it(report_cats);
  QString cur_cat;
  while(cat_it.hasNext()) {
    cur_cat=cat_it.next();
    p_category=new QTreeWidgetItem(this->ui->TrReports);
    p_category->setText(0,cur_cat);
    QStringList reports=this->p_reports->GetAvailableReports(cur_cat);
    QListIterator<QString> rep_it(reports);
    while(rep_it.hasNext()) {
      p_category_report=new QTreeWidgetItem(p_category);
      p_category_report->setText(0,rep_it.next());
      p_category_report->setFlags(Qt::ItemIsEnabled|
                                  Qt::ItemIsSelectable|
                                  Qt::ItemIsUserCheckable);
      p_category_report->setCheckState(0,Qt::Unchecked);
    }
  }
}

DlgReportChooser::~DlgReportChooser() {
  delete this->ui;
}

void DlgReportChooser::changeEvent(QEvent *e) {
  QDialog::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      this->ui->retranslateUi(this);
      break;
    default:
      break;
  }
}

void DlgReportChooser::on_BtnCancel_clicked() {
  this->reject();
}

void DlgReportChooser::on_TrReports_currentItemChanged(QTreeWidgetItem *current,
                                                       QTreeWidgetItem *previous)
{
  QString category,name;

  if(current->parent()==NULL) return;

  category=current->parent()->text(0);
  name=current->text(0);
  QMap<QString,QString> report_infos=
    this->p_reports->GetReportInfo(category,name);
  this->ui->LblAuthor->setText(tr("Author: %1")
                                 .arg(report_infos["report_author"]));
  this->ui->LblDesc->setText(tr("Description:\n\n %1")
                               .arg(report_infos["report_desc"]));
}
