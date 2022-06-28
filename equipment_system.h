#pragma once

#include <QtWidgets/QWidget>
#include "ui_equipment_system.h"
#include <qsqldatabase.h>
#include <qsqlquery.h>

class equipment_system : public QWidget
{
    Q_OBJECT

public:
    equipment_system(QWidget *parent = Q_NULLPTR);

private:
    Ui::equipment_systemClass ui;
    QSqlDatabase db;
    //表格行数
    int rowcount = 0;
    //表格列数
    int columncount = 0;
    //第一列数据
    QString shebei_id;
    //第二列数据
    QString shebei_info;
    //第三列数据
    QString shebei_status;
    //结果集大小
    int res_size;



};
