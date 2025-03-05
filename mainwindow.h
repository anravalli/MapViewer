#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "map.h"
#include "ego.h"

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
    void egoUpdated(QPointF p);

private:
    Ui::MainWindow *ui;
    MapViewer *the_map_viewer;
    Map *the_map;
    Ego *the_ego;
    Ego *the_other;
    // QString map_file;

protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // MAINWINDOW_H
