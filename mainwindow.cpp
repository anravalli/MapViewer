#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "mapviewer.h"
#include <QDebug>
#include <QFileDialog>
#include <QColorDialog>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // auto old_map = ui->map_area;
    QVBoxLayout *layout = new QVBoxLayout(ui->map_area);

    the_map = new Map();
    the_ego = new Ego();
    the_other = new Ego();
    the_ego_dialog = new EditRoadUser(the_ego, this);
    the_other_dialog = new EditRoadUser(the_other, this);
    the_ego_info = new QLabel();
    ui->statusbar->addWidget(the_ego_info);
    ui->statusbar->addWidget(&the_other_info);
    ui->statusbar->addWidget(&the_risk_info);

    the_map_viewer = new MapViewer(static_cast<QWidget *>(this),
                                   the_map, the_ego, the_other);
    QPalette palette = ui->map_area->palette();

    the_ego_dialog->hide();
    the_other_dialog->hide();

    the_map_viewer->setPalette(palette);
    the_map_viewer->setAutoFillBackground(true);
    the_map_viewer->repaint();
    // ui->map_area = the_map_viewer;
    layout->addWidget(the_map_viewer);

    connect(ui->actionLoad_Map, SIGNAL(triggered(bool)), this, SLOT(selectMap()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), this, SLOT(resetView()));
    connect(ui->actionCenter, SIGNAL(triggered(bool)), this, SLOT(centerView()));
    connect(ui->actionCenter_to_EGO, SIGNAL(triggered(bool)), this, SLOT(centerToEgo()));
    connect(ui->actionCenter_to_OTHER, SIGNAL(triggered(bool)), this, SLOT(centerToOther()));

    connect(ui->actionBackground_Color, SIGNAL(triggered(bool)), this, SLOT(changeBackGroundColor()));

    connect(ui->actionShow_Markers, SIGNAL(toggled(bool)), this, SLOT(showMarkers(bool)));
    connect(ui->actionShow_Markers_labels, SIGNAL(toggled(bool)), this, SLOT(showMarkersLabels(bool)));

    connect(ui->actionEGO, SIGNAL(triggered(bool)), this, SLOT(editEgo()));
    connect(ui->actionOTHER, SIGNAL(triggered(bool)), this, SLOT(editOther()));

    connect(the_ego, SIGNAL(updated(QPointF)), this, SLOT(egoUpdated(QPointF)));
    connect(the_ego, SIGNAL(updated(QPointF)), the_map, SLOT(matchEgoToMap(QPointF)));

    connect(the_other, SIGNAL(updated(QPointF)), this, SLOT(otherUpdated(QPointF)));
    connect(the_other, SIGNAL(updated(QPointF)), the_map, SLOT(matchOtherToMap(QPointF)));

    connect(the_map, SIGNAL(matchedEgo(SegmentInfo)), the_ego, SLOT(updateMapInfo(SegmentInfo)));
    connect(the_map, SIGNAL(matchedOther(SegmentInfo)), the_other, SLOT(updateMapInfo(SegmentInfo)));

    // delete old_map;
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

void MainWindow::resetView()
{
    the_map_viewer->reset();
}

void MainWindow::centerView()
{
    the_map_viewer->centerView();
}

void MainWindow::centerToEgo()
{
    the_map_viewer->centerToEgo();
}

void MainWindow::centerToOther()
{
    the_map_viewer->centerToOther();
}

void MainWindow::changeBackGroundColor()
{
    QPalette palette = the_map_viewer->palette();
    auto brush = palette.brush(QPalette::Window);
    // brush.color()
    auto color = QColorDialog::getColor(brush.color(), nullptr, "Select new map background color");
    palette.setColor(QPalette::Window, color);
    the_map_viewer->setPalette(palette);
}

void MainWindow::showMarkers(bool checked)
{
    the_map_viewer->setMarker_en(checked);
}

void MainWindow::showMarkersLabels(bool checked)
{
    the_map_viewer->setMarker_label_en(checked);
}

void MainWindow::editEgo()
{
    the_ego_dialog->show();
}

void MainWindow::editOther()
{
    the_other_dialog->show();
}

void MainWindow::egoUpdated(QPointF p)
{
    // auto status_bar = ui->statusbar;
    QString msg = "EGO: ";
    msg += QString::number(p.x()) + ", " + QString::number(p.y());
    // status_bar->showMessage(msg);
    the_ego_info->setText(msg);
    if(the_map_viewer != nullptr)
        the_map_viewer->update();
}

void MainWindow::otherUpdated(QPointF p)
{
    // auto status_bar = ui->statusbar;
    QString msg = "OTHER: ";
    msg += QString::number(p.x()) + ", " + QString::number(p.y());
    // status_bar->showMessage(msg);
    the_other_info.setText(msg);
    if(the_map_viewer != nullptr)
        the_map_viewer->update();
}
