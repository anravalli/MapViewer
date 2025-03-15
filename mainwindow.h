#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "map.h"
#include "ego.h"

#include "editroaduser.h"

class MapViewer;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void selectMap();
    void resetView();
    void centerView();
    void changeBackGroundColor();
    void showMarkers(bool checked);
    void showMarkersLabels(bool checked);
    void editEgo();
    void editOther();
    void egoUpdated(QPointF p);
    void otherUpdated(QPointF p);


private:
    Ui::MainWindow *ui;
    MapViewer *the_map_viewer = nullptr;
    Map *the_map;
    Ego *the_ego;
    Ego *the_other;
    QLabel *the_ego_info;
    QLabel the_other_info;
    QLabel the_risk_info;
    EditRoadUser *the_ego_dialog;
    EditRoadUser *the_other_dialog;

protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // MAINWINDOW_H
