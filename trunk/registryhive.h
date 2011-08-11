#ifndef REGISTRYHIVE_H
#define REGISTRYHIVE_H

#include <QObject>

#include <hivex.h>

class RegistryHive : public QObject {
  Q_OBJECT

  public:
    explicit RegistryHive(QObject *p_parent=0);

  signals:

  public slots:

};

#endif // REGISTRYHIVE_H
