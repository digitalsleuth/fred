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

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <QString>
#include <QStringList>
#include <QHash>

class ArgParser {
  public:
    ArgParser(QStringList args);
    QString GetErrorMsg();
    bool ParseArgs();
    bool IsSet(QString arg);
    QString GetArgVal(QString arg);

  private:
    QStringList argv;
    int argc;
    QString error_msg;
    QHash<QString,QString> parsed_args;

    void SetError(QString msg=QString());
};

#endif // ARGPARSER_H
