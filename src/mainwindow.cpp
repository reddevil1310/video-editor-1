#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "menufile.h"
#include "videoplayer.h"
#include "playercontrol.h"
#include "tracktool.h"
#include "track.h"
#include "actions.h"

#include <QListView>
#include <QGridLayout>
#include <QMediaPlayer>

QSettings* MainWindow::settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, "ua-bbm", "video-editor");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Video editor");
    initSettings();

    mnuFile = new MenuFile(this);
    ui->menuBar->addMenu(mnuFile);

    actAddRushs = ui->mainToolBar->addAction(QIcon("://icon/file-plus.svg"), "Add rushs");
    actFinalVideo = ui->mainToolBar->addAction(QIcon("://icon/file-archive.svg"), "Generate final media");
    ui->mainToolBar->addSeparator();
    actRemoveMedia = ui->mainToolBar->addAction(QIcon("://icon/delete.svg"), "Delete media");
    actFusionMedia = ui->mainToolBar->addAction(QIcon("://icon/merge.svg"), "Fusion media");
    actRenameMedia = ui->mainToolBar->addAction(QIcon("://icon/edit.svg"), "Rename media");

    gLayout = new QGridLayout();
    ui->centralWidget->setLayout(gLayout);

    trackTool = new TrackTool(this);

    listRush = new QListView(this);
    rushListModel = new RushListModel(listRush);
    listRush->setModel(rushListModel);
    listRush->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listRush->setDragEnabled(true);
    listRush->setAcceptDrops(true);
    listRush->setFixedWidth(200);

    videoPlayer = new VideoPlayer(this);

    gLayout->addWidget(listRush, 0, 0);
    gLayout->addWidget(videoPlayer, 0, 1);
    gLayout->addWidget(trackTool, 1, 0, 1, 2);

    connect(listRush->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), rushListModel, SLOT(currentSelectionChanged(QItemSelection,QItemSelection)));
    connect(rushListModel, SIGNAL(emitSelection(Media&)), trackTool, SLOT(setMedia(Media&)));
    connect(rushListModel, SIGNAL(emitSelection(Media&)), videoPlayer, SLOT(setCurrentMedia(Media&)));
    connect(rushListModel, SIGNAL(selectionTypeChange(RushListModel::SelectionType)), this, SLOT(selectionActionChanged(RushListModel::SelectionType)));
    connect(trackTool, SIGNAL(actionClick(Actions::enumActions,QVector<QTime>)), rushListModel, SLOT(updateMedia(Actions::enumActions, QVector<QTime>)));
    connect(videoPlayer->getMediaPlayer(), SIGNAL(positionChanged(qint64)), trackTool->getTrack(), SLOT(updateCursorVideo(qint64)));
    connect(actAddRushs, SIGNAL(triggered(bool)), mnuFile, SLOT(importFiles()));
    connect(actFinalVideo, SIGNAL(triggered(bool)), rushListModel, SLOT(getFinalVideo()));
    connect(actRemoveMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(removeSelectedMedia()));
    connect(actRenameMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(renameSelectedMedia()));
    connect(actFusionMedia, SIGNAL(triggered(bool)), rushListModel, SLOT(fusionSelectedMedia()));
    connect(mnuFile, SIGNAL(filesImported(QStringList)), rushListModel, SLOT(addRushs(QStringList)));
    connect(mnuFile, SIGNAL(quit()), this, SLOT(close()));
    selectionActionChanged(RushListModel::NOTHING);
    resize(600, 500);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete actAddRushs;
    delete actRemoveMedia;
    delete actFusionMedia;
    delete actRenameMedia;
    delete actFinalVideo;
    delete mnuFile;
    delete rushListModel;
    delete listRush;
    delete videoPlayer;
    delete trackTool;
    delete gLayout;

    // Suppression des fichiers vidéos des dossiers de gestion (original, preview)
    QString nameDir = MainWindow::settings->value("General/dir_original").toString();
    Actions::removeAllFileDir(nameDir);
    nameDir = MainWindow::settings->value("General/dir_preview").toString();
    Actions::removeAllFileDir(nameDir);

    settings->sync();
    delete settings;
}

void MainWindow::selectionActionChanged(RushListModel::SelectionType type)
{
    // actAddRushs always enable
    switch (type) {
        case RushListModel::SINGLE:
            trackTool->setEnabled(true);
            actRenameMedia->setEnabled(true);
            actRemoveMedia->setEnabled(true);
            actFusionMedia->setEnabled(false);
            break;
        case RushListModel::MULTI:
            trackTool->setEnabled(false);
            actRenameMedia->setEnabled(false);
            actFusionMedia->setEnabled(true);
            actRemoveMedia->setEnabled(true);
            break;
        case RushListModel::NOTHING:
            trackTool->setEnabled(false);
            actFusionMedia->setEnabled(false);
            actRenameMedia->setEnabled(false);
            actRemoveMedia->setEnabled(false);
            break;
        default:
            break;
    }
}

void MainWindow::initSettings()
{
    // Si première utilisation
    if (settings->allKeys().isEmpty()){
        settings->beginGroup("General");
        QString preview_path = QString(QDir::homePath()+"/."+settings->applicationName()+"/preview");
        QDir dir;
        if (dir.mkpath(preview_path))
            settings->setValue("dir_preview", preview_path);
        QString originalsplit_path = QString(QDir::homePath()+"/."+settings->applicationName()+"/original");
        if (dir.mkpath(originalsplit_path))
            settings->setValue("dir_original", originalsplit_path);
        settings->endGroup();
        settings->sync();
    }
}
