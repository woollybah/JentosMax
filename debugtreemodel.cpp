/*
Ted, a simple text editor/IDE.

Copyright 2012, Blitz Research Ltd.

See LICENSE.TXT for licensing terms.
*/

#include "debugtreemodel.h"
#include "proc.h"

enum DebugType {
    DEBUG_NDEF = -1,
    DEBUG_FUNC = 0,
    DEBUG_VAR,
    DEBUG_OBJ
};

class DebugItem : public QStandardItem{
public:
    DebugItem():_type(DEBUG_NDEF),_expanded( false ){
        setEditable( false );
    }

    int type()const{
        return _type;
    }

    QString address()const{
        return _addr;
    }

    QString info()const{
        return _info;
    }

    void setInfo(QString & info) {
        _info = info;
    }

    void setExpanded( bool expanded ){
        _expanded=expanded;
    }

    bool expanded()const{
        return _expanded;
    }

    void update( const QString &ctext, const QString & file) {

        QString txt=ctext;
        _type=DEBUG_VAR;
        _info="";
        if( txt.startsWith("Function") ){
           _type=DEBUG_FUNC;
           _info = file;
        }else if( txt.startsWith("Local") ){
            _type=DEBUG_VAR;

            if( txt.indexOf( "\"" )==-1 ){
                int i=txt.indexOf( "=$" );
                if( i!=-1 ){
                   _type=DEBUG_OBJ;
                   _addr=txt.mid( i+2 );
                }
            }
        }
        if (text() != txt) {
            setText( txt );
        }
    }

private:
    DebugType _type;
    QString _addr;
    QString _info;
    bool _expanded;
};

DebugTreeModel::DebugTreeModel( Process *proc,QObject *parent ):QStandardItemModel( parent ),_proc( proc ),_stopped( false ){
}

void DebugTreeModel::stop(QString & line){

    if( _stopped ) return;

    _stopped=true;

    QStandardItem *root=invisibleRootItem();

    //build callstack...
    //
    DebugItem *func=0;
    int n_funcs=0,n_vars=0;
    QString file;

    QStack<DebugItem*> objs;

    if (line == "Debug:" || line == "DebugStop:") {
        // eat the input line
        _proc->readLine( 1 );

        _proc->writeLine("t");
    }

    for(;;){

        QString text=_proc->readLine( 1 );

        if (text.startsWith("~>")) {
            text = text.mid(2);
        }

        if( text.isEmpty() ) break;

        // end of debug scope
        if (text == "}") {
            continue;
        }

        // stack file/line
        if (text.startsWith("@")) {
            file = text.mid(1);
            continue;
        }

        // local scope change
        if (text.startsWith("Local <local>")) {

            // set function file offset
            if (func) {
                func->setInfo(file);
            }

            continue;
        }

        if (text.startsWith("Function")) {
            if( func ) func->setRowCount( n_vars );
            n_vars = 0;
            func = dynamic_cast<DebugItem*>( root->child( n_funcs++ ) );
            if (!func) {
                func = new DebugItem;
                root->appendRow(func);
            }

            func->update(text, file);

        } else if (func) {
            DebugItem *item = dynamic_cast<DebugItem*>(func->child(n_vars++));

            if (!item) {
                item = new DebugItem;
                func->appendRow(item);
            }

            item->update(text, file);

            if (item->type()==DEBUG_OBJ && item->address()!=0 && item->expanded()) {
                objs.push( item );
            } else {
                item->setRowCount(0);
            }
        }

    }

    if( func ) func->setRowCount( n_vars );

    root->setRowCount( n_funcs );

    while( !objs.isEmpty() ){

        DebugItem *item=objs.pop();

        _proc->writeLine( QString("D" + item->address() ) );

        int n_vars=0;

        for(;;){

            QString text=_proc->readLine( 1 );

            if (text.startsWith("~>")) {
                text = text.mid(2);
            }

            if( text.isEmpty() ) break;

            if (text.startsWith("ObjectDump@") || text == "}") {
                continue;
            }

            DebugItem *child=dynamic_cast<DebugItem*>( item->child( n_vars++ ) );
            if( !child ){
                child=new DebugItem;
                item->appendRow( child );
            }

            child->update( text, "" );

            if( child->type()==DEBUG_OBJ && child->expanded() ){
                objs.push( child );
            }else{
                child->setRowCount(0);
            }
        }
        item->setRowCount( n_vars );
    }


    showFileLine( file );
}

void DebugTreeModel::run(){
    if( !_stopped ) return;
    _proc->writeLine( "r" );
    _stopped=false;
}

void DebugTreeModel::step(){
    if( !_stopped ) return;
    _proc->writeLine( "s" );
    _stopped=false;
}

void DebugTreeModel::stepInto(){
    if( !_stopped ) return;
    _proc->writeLine( "e" );
    _stopped=false;
}

void DebugTreeModel::stepOut(){
    if( !_stopped ) return;
    _proc->writeLine( "l" );
    _stopped=false;
}

void DebugTreeModel::kill(){
    if( !_stopped ) return;
    _proc->writeLine( "q" );
    _stopped=false;
}

void DebugTreeModel::onClicked( const QModelIndex &index ){

    DebugItem *item=dynamic_cast<DebugItem*>( itemFromIndex( index ) );
    if( !item ) return;

    if( item->type()==DEBUG_FUNC ){
        showFileLine(item->info());
/*        QString info=item->info();
        int i=info.lastIndexOf( '<' );
        if( i!=-1 && info.endsWith( '>' ) ){
            int n = info.indexOf(',', i);
            QString path=info.left( i );
            int line=info.mid( i+1,n - i - 1 ).toInt()-1;
            emit showCode( path,line );
        }*/
    }
}

void DebugTreeModel::showFileLine(const QString &file) {
    QString info=file;
    int i=info.lastIndexOf( '<' );
    if( i!=-1 && info.endsWith( '>' ) ){
        int n = info.indexOf(',', i);
        QString path=info.left( i );
        int line=info.mid( i+1,n - i - 1 ).toInt()-1;
        emit showDebugCode( path,line );
    }
}

bool DebugTreeModel::hasChildren( const QModelIndex &parent )const{

    if( DebugItem *item=dynamic_cast<DebugItem*>( itemFromIndex( parent ) ) ){
        return item->type()==DEBUG_FUNC || item->type()==DEBUG_OBJ;
    }

    return QStandardItemModel::hasChildren( parent );
}

bool DebugTreeModel::canFetchMore( const QModelIndex &parent )const{

    if( !_stopped ) return false;

    if( DebugItem *item=dynamic_cast<DebugItem*>( itemFromIndex( parent ) ) ){

        return item->type()==DEBUG_OBJ && !item->expanded();
    }

    return false;
}

void DebugTreeModel::fetchMore( const QModelIndex &parent ){

    if( !_stopped ) return;

    if( DebugItem *item=dynamic_cast<DebugItem*>( itemFromIndex( parent ) ) ){

        int n_vars=0;

        _proc->writeLine( QString("D" + item->address()) );

        for(;;){

            QString text=_proc->readLine( 1 );

            if (text.startsWith("~>")) {
                text = text.mid(2);
            }

            if( text.isEmpty() ) break;

            if (text.startsWith("ObjectDump@") || text == "}") {
                continue;
            }

            DebugItem *child=dynamic_cast<DebugItem*>( item->child( n_vars++ ) );
            if( !child ){
                child=new DebugItem;
                item->appendRow( child );
            }
            child->update( text, "" );
        }

        item->setRowCount( n_vars );

        item->setExpanded( true );
    }
}
