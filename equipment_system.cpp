#include "equipment_system.h"
#include "stdafx.h"
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <qdebug.h>
#include <qmessagebox.h>
#include <QtXml>
#include <qfile.h>
#include <qtreewidget.h>

equipment_system::equipment_system(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setFixedSize(1094, 756);

    //设置树控件前面序列隐藏
    ui.treeWidget->setHeaderHidden(true);

    //创建表格格式,3列
    ui.tableWidget->setColumnCount(3);
    //设置三列的宽度
    ui.tableWidget->setColumnWidth(0, 200);
    ui.tableWidget->setColumnWidth(1, 400);
    ui.tableWidget->setColumnWidth(2, 200);

    //设置不可编辑表格内容
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    //根据数据长度调整列宽
    //ui.tableWidget->resizeColumnToContents(0);

    QStringList list;  


    // 创建列表头list
    list << "ID" << "INFO" << "STATUS";
    ui.tableWidget->setHorizontalHeaderLabels(list);

    //输出支持数据库
    qDebug() << QSqlDatabase::drivers();

    db = QSqlDatabase::addDatabase("QODBC");
    db.setUserName("root");
    db.setPassword("Amu666888.");
    db.setHostName("192.168.77.130");
    db.setPort(3306);
    db.setDatabaseName("mysql");

    
    //点击开始查询做的操作，连接数据库，读取所有数据放到表格
    connect(ui.start_select, &QPushButton::clicked, this, [=]() {


        //查询新的数据，先把之前数据删除，再进行添加
        if (ui.tableWidget->rowCount() > 0)
        {
            for (int row = ui.tableWidget->rowCount() - 1; row >= 0; row--)
            {
                ui.tableWidget->removeRow(row);
            }
        }


        qDebug() << "1";

        bool res = db.open();
        if (!res)
        {
            qDebug() << "6";
            QMessageBox::information(this, "提示", "连接数据库失败！");
            return;
        }
        else
        {
            qDebug() << "5";
            QMessageBox::information(this, "提示", "连接成功！");
        }

        qDebug() << "2";

    //执行查询语句
        qDebug() << "3";

        QSqlQuery query;

        QString str = "select * from shebei";

        query.exec(str);

        //获取表的列数
        QSqlRecord rec = query.record();
        int rows = rec.count();
        qDebug() << rows;

        //获取结果集大小
        //if (query.last())
        //{
        //    res_size = query.at() + 1;
        //}

        //遍历数据
        while(query.next())
        {
            
            ui.tableWidget->insertRow(rowcount);

            shebei_id = query.value(0).toString();
            shebei_info =  query.value(1).toString();
            shebei_status =  query.value(2).toString();

            ui.tableWidget->setItem(rowcount, columncount, new QTableWidgetItem(shebei_id));
            columncount++;
            ui.tableWidget->setItem(rowcount, columncount, new QTableWidgetItem(shebei_info));
            columncount++;
            ui.tableWidget->setItem(rowcount, columncount, new QTableWidgetItem(shebei_status));
            
            columncount = 0;
            rowcount++;
        }
        qDebug() << rowcount;
        qDebug() << columncount;

        rowcount = 0;
        columncount = 0;

        //设置表格内容文本居中
        for (int i = 0; i < ui.tableWidget->rowCount(); i++)
        {
            for (int j = 0; j < ui.tableWidget->columnCount(); j++)
            {
                ui.tableWidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            }
        }

        db.close();

        });

    //点击插入信息，弹出输入框，确认就往数据库写入
    connect(ui.btn_insert_info, &QPushButton::clicked, this, [=]() {
    
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);

        if (ui.line_insert_id->text() == nullptr || ui.line_insert_info->text() == nullptr || ui.line_insert_status->text() == nullptr)
        {
            QMessageBox::information(this, "提示", "插入数据有空值！");
            return;
        }
        //打开数据库写入数据
        bool res = db.open();
        if (!res)
        {
            QMessageBox::information(this, "提示", "连接数据库失败!");
            return;
        }
        else
        {
            qDebug() << "yes";
        }

        QSqlQuery query;

        //存储一下插入数据的值
        int insert_id = ui.line_insert_id->text().toInt();
        QString insert_info = ui.line_insert_info->text();
        QString insert_status = ui.line_insert_status->text();

        QString insert_str = QString("insert into shebei(id,shebei_info,shebei_status)" 
                                     "values('%1','%2','%3')").arg(insert_id).arg(insert_info).arg(insert_status);
        bool insert_res = query.exec(insert_str);
        if (insert_res)
        {
            QMessageBox::information(this, "提示", "插入成功");
        }
        else
        {
            qDebug() << "error";
        }

        ui.line_insert_id->clear();
        ui.line_insert_info->clear();
        ui.line_insert_status->clear();

        db.close();
    });

    //点击删除后删除选中行数据，数据库也进行同步删除
    connect(ui.delete_choose, &QPushButton::clicked, this, [=]() {
        
        if (ui.tableWidget->currentRow() < 0)
        {
            return;
        }
        //获取当前选中的行
        int rowIndex = ui.tableWidget->currentRow();

        qDebug() << rowIndex;


        //找到删除行的id，去数据库进行删除
        int delete_id = ui.tableWidget->item(rowIndex,0)->text().toInt();

        //对话框返回结果，返回是就进行删除
        QMessageBox::StandardButton delete_res;
   
        //设置编码格式，能显示中文
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        QTextCodec::setCodecForLocale(codec);

        delete_res = QMessageBox::question(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("是否确认删除?"));
        if (delete_res == QMessageBox::Yes)
        {
            ui.tableWidget->removeRow(rowIndex);
            bool res = db.open();
            if (!res)
            {
                QMessageBox::information(this, "提示", "连接数据库失败!");
                return;
            }
            else
            {    
                qDebug() << "yes";
            }

            QSqlQuery query;

            QString delete_str = QString("delete from shebei where id='%1'").arg(delete_id);
            bool insert_res = query.exec(delete_str);
            if (insert_res)
            {
                QMessageBox::information(this, "提示", "删除成功");
            }
            else
            {
                qDebug() << "删除失败";
            }

            db.close();
            
        }
        

     });

    //保存表格数据成xml文件
    connect(ui.save_info, &QPushButton::clicked, this, [=]() {

        QString filename = QFileDialog::getSaveFileName(this,
            tr("保存文件"), ".",
            tr("Xml files (*.xml)"));;
       
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        QXmlStreamWriter xmlWriter(&file);

        //自动格式化
        xmlWriter.setAutoFormatting(true);
        //写入xml文件头
        xmlWriter.writeStartDocument();

        //嵌套循环，读取表格里的数据
        int i = 0;
        int j = 0;
        QString data_id;
        QString data_info;
        QString data_status;

        xmlWriter.writeStartElement("设备介绍");

        for ( i = 0 ; i < ui.tableWidget->rowCount(); i++)
        {
            xmlWriter.writeStartElement("设备");
            for ( j = 0 ; j < 1; j++)
            {
                data_id = ui.tableWidget->item(i, j)->text();
                data_info = ui.tableWidget->item(i, j + 1)->text();
                data_status = ui.tableWidget->item(i, j + 2)->text();

                xmlWriter.writeTextElement("ID", data_id);
                xmlWriter.writeTextElement("设备信息", data_info);
                xmlWriter.writeTextElement("设备状态", data_status);              
            }

            xmlWriter.writeEndElement();
           
        }

        xmlWriter.writeEndElement();

        xmlWriter.writeEndDocument();


        file.close();

        });
    
    //读取xml文件，插入到数据库
    connect(ui.file_insert, &QPushButton::clicked, this, [=](){
        

        //定义存储的变量
        QString data_id;
        QString data_info;
        QString data_status;
        QSqlQuery query;
        //统计插入成功多少条
        int data_num = 0;

        QString filename = QFileDialog::getOpenFileName(this,
            tr("Open Xml"), ".",
            tr("Xml files (*.xml)"));
        QFile file(filename);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::information(this,"提示","选择文件失败");
            return;
        }

        //打开数据库
        bool res = db.open();
        if (!res)
        {
            QMessageBox::information(this, "提示", "连接数据库失败!");
            return;
        }
        else
        {
            qDebug() << "yyyyes";
        }

        QXmlStreamReader xmlReader(&file);
        xmlReader.readNext();
        while (!xmlReader.atEnd()) {
            if (xmlReader.isStartElement()) {
                if (xmlReader.name() == "设备介绍") {
                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "设备")
                {
                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "ID") {
                    data_id = xmlReader.readElementText();
                    qDebug() << "读到的id：" << data_id;
                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "设备信息") {
                    data_info = xmlReader.readElementText();
                    qDebug() << "读到的简介：" << data_info;

                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "设备状态") {
                    data_status = xmlReader.readElementText();
                    qDebug() << "读到的状态：" << data_status;

                    QString data_str = QString("insert into shebei(id,shebei_info,shebei_status)"
                        "values('%1','%2','%3')").arg(data_id).arg(data_info).arg(data_status);
                    bool insert_res = query.exec(data_str);
                    if (insert_res)
                    {
                        data_num++;
                    }
                    else
                    {
                        qDebug() << "插入失败";
                    }


                    xmlReader.readNext();
                }
                else {
                    xmlReader.raiseError(QObject::tr("Not a options file"));
                }
            }
            else {
                xmlReader.readNext();
            }


            //xml文件读取有错误
            if (xmlReader.hasError()) {
                QMessageBox::information(this, "提示", "Error: Failed to parse file");
            }
            else if (file.error() != QFile::NoError) {
                QMessageBox::information(this, "提示", "Error: Cannot read file");
            }
        }

        file.close();
        db.close();

        QString insert_okstr = QString("成功插入'%1'条数据").arg(data_num);
        QMessageBox::information(this, "提示", insert_okstr);


        });

    connect(ui.list_openfile, &QPushButton::clicked, this, [=]() {
        

        QString filename = QFileDialog::getOpenFileName(this,
            tr("Open Xml"), ".",
            tr("Xml files (*.xml)"));
        QFile file(filename);
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            QMessageBox::information(this, "提示", "选择文件失败");
            return;
        }

        //定义存储的变量
        QString data_id;
        QString data_info;
        QString data_status;
        QSqlQuery query;

        //树插入节点
       static QTreeWidgetItem* top_shebei;
       static QTreeWidgetItem* tree_shebei;
       static QTreeWidgetItem* tree_shebei_id;
       static QTreeWidgetItem* tree_shebei_id_child;
       static QTreeWidgetItem* tree_shebei_info;
       static QTreeWidgetItem* tree_shebei_info_child;
       static QTreeWidgetItem* tree_shebei_status;
       static QTreeWidgetItem* tree_shebei_status_child;

        int shebei_num = 0;

        QXmlStreamReader xmlReader(&file);
        xmlReader.readNext();
        while (!xmlReader.atEnd()) {
            if (xmlReader.isStartElement()) {
                if (xmlReader.name() == "设备介绍") {
                    top_shebei = new QTreeWidgetItem(ui.treeWidget);
                    top_shebei->setText(0, "设备介绍");
                    top_shebei->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    top_shebei->setCheckState(0, Qt::Unchecked);      


                    connect(ui.treeWidget, &QTreeWidget::itemClicked, ui.treeWidget, [=](QTreeWidgetItem* item, int column) {

                        if (Qt::Checked == item->checkState(0))//全选check打勾
                        {
                            int count = item->childCount();//子节点的数目
                            if (count > 0)
                            {
                                for (int i = 0; i < count; i++)
                                {
                                    //子节点也选中
                                    item->child(i)->setCheckState(0, Qt::Checked);
                                }
                            }
                            else
                            {
                                return;
                                //item->setCheckState(0, Qt::Checked);
                            }
                        }
                        else  if (Qt::Unchecked == item->checkState(0))//存在未选中
                        {
                            int count = item->childCount();
                            if (count > 0)
                            {
                                for (int i = 0; i < count; i++)
                                {
                                    item->child(i)->setCheckState(0, Qt::Unchecked);
                                }
                            }
                            else
                            {
                                return;
                                //item->setCheckState(0, Qt::Unchecked);
                            }

                        }
                        else
                        {
                            return;
                        }

                        });

                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "设备")
                {
                    shebei_num++;
                    tree_shebei = new QTreeWidgetItem(top_shebei,QStringList(QString("设备%1").arg(shebei_num)));
                    tree_shebei->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                    tree_shebei->setCheckState(0, Qt::Unchecked);

                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "ID") {
                    data_id = xmlReader.readElementText();
                    qDebug() << "读到的id：" << data_id;

                    //创建一个id选择，点开就是下面的id值
                    tree_shebei_id = new QTreeWidgetItem(tree_shebei);
                    tree_shebei_id->setText(0, "ID");
                    tree_shebei_id->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

                    tree_shebei_id_child = new QTreeWidgetItem(tree_shebei_id);
                    tree_shebei_id_child->setText(0, data_id);
                    tree_shebei_id_child->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "设备信息") {
                    data_info = xmlReader.readElementText();
                    qDebug() << "读到的简介：" << data_info;

                    //创建一个设备信息选择，点开就是下面的信息
                    tree_shebei_info = new QTreeWidgetItem(tree_shebei);
                    tree_shebei_info->setText(0, "设备信息");
                    tree_shebei_info->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

                    tree_shebei_info_child = new QTreeWidgetItem(tree_shebei_info);
                    tree_shebei_info_child->setText(0, data_info);
                    tree_shebei_info_child->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

                    xmlReader.readNext();
                }
                else if (xmlReader.name() == "设备状态") {
                    data_status = xmlReader.readElementText();
                    qDebug() << "读到的状态：" << data_status;

                    //创建一个设备状态选择，点开就是下面的状态
                    tree_shebei_status = new QTreeWidgetItem(tree_shebei);
                    tree_shebei_status->setText(0, "设备状态");
                    tree_shebei_status->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

                    tree_shebei_status_child = new QTreeWidgetItem(tree_shebei_status);
                    tree_shebei_status_child->setText(0, data_status);
                    tree_shebei_status_child->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

                    xmlReader.readNext();
                }
                else {
                    xmlReader.raiseError(QObject::tr("Not a options file"));
                }
            }
            else {
                xmlReader.readNext();
            }

            //xml文件读取有错误
            if (xmlReader.hasError()) {
                QMessageBox::information(this, "提示", "Error: Failed to parse file");
            }
            else if (file.error() != QFile::NoError) {
                QMessageBox::information(this, "提示", "Error: Cannot read file");
            }
        }


        file.close();

        });


}
