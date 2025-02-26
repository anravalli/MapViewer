#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mapviewer.h"
#include <QDebug>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto old_map = ui->map_area;
    //auto scroll_area = new QScrollArea(static_cast<QWidget *>(old_map->parent()));
    //map_viewer = new MapViewer(nullptr);
    //scroll_area->setWidgetResizable(map_viewer);
    //ui->map_area = scroll_area;

    //no scroll area
    map_viewer = new MapViewer(static_cast<QWidget *>(old_map->parent()));
    ui->map_area = map_viewer;

    connect(ui->actionLoad_Map, SIGNAL(triggered(bool)), map_viewer, SLOT(selectMap()));
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

