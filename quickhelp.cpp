/*
Jentos IDE.

Copyright 2014, Evgeniy Goroshkin.

See LICENSE.TXT for licensing terms.
*/

#include "quickhelp.h"

QuickHelp::QuickHelp(QObject *parent) :
    QObject(parent)
{
    isKeyword = isGlobal = false;
    _topic = ltopic = url = kind = module = descr = clazz = descrForInsert = "";
}

QuickHelp::~QuickHelp(){
}


bool QuickHelp::init( QString &maxPath ) {

    map()->clear();
    map2()->clear();


    QFile commands(maxPath + "/docs/html/Modules/commands.txt");

    if (!commands.exists()) {
        return false;
    }

    if (commands.open(QIODevice::ReadOnly)) {
        QTextStream stream(&commands);
        while (!stream.atEnd()) {
            QString line = stream.readLine();



            int index = line.indexOf("|");

            QString left;
            QString url;

            if (index >= 0) {
                left = line.left(index);
                url = line.mid(index + 1);
            }

            index = left.indexOf(" : ");

            QString topic;
            int i;

            for (i=0; i < left.length(); i++) {
                QChar c = left[i];
                if (c == '_') continue;
                if (c >= '0' && c <= '9') continue;
                if (c >= 'a' && c <= 'z') continue;
                if (c >= 'A' && c <= 'Z') continue;
                break;
            }
            topic = left.left(i);

            QString desc = left.mid(index + 3);

            QuickHelp * h = new QuickHelp();
            h->_topic = topic;
            h->sig = left.left(index);
            h->url = "file:///" + maxPath + url;
            h->descr = desc;
            h->isGlobal = false;
            insert( h );

       }
       commands.close();
    }

/*
    QString text, line, topic, url, kind, module, descr, classs, tmp, paramsLine;
    QStringList lines;
    QuickHelp *h;

    QString path = QApplication::applicationDirPath()+"/";
    if( monkeyPath.isEmpty() )
        monkeyPath = path;

    QFile file0( path+"keywords.txt" );
    if( file0.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file0 );
        stream.setCodec( "UTF-8" );
        text = stream.readAll();
        file0.close();
    }
    else {
        text = "Strict;SuperStrict;Public;Private;"
            "Byte;Short;Int;Long;Float;Double;Ptr;String;Object;Mod;Continue;Exit;"
            "Include;Import;Module;Extern;ModuleInfo;"
            "New;Self;Super;Eachin;True;False;Null;Not;"
            "Extends;Abstract;Final;Select;Case;Default;Rem;"
            "Const;Local;Global;Field;Method;Function;Type;Interface;Implements;"
            "And;Or;Shl;Shr;End;If;Then;Else;Elseif;Endif;While;Wend;Repeat;Until;Forever;For;To;Step;Next;Return;"
            "Try;Catch;Throw;"
            "EndModule;EndExtern;EndFor;EndWhile;EndRem;DebugStop;DebugLog;";
    }

    lines = text.split(';');

    for( int i = 0 ; i < lines.size() ; ++i ){
        topic = lines.at(i);
        h = new QuickHelp();
        h->_topic = topic;
        h->descr = topic;
        h->descrForInsert = topic;
        h->kind = "keyword";
        h->isKeyword = true;
        if( topic=="Include"||topic=="Import"||topic=="Module"||topic=="Extern"||
                topic=="New"||topic=="Eachin"||
                topic=="Extends"||//topic=="Abstract"||topic=="Final"||
                topic=="Select"||topic=="Case"||
                topic=="Const"||topic=="Local"||topic=="Global"||topic=="Field"||topic=="Method"||topic=="Function"||topic=="Type"||topic=="Interface"||topic=="Implements"||
                topic=="And"||topic=="Or"||
                topic=="Until"||topic=="For"||topic=="To"||topic=="Step"||
                topic=="Catch"||topic=="Print" ) {
            h->descrForInsert += " ";
        }
        insert( h );
    }

    QFile file( monkeyPath+"/docs/html/index.txt" );
    if( file.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file );
        stream.setCodec( "UTF-8" );
        text = stream.readAll();
        file.close();
        lines = text.split('\n');

        //global values for highlight
        for( int i = 0 ; i < lines.count() ; ++i ) {
            line = lines.at( i ).trimmed();
            int j = line.lastIndexOf( ':' );
            if( j == -1 )
                continue;
            topic = line.left(j);
            url = "file:///"+monkeyPath+"/docs/html/"+line.mid(j+1);
            h = help( topic );
            if( !h ) {
                h = new QuickHelp();
            }
            else if( h->isKeyword ) {
                h->url = url;
                continue;
            }
            h = new QuickHelp();
            h->_topic = topic;
            h->url = url;
            h->isGlobal = false;
            insert( h );
        }
    }

    //all values with detailed description
    QFile file2( monkeyPath+"/docs/html/decls.txt" );
    if( !file2.open( QIODevice::ReadOnly ) )
        return false;
    QTextStream stream2( &file2 );
    stream2.setCodec( "UTF-8" );
    text = stream2.readAll();
    file2.close();
    lines = text.split('\n');

    for( int i = 0 ; i < lines.count() ; ++i ) {
        line = lines.at( i ).trimmed();
        line = line.left( line.length()-1 );
        int j = line.lastIndexOf( ';' );
        if( j == -1 )
            continue;

        topic = url = descr = kind = module = classs = paramsLine = "";

        descr = line.left(j);
        url = line.mid(j+1);

        h = help( topic );
        if( !h ) {
            h = new QuickHelp();
        }
        else if( h->isKeyword ) {
            continue;
        }

        //kind is {function, method, const, ...}
        j = descr.indexOf( ' ' );
        if( j > 0) {
            kind = descr.left(j);
            descr = descr.mid(j+1);
        }

        if( kind == "Module") {
            j = topic.lastIndexOf('.');
            if(j > 0) {
                module = topic.left(j);
                topic = topic.mid(j+1);
            }
            else {
                module = topic = descr;
            }
        }
        else {
            j = descr.indexOf( ':' );
            int j2 = descr.indexOf( '(' );
            if(j < 0 || (j2 > 0 && j2 < j))
                j = j2;
            if(j > 0) {
                topic = descr.left(j);
            }
            else {
                topic = descr;
            }
            j = topic.lastIndexOf( '.' );
            if(j > 0) {
                tmp = topic.left(j);
                topic = topic.mid(j+1);
                descr = descr.mid(j+1);
                j = tmp.lastIndexOf( '.' );
                if( j > 0 ) {
                    classs = tmp.mid(j+1);
                    if( url.indexOf( "_"+classs ) > 0) {
                        module = tmp.left(j);
                    }
                    else {
                        classs = "";
                        module = tmp;
                    }
                }
            }
            //params if exists
            j2 = descr.indexOf( '(' );
            if( j2 > 0) { // (
                paramsLine = descr.mid(j2+1);
                j = paramsLine.lastIndexOf( ')' );
                if( j >= 0 ) {
                    paramsLine = paramsLine.left(j);
                    if( paramsLine == "" )
                        paramsLine = "*"; //has empty brackets
                }
            }
        }

        h->_topic = topic;
        h->url = "file:///"+monkeyPath+"/docs/html/"+url;
        h->descr = descr;
        h->kind = kind.toLower();
        h->module = module;
        h->classs = classs;
        h->descrForInsert = topic;
        if( paramsLine != "" ) {
            h->params = paramsLine.split( ',' );
        }
        h->isGlobal = (h->kind=="function" || h->kind=="global" || h->kind=="const");

        insert( h );

    }
    */

    return true;
}

void QuickHelp::insert(QuickHelp *help ) {
    help->ltopic = help->_topic.toLower();
    map()->insert( help->ltopic, help );
    map2()->insert( help->descr, help );
}

QuickHelp* QuickHelp::help( const QString &topic ) {
    return map()->value( topic.toLower(), 0 );
}

QuickHelp* QuickHelp::help2( const QString &descr ) {
    return map2()->value( descr, 0 );
}

QMap<QString,QuickHelp*>* QuickHelp::map() {
    static QMap<QString, QuickHelp*> *m;
    if (!m) {
        m = new QMap<QString,QuickHelp*>;
    }
    return m;
}

QMap<QString,QuickHelp*>* QuickHelp::map2() {
    static QMap<QString, QuickHelp*> *m;
    if (!m) {
        m = new QMap<QString,QuickHelp*>;
    }
    return m;
}

QString QuickHelp::quick() {
    QString mod = sig;//(module.isEmpty() || _topic == module ? "" : module+" > ");
    QString cl = (clazz.isEmpty() ? "" : clazz+" > ");
    return mod + " : " + descr;
}

QString QuickHelp::hint() {
    QString mod = sig;//(module.isEmpty() || _topic == module ? "" : module);
    QString cl = (clazz.isEmpty() ? "" : clazz);
    QString s = "(" + kind + ") <b>" + descr+"</b>";
    if( mod != "" ) {
        s += "\n<i>Declared in:</i> "+mod;
    }
    if( cl != "" ) {
        s += " > "+cl;
    }
    return s;
}
