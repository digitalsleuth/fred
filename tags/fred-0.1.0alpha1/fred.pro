#*******************************************************************************
# fred Copyright (c) 2011 by Gillen Daniel <gillen.dan@pinguin.lu>             *
#                                                                              *
# Forensic Registry EDitor (fred) is a M$ registry hive editor for Linux with  *
# special feautures useful during forensic analysis.                           *
#                                                                              *
# This program is free software: you can redistribute it and/or modify it      *
# under the terms of the GNU General Public License as published by the Free   *
# Software Foundation, either version 3 of the License, or (at your option)    *
# any later version.                                                           *
#                                                                              *
# This program is distributed in the hope that it will be useful, but WITHOUT  *
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for     *
# more details.                                                                *
#                                                                              *
# You should have received a copy of the GNU General Public License along with *
# this program. If not, see <http://www.gnu.org/licenses/>.                    *
#******************************************************************************/

system(bash compileinfo.sh > compileinfo.h)

QT       += core \
            gui

TARGET    = fred

TEMPLATE  = app


SOURCES  += main.cpp\
            mainwindow.cpp \
            registrynode.cpp \
            registrynodetreemodel.cpp \
            registrykey.cpp \
            registrykeytablemodel.cpp \
    dlgabout.cpp \
    dlgkeydetails.cpp \
    qhexedit/qhexedit_p.cpp \
    qhexedit/qhexedit.cpp

HEADERS  += mainwindow.h \
            registrynode.h \
            registrynodetreemodel.h \
            registrykey.h \
            registrykeytablemodel.h \
    dlgabout.h \
    dlgkeydetails.h \
    qhexedit/qhexedit_p.h \
    qhexedit/qhexedit.h

FORMS    += mainwindow.ui \
    dlgabout.ui \
    dlgkeydetails.ui

LIBS     += -lhivex

RESOURCES += \
    fred.qrc
