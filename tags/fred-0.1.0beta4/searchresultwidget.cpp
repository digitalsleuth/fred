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

#include "searchresultwidget.h"

#include <QHeaderView>
#include <QAbstractItemView>
#include <QStringList>
#include <QTableWidgetItem>
#include <QFontMetrics>

SearchResultWidget::SearchResultWidget(QWidget *p_parent)
  : QTableWidget(p_parent)
{
  this->setColumnCount(3);
  this->setRowCount(0);
  this->setTextElideMode(Qt::ElideNone);
  this->verticalHeader()->setHidden(true);
  this->setSelectionBehavior(QAbstractItemView::SelectRows);
  this->setSelectionMode(QAbstractItemView::SingleSelection);
  this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  this->setHorizontalHeaderLabels(QStringList()<<tr("Path")
                                               <<tr("Match type")
                                               <<tr("Match text"));
}

void SearchResultWidget::SlotFoundMatch(ThreadSearch::eMatchType match_type,
                                        QString path,
                                        QString key,
                                        QString value)
{
  QTableWidgetItem *p_item;

  QString full_path;
  QString type;
  QString match;

  switch(match_type) {
    case ThreadSearch::eMatchType_NodeName:
      type=tr("Node name");
      full_path=path;
      match=key;
      break;
    case ThreadSearch::eMatchType_KeyName:
      type=tr("Key name");
      full_path=path;
      match=key;
      break;
    case ThreadSearch::eMatchType_KeyValue:
      type=tr("Key value");
      full_path=path+"\\"+key;
      match=value;
      break;
  }

  int rows=this->rowCount();
  this->setRowCount(rows+1);
  // TODO: Use setCellWidget to add QTextEdit and then use insertText and
  // insertHtml to format match
  p_item=new QTableWidgetItem(full_path=="" ? "\\" : full_path);
  p_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  this->setItem(rows,0,p_item);
  p_item=new QTableWidgetItem(type);
  p_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  this->setItem(rows,1,p_item);
  p_item=new QTableWidgetItem(match);
  p_item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  this->setItem(rows,2,p_item);
}

int SearchResultWidget::sizeHintForColumn(int column) const {
  int size_hint=0;
  int i=0;
  int item_width=0;
  QFontMetrics fm(this->fontMetrics());

  // Find string that needs the most amount of space
  for(i=0;i<this->rowCount();i++) {
    item_width=fm.width(this->item(i,column)->text())+10;
    if(item_width>size_hint) size_hint=item_width;
  }

  return size_hint;
}

void SearchResultWidget::SlotSearchFinished() {
  this->resizeColumnsToContents();
  this->resizeRowsToContents();
}
