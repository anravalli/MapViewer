#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    MapViewer *map_viewer;
    QString map_file;
protected:
    void resizeEvent(QResizeEvent *event) override;

};
#endif // MAINWINDOW_H
