/*
Ted, a simple text editor/IDE.

Copyright 2012, Blitz Research Ltd.

See LICENSE.TXT for licensing terms.
*/

#include "std.h"

QString stripDir( const QString &path ){
    int i=path.lastIndexOf( '/' );
    if( i==-1 ) return path;
    return path.mid( i+1 );
}

QString extractDir( const QString &path ){
    int i=path.lastIndexOf( '/' );
    if( i==-1 ) return "";
#ifdef Q_OS_WIN32
    if( i && path[i-1]==':' ) return "";
#endif
    return path.left( i );
}

QString extractExt( const QString &path ){
    int i=path.lastIndexOf( '.' )+1;
    return i && path.indexOf( '/',i )==-1 ? path.mid( i ) : "";
}

QString stripExt( const QString &path ) {
    QString p(fixPath(path));

    int i = p.lastIndexOf(".");
    if ((i >= 0) && (p.indexOf("/", i+1) == -1)) {
        return p.mid(0, i);
    }
    return p;
}

// Converts \ to /, removes trailing /s and prefixes drive if necessary.
//
QString fixPath( QString path ){
    if( path.isEmpty() )
        return path;
    if( isUrl( path ) )
        return path;
    path = path.replace( '\\','/' );
    path = QDir::cleanPath( path );
#ifdef Q_OS_WIN32
    if( path.startsWith( "//" ) )
        return path;
    if( path.startsWith( '/' ) )
        path = QDir::rootPath()+path.mid( 1 );
    if( path.endsWith( '/' ) && !path.endsWith( ":/" ) )
        path = path.left( path.length()-1 );
#else
    if( path.endsWith( '/' ) && path!="/" )
        path = path.left( path.length()-1 );
#endif
    return path;
}

bool removeDir( const QString &path ){

    bool result=true;
    QDir dir( path );

    if( dir.exists( path ) ){
        Q_FOREACH( QFileInfo info,dir.entryInfoList( QDir::NoDotAndDotDot|QDir::System|QDir::Hidden|QDir::AllDirs|QDir::Files,QDir::DirsFirst ) ){
            if( info.isDir() ){
                result=removeDir( info.absoluteFilePath() );
            } else {
                result=QFile::remove( info.absoluteFilePath() );
            }
            if( !result ) return result;
        }
        result=dir.rmdir( path );
    }

    return result;
}

bool isUrl( const QString &path ){
    return path.startsWith( "file:" ) || path.startsWith( "http:" ) || path.startsWith( "https:" ) || path.startsWith( "qrc:" );
}

static QString bmxPath;

QString blitzMaxPath(QString altPath) {
    if (!bmxPath.isEmpty()) {
        return bmxPath;
    }

    char * p = getenv("BMXPATH");

    if (p) {
        bmxPath = p;
        return bmxPath;
    }

    QString dir = QCoreApplication::applicationDirPath();
    if (!altPath.isEmpty()) {
        dir = altPath;
    }

    do {

        QString bmk(dir + "/bin/bmk");
#ifdef Q_OS_WIN32
        bmk += ".exe";
#endif

        QFileInfo info(bmk);

        if (info.exists()) {
            bmxPath = dir;

            QString p("BMXPATH=" + bmxPath);
            QByteArray q = p.toUtf8();
            putenv(q.data());

            return bmxPath;
        }

        QString nextDir(extractDir(dir));
        if (nextDir == dir) {
            return "";
        }
        dir = nextDir;

    } while (true);

}

QString modulePath(QString modid) {
    QString path(blitzMaxPath("") + "/mod");
    if (!modid.isEmpty()) {
        path += "/" + modid.replace(".", ".mod/") + ".mod";
    }

    return path;
}

void enumModules(QString modid, QStringList & mods) {
    QString path(modulePath(modid));

    QDir dir(path);

    QStringList list = dir.entryList();

    QString file;
    for (int i = 0; i < list.size() ; ++i) {
        file = list.at(i);
        if (file=="." || file=="..") {
            continue;
        }

        QString filePath(path + "/" + file);

        if ((file.right(4) != ".mod") && !QFileInfo(filePath).isDir()) {
            continue;
        }

        QString m(file.left(file.length() - 4));
        if (!modid.isEmpty()) {
            m = modid + "." + m;
        }

        int n = m.indexOf(".");
        if ((n != -1) && (m.indexOf(".", n + 1) == -1)) {
            mods.append(m);
        }

        enumModules(m, mods);
    }
}

QString moduleIdent(const QString modid) {
    return modid.mid(modid.lastIndexOf(".") + 1);
}

QString moduleSource(const QString modid) {
    return modulePath(modid) + "/" + moduleIdent(modid) + ".bmx";
}

QString quote(QString &s) {
    if (s.isEmpty()) {
        return s;
    }

    // already quoted
    if (s[0] == '"') {
        return s;
    }

    // no spaces
    if (s.indexOf(" ") == -1) {
        return s;
    }

    return "\"" + s + "\"";
}
