#include "editroaduser.h"
#include "ui_editroaduser.h"
#include <iostream>
#include <QPushButton>


EditRoadUser::EditRoadUser(Ego *user, QWidget *parent) :
    m_roaduser(user),
    QDialog(parent),
    ui(new Ui::EditRoadUser)
{
    ui->setupUi(this);
    QPushButton *apply_button = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onEditOk()));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(onFieldReady()));
    connect(apply_button, SIGNAL(clicked(bool)), this, SLOT(onApply()));
}

EditRoadUser::~EditRoadUser()
{
    delete ui;
}

void EditRoadUser::onEditOk()
{
    QPointF p(ui->lon_spin->value(),ui->lat_spin->value());
    m_roaduser->update(p);
    this->accept();
}

void EditRoadUser::onApply()
{
    auto line = ui->lineEdit->text();
    if(line.size())
        onFieldReady();
    QPointF p(ui->lon_spin->value(),ui->lat_spin->value());
    m_roaduser->update(p);

}

void EditRoadUser::onFieldReady()
{
    auto line = ui->lineEdit->text();
    double lat = 0.0;
    double lon = 0.0;
    double head = 0.0;
    // int start = 0;
    // int split = line.indexOf(',');
    QString str_lat = line.section(',',0, 0);
    // start = split+1;
    // split = line.indexOf(',', start);
    auto str_lon = line.section(',',1, 1);
    // start = split+1;
    auto str_head = line.section(',',2, 2);
    std::cout << "position: " << str_lat.toStdString() << ", " << str_lon.toStdString() << std::endl;
    // lat = std::strtod(str_lat.c_str(), nullptr);
    // lon = std::strtod(str_lon.c_str(), nullptr);
    // head = std::strtod(str_head.c_str(), nullptr);
    lat = str_lat.toDouble();
    lon = str_lon.toDouble();
    head = str_head.toDouble();
    std::cout << "position: " << lat << ", " << lon << std::endl;
    ui->lon_spin->setValue(lon);
    ui->lat_spin->setValue(lat);
    ui->head_spin->setValue(head);
}
