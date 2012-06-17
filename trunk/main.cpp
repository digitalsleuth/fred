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

#include <QtGui/QApplication>
#include <QStringList>

#include <stdio.h>

#include "mainwindow.h"
#include "argparser.h"
#include "compileinfo.h"

int main(int argc, char *argv[]) {
  // Disable output buffering
  setbuf(stdout,NULL);

  // Init QApplication
  QApplication a(argc, argv);

#define PRINT_HEADER {                                        \
  printf("%s v%s %s\n\n",APP_NAME,APP_VERSION,APP_COPYRIGHT); \
}
#define PRINT_HEADER_AND_USAGE { \
  PRINT_HEADER;                  \
  args.PrintUsage();             \
}
#define PRINT_VERSION printf("%s\n",APP_VERSION);
#define PRINT_UNKNOWN_ARG_ERROR(s) {                          \
  PRINT_HEADER;                                               \
  printf("ERROR: Unknown command line argument '%s'!\n\n",s); \
  args.PrintUsage();                                          \
}

  // Parse command line args
  ArgParser args(a.arguments());
  if(!args.ParseArgs()) {
    PRINT_HEADER;
    printf("ERROR: %s\n\n",args.GetErrorMsg().toAscii().constData());
    args.PrintUsage();
    exit(1);
  }

  // Check command line args for correctness
  if(args.IsSet("dump-report")) {
    if(args.GetArgVal("dump-report")=="") {
      PRINT_HEADER;
      printf("ERROR: --dump-report specified without a report file!\n\n");
      args.PrintUsage();
      exit(1);
    }
    if(!args.IsSet("hive-file")) {
      PRINT_HEADER;
      printf("ERROR: --dump-report specified without a hive file!\n\n");
      args.PrintUsage();
      exit(1);
    }
  }

  // React on some command line args
  if(args.IsSet("?") || args.IsSet("h") || args.IsSet("help")) {
    PRINT_HEADER_AND_USAGE;
    exit(0);
  }
  if(args.IsSet("v") || args.IsSet("version")) {
    PRINT_VERSION;
    exit(0);
  }
  if(args.IsSet("dump-report")) {
    printf("Dumping report '%s' using hive '%s'\n",
           args.GetArgVal("dump-report").toAscii().constData(),
           args.GetArgVal("hive-file").toAscii().constData());

    // TODO: Open hive and dump report

    exit(0);
  }

#undef PRINT_UNKNOWN_ARG_ERROR
#undef PRINT_VERSION
#undef PRINT_HEADER_AND_USAGE
#undef PRINT_HEADER

  // Create and show main window
  MainWindow w(&args);
  w.show();

  return a.exec();
}
