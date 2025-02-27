#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mapviewer.h"
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    auto old_map = ui->map_area;

    the_map = new Map();
    the_ego = new Ego();
    the_map_viewer = new MapViewer(static_cast<QWidget *>(old_map->parent()),
                                   the_map, the_ego);
    ui->map_area = the_map_viewer;

    connect(ui->actionLoad_Map, SIGNAL(triggered(bool)), this, SLOT(selectMap()));
    delete old_map;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    qDebug() << "Main window:" << this->size();
    ui->map_area->resize(event->size());
}


void MainWindow::selectMap()
{
    std::cout << "select map " << std::endl;
    QString map_file = QFileDialog::getOpenFileName(this, "Open Map File", "", "OSM (*.osm)");

    // Check if the user selected a file
    if (!map_file.isEmpty()) {
        qDebug() << "Selected file:" << map_file;
        the_map->loadMap(map_file);
        the_map_viewer->reset();
    } else {
        qDebug() << "No file selected.";
    }
}

