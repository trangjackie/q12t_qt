#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QList>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color:white;");
    //serial = new QSerialPort(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Quit_clicked()
{
    QApplication::quit();
}


void MainWindow::on_pushButton_uart_getport_clicked()
{
    ui->comboBox_uartport->clear();
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        {
        ui->comboBox_uartport->addItem(serialPortInfo.portName());
        }
}

void MainWindow::on_pushButton_uart_connect_clicked()
{
    ui->label_status->setText("chưa có gì để connect cả");
}

void MainWindow::on_pushButton_write_start_clicked()
{
    QString filename = "Write_"+get_time_string()+".txt";
    QString file_content;
    ui->label_status->setText(filename);
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );
        stream << "something" << endl;
    }

    file.close();
    // read back for verifing the file content
    file_content = file_read(filename);
    ui->plainTextEdit_textbox->document()->setPlainText(file_content);
}

QString MainWindow::get_time_string()
{
    QString fmt = "yyyyMMddhhmmss";
    QString timeStr = QDateTime::currentDateTime().toString(fmt);

    return timeStr;
}

QString MainWindow::file_read(QString filename)
{
    QString file_content = "";
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return file_content+"File open fail";

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        file_content += line;
    }
    file.close();
    return file_content;
}

void MainWindow::file_upload_to_host(QString host_ip_path)
{

}
