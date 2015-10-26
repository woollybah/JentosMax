/*
Ted, a simple text editor/IDE.

Copyright 2012, Blitz Research Ltd.

See LICENSE.TXT for licensing terms.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "std.h"
#include <QProxyStyle>

class CodeEditor;
class ProjectTreeModel;
class DebugTreeModel;
class FindDialog;
class Process;
class FindInFilesDialog;
class QuickHelp;
class HelpButtonLabel;

namespace Ui {
class MainWindow;
}

class Prefs;
class PrefsDialog;
class QuickHelp;
class TabWidgetDrop;


class MainWindow : public QMainWindow{
    Q_OBJECT

public:

    MainWindow( QWidget *parent=0 );
    ~MainWindow();

    void cdebug( const QString &str );
    static bool isValidBlitzMaxPath(QString &path);
    void updateTheme();

private:

    void updateCodeViews(QTreeView *tree, QListView* list);

    void parseAppArgs();
    void initKeywords();


    bool isBuildable( CodeEditor *editor );
    QString widgetPath( QWidget *widget );
    CodeEditor *editorWithPath( const QString &path );

    QWidget *newFile( const QString &path );
    QWidget *openFile( const QString &path,bool addToRecent );
    bool saveFile( QWidget *widget,const QString &path );
    bool closeFile( QWidget *widget,bool remove=true );

    void initSettings();
    void readSettings();
    void writeSettings();

    void updateWindowTitle();
    void updateTabLabel( QWidget *widget );
    void updateActions();

    void print(const QString &str , QString kind);
    void runCommand(QString cmd, QWidget *fileWidget , QString message);
    void build( QString mode,QString pathmonkey);
    void buildModules(bool fullBuild);

    bool confirmQuit();
    void closeEvent( QCloseEvent *event );

    QString getBMKPath();
    QString getPlatform();
    QString getArchitecture();

    void cleanupDebug();

protected:

    void showEvent(QShowEvent * event);


public slots:

    void onStyleChanged(bool b);
    void onCodeTreeViewClicked( const QModelIndex &index );
    void onSourceListViewClicked( const QModelIndex &index );

    void onLowerUpperCase();
    void onNetworkFinished(QNetworkReply *reply);
    void onCheckForUpdates();
    void onCheckForUpdatesSilent();
    void onOpenUrl();
    void onKeyEscapePressed();

    //File menu
    void onFileNew();
    void onFileOpen();
    void onFileOpenRecent();
    void onFileClose();
    void onFileCloseAll();
    void onFileCloseOthers();
    void onFileSave();
    void onFileSaveAs();
    void onFileSaveAll();
    void onFileNext();
    void onFilePrevious();
    void onFilePrefs();
    void onFileQuit();

    //Edit menu
    void onEditUndo();
    void onEditRedo();
    void onEditCut();
    void onEditCopy();
    void onEditPaste();
    void onEditDelete();
    void onEditSelectAll();
    void onEditFind();
    void onEditFindNext();
    void onEditFindPrev();
    void onEditReplace();
    void onEditReplaceAll();
    void onFindReplace( int how );
    void onEditGoto();
    void onEditFindInFiles();

    //View menu
    void onViewToolBar();
    void onViewWindow();
    //
    void onCommentUncommentBlock();
    void onToggleBookmark();
    void onPreviousBookmark();
    void onNextBookmark();
    void onFoldAll();
    void onUnfoldAll();
    void onGoBack();
    void onGoForward();
    //
    //Build/Debug menu
    void onBuildBuild();
    void onBuildRun();
    void onBuildCheck();
    void onBuildUpdate();
    void onDebugStep();
    void onDebugStepInto();
    void onDebugStepOut();
    void onDebugKill();
    void onBuildTarget();
    void onBuildConfig();
    void onBuildLockFile();
    void onBuildUnlockFile();
    void onBuildAddProject();

    //Window menu
    void onSwitchFullscreen();

    //Help menu
    void onHelpHome();
    void onHelpBack();
    void onHelpForward();
    void onHelpQuickHelp();
    void onHelpAbout();
    void onHelpRebuild();
    void onHelpOnlineDocs();
    void onHelpBlitzMaxHomepage();

    void onOpenCodeFile(const QString &file, const QString &folder, const int &lineNumber );
    void onDropFiles( const QStringList &list );

    void onCodeAnalyze();
    void onClearOutput();
    void onCloseSearch();
    void onChangeAnalyzerProperties(bool toggle=false);

    void onTabsCloseTab();
    void onTabsCloseOtherTabs();
    void onTabsCloseAllTabs();

    void onThemeAndroidStudio();
    void onThemeNetBeans();
    void onThemeQtCreator();
    void onFindUsages();

    void onLinkClicked( const QUrl &url );
    void onDocsZoomChanged(int);
    void onLoadFinished(bool ok);

private slots:

    void onShowHelp( const QString &text );

    void onCloseUsagesTab(int index);
    void onUsagesJumpToLine(QTreeWidgetItem *item, int column);
    void onCloseMainTab( int index );
    void onMainTabChanged( int index );

    void onDockVisibilityChanged( bool visible );

    void onEditorMenu(const QPoint &pos);
    void onTabsMenu( const QPoint &pos );
    void onProjectMenu( const QPoint &pos );
    void onFileClicked( const QModelIndex &index );
    void onSourceMenu( const QPoint &pos );
    void onUsagesMenu( const QPoint &pos );

    void onTextChanged();
    void onCursorPositionChanged();
    void onShowCode(const QString &path, int line , bool error=false);
    void onShowDebugCode(const QString &path, int line, bool toFront);
    void onShowCode( const QString &path,int pos,int len );

    void onProcStdout();
    void onProcStderr();
    void onProcLineAvailable( int channel );
    void onProcFinished();

    void onStatusBarChanged(const QString &text);
    void onAutoformatAll();


    void onUsagesRename();
    void onUsagesSelectAll();
    void onUsagesUnselectAll();


    void on_actionLock_Target_triggered();

    void on_actionLock_Target_toggled(bool arg1);

    void on_webView_selectionChanged();

    void on_docsDockWidget_allowedAreasChanged(const Qt::DockWidgetAreas &allowedAreas);

    void on_actionClose_all_Tabs_triggered();

    void on_actionThemeMonokaiDarkSoda_triggered();

    void on_actionThemeLightTable_triggered();

    void onTargetChanged( int index );

private:


    QNetworkAccessManager *_networkManager;
    bool _showMsgbox;
    Ui::MainWindow *_ui;

    QString _defaultDir;

    QString _blitzmaxPath;
    QString _buildBmxCmd;
    QString _runBmxCmd;

    static QString _blitzMaxPath, _transPath;
    bool _isShowHelpInDock;
    QString _buildBlitzMaxCmd;
    QString _runBlitzMaxCmd;

    QString _transVersion;

    TabWidgetDrop *_mainTabWidget;

    Process *_consoleProc;

    ProjectTreeModel *_projectTreeModel;
    DebugTreeModel *_debugTreeModel;

    CodeEditor *_codeEditor;
    CodeEditor *_lockedEditor;
    QWebView *_helpWidget;

    PrefsDialog *_prefsDialog;
    //FindDialog *_findDialog;
    FindInFilesDialog *_findInFilesDialog;

    QMenu *_tabsPopupMenu;
    QMenu *_projectPopupMenu;
    QMenu *_dirPopupMenu;
    QMenu *_filePopupMenu;
    QMenu *_fileImagePopupMenu;
    QMenu *_fileBlitzMaxPopupMenu;
    QMenu *_sourcePopupMenu;
    QMenu *_editorPopupMenu;
    QMenu *_usagesPopupMenu;

    QLabel *_statusWidget;

    QComboBox *_targetsWidget;
    QComboBox *_architecturesWidget;
    QComboBox *_configsWidget;
    QComboBox *_indexWidget;

    QString _lastHelpIdent;
    int _lastHelpCursorPos;

    HelpButtonLabel * _helpBack;
    HelpButtonLabel * _helpForward;
    bool _home;
};

// default font size on OS X is too big. We'll use a smaller size instead :-)
class CustomProxyStyle : public QProxyStyle {
public:
#ifdef Q_OS_MAC
    virtual void polish(QWidget * widget) {
        QMenu* menu = dynamic_cast<QMenu*>(widget);
        if (!menu && !widget->testAttribute(Qt::WA_MacNormalSize)) {
            widget->setAttribute(Qt::WA_MacSmallSize);
        }
    }
#endif
};

class HelpButtonLabel : public QLabel {
    Q_OBJECT
public:
    HelpButtonLabel( const QPixmap & pix, QWidget * parent = 0 );
    ~HelpButtonLabel(){}

    signals:
        void clicked();

    protected:
        void mouseReleaseEvent ( QMouseEvent * event ) ;
};

#endif // MAINWINDOW_H
