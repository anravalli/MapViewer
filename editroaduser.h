#ifndef EDITROADUSER_H
#define EDITROADUSER_H

#include <QDialog>
#include "ego.h"

namespace Ui {
class EditRoadUser;
}

class EditRoadUser : public QDialog
{
    Q_OBJECT

public:
    explicit EditRoadUser(Ego *user, QWidget *parent = nullptr);
    ~EditRoadUser();

public slots:
    void onEditOk();
    void onApply();
    void onFieldReady();

private:
    Ui::EditRoadUser *ui;
    Ego *m_roaduser;
    Ego m_tmp_roaduser;
};

#endif // EDITROADUSER_H
