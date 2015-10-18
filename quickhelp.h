/*
Jentos IDE.

Copyright 2014, Evgeniy Goroshkin.

See LICENSE.TXT for licensing terms.
*/

#ifndef QUICKHELP_H
#define QUICKHELP_H

#include "std.h"
#include "codeeditor.h"

class Highlighter;



class QuickHelp : public QObject {
    Q_OBJECT

public:
    QuickHelp(QObject *parent = 0);
    ~QuickHelp();

    QString _topic, ltopic, url, kind, module, descr, clazz, descrForInsert;
    QString sig;
    bool isGlobal, isKeyword;
    QString quick();
    QString hint();
    QStringList params;

    //------------
    static bool init(QString &maxPath );
    static QuickHelp* help( const QString &_topic );
    static QuickHelp* help2( const QString &descr );
    static QMap<QString, QuickHelp*>* map();
    static QMap<QString, QuickHelp*>* map2();//keys are descr
    static void insert( QuickHelp *help );

};

#endif // QUICKHELP_H
