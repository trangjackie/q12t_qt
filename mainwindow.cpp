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
#include <QMessageBox>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lineEditkeyboard = new KeyboardQwerty();
    this->setStyleSheet("background-color: rgba(215, 214,213, 100);");
    ui->mainToolBar->setIconSize(QSize(40,40));
    // UART
    serial = new QSerialPort(this);
    settings = new SettingsDialog;
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(uart_readData()));

    // connect fuction for on-screen keyboard
    connect(ui->lineEdit_host_ip ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_host_user,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_host_passwd,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_UartCommand,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    // Currently, onscreen keyboard only support line edit
    //connect(ui->plainTextEdit_textbox,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::run_keyboard_lineEdit()
{
    QLineEdit *line = (QLineEdit *)sender();
    lineEditkeyboard->setLineEdit(line);

    int ph = this->geometry().height();
    int pw = this->geometry().width();
    int px = this->geometry().x();
    int py = this->geometry().y();
    int dw = lineEditkeyboard->geometry().width();
    int dh = lineEditkeyboard->geometry().height();
    lineEditkeyboard->setGeometry(px+(pw-dw)/2, py+ph-dh, dw, dh );
    lineEditkeyboard->show();
}


void MainWindow::on_pushButton_write_start_clicked()
{
    QString filename = "Write_"+get_time_string()+".txt";
    QString file_content;

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

    // upload file to server
    file_upload_to_host(filename,ui->lineEdit_host_user->text(),ui->lineEdit_host_ip->text());
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

void MainWindow::file_upload_to_host(QString filename,QString user,QString host_ip_path)
{
    QString command = "scp "+filename+" "+user+"@"+host_ip_path+":~/";
    // upload data to server

    ui->statusBar->showMessage(command);

}

/********************* Functions for UART ******************************/
void MainWindow::uart_writeData(const QByteArray &data)
{
    serial->write(data);
}

void MainWindow::uart_readData()
{
    QByteArray data = serial->readAll();
    qDebug(data);
    ui->statusBar->showMessage(QString(data));
    ui->plainTextEdit_textbox->insertPlainText(QString(data));
}

void MainWindow::on_actionExit_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Quit program.");
    msgBox.setInformativeText("Do you want to go?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Yes:
          // Yes was clicked
        QApplication::quit();
          break;
      case QMessageBox::Cancel:
          // Cancel was clicked
          break;
      default:
          // should never be reached
          break;
    }
}

void MainWindow::on_actionUartConnect_triggered()
{
    if (ui->actionUartConnect->isChecked()) // have not connected
    {
        SettingsDialog::Settings p = settings->settings();
        serial->setPortName(p.name);
        serial->setBaudRate(p.baudRate);
        serial->setDataBits(p.dataBits);
        serial->setParity(p.parity);
        serial->setStopBits(p.stopBits);
        serial->setFlowControl(p.flowControl);
        if (serial->open(QIODevice::ReadWrite)) {
            //console->setEnabled(true);
            //console->setLocalEchoEnabled(p.localEchoEnabled);
            ui->actionUartConnect->setIcon(QIcon(":/new/prefix1/gtk-connect.png"));
            ui->actionUartConnect->setToolTip("Click to disconnect UART.");
            ui->actionUartConnect->setChecked(true);
            ui->actionUartConfig->setEnabled(false);
            ui->statusBar->showMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                              .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        } else {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
            ui->statusBar->showMessage("Open error");
        }
    }
    else // connected already
    {
        QMessageBox msgBox;
        msgBox.setText("Disconnect serial port");
        msgBox.setInformativeText("Are you sure?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        switch (ret) {
          case QMessageBox::Yes:
            // Yes was clicked--> disconnect
            if (serial->isOpen())
                serial->close();
            //console->setEnabled(false);
            ui->actionUartConnect->setIcon(QIcon(":/new/prefix1/gtk-disconnect.png"));
            ui->actionUartConnect->setToolTip("Click to connect UART.");
            ui->actionUartConnect->setChecked(false);
            ui->actionUartConfig->setEnabled(true);
            ui->statusBar->showMessage("Disconnected");
              break;
          case QMessageBox::Cancel:
            // Cancel was clicked
            ui->actionUartConnect->setChecked(true);
            break;
          default:
            // should never be reached
            break;
        }


    }
}

void MainWindow::on_actionUartConfig_triggered()
{
    settings->show();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
    QMessageBox::critical(this, QString("Critical Error"), serial->errorString());
    }

}


void MainWindow::on_pushButton_UartSendCommand_clicked()
{
    uart_writeData(ui->lineEdit_UartCommand->text().toLocal8Bit());
}

void MainWindow::on_actionQuickTest_triggered()
{
    QString str = "T"; // Test command in FPGA board
    uart_writeData(str.toLocal8Bit());
}
