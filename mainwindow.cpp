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
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QTime>

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
    // SRAM data storage
    sram_data = new QByteArray();
    selected_block = 0;
    radiobutton_block_select_setup();
    flag_kind_ER = 'X';
    flag_data_pattern = 0b00000000;
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

void MainWindow::radiobutton_block_select_setup()
{
    connect(ui->radioButton_b0 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b1 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b2 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b3 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b4 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b5 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b6 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b7 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b8 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b9 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b10 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b11 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b12 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b13 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b14 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b15 ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
    connect(ui->radioButton_b1tid ,SIGNAL(clicked()),this,SLOT(block_select_handle()));
}

void MainWindow::block_select_handle()
{
    if (ui->radioButton_b0->isChecked()){selected_block = 0; }
    if (ui->radioButton_b1->isChecked()){selected_block = 1; }
    if (ui->radioButton_b2->isChecked()){selected_block = 2; }
    if (ui->radioButton_b3->isChecked()){selected_block = 3; }
    if (ui->radioButton_b4->isChecked()){selected_block = 4; }
    if (ui->radioButton_b5->isChecked()){selected_block = 5; }
    if (ui->radioButton_b6->isChecked()){selected_block = 6; }
    if (ui->radioButton_b7->isChecked()){selected_block = 7; }
    if (ui->radioButton_b8->isChecked()){selected_block = 8; }
    if (ui->radioButton_b9->isChecked()){selected_block = 9; }
    if (ui->radioButton_b10->isChecked()){selected_block = 10; }
    if (ui->radioButton_b11->isChecked()){selected_block = 11; }
    if (ui->radioButton_b12->isChecked()){selected_block = 12; }
    if (ui->radioButton_b13->isChecked()){selected_block = 13; }
    if (ui->radioButton_b14->isChecked()){selected_block = 14; }
    if (ui->radioButton_b15->isChecked()){selected_block = 15; }
    //if (ui->radioButton_b1tid->isChecked()){selected_block = 16; }

    // show bitmap of selected block (if have)
    if (sram_data->length()>0)
    {
        convert_data_to_image(*sram_data);
    }
}

//void MainWindow::on_pushButton_write_start_clicked()
//{
//    QString filename = "Write_"+get_time_string()+".txt";
//    QString file_content;

//    QFile file( filename );
//    if ( file.open(QIODevice::ReadWrite) )
//    {
//        QTextStream stream( &file );
//        stream << "something" << endl;
//    }

//    file.close();
//    // read back for verifing the file content
//    file_content = file_read(filename);
//    ui->plainTextEdit_textbox->document()->setPlainText(file_content);

//    // upload file to server
//    file_upload_to_host(filename,ui->lineEdit_host_user->text(),ui->lineEdit_host_ip->text());
//}

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
    sram_data->clear();
    sram_data->append(serial->readAll());

    while (serial->waitForReadyRead(50))
            sram_data->append(serial->readAll());

    if (serial->error() == QSerialPort::ReadError) {
        qDebug("Failed to read from port "+serial->portName().toLatin1()+", error: "+serial->errorString().toLatin1());
    } else if (serial->error() == QSerialPort::TimeoutError && sram_data->isEmpty()) {
        qDebug("No data was currently available for reading from port "+serial->portName().toLatin1());
    }

    qDebug("Data successfully received from port "+serial->portName().toLatin1());
    ui->statusBar->showMessage("Data length "+QString::number(sram_data->length(),10));
    ui->plainTextEdit_textbox->clear();
    ui->plainTextEdit_textbox->insertPlainText(QString(*sram_data));

    convert_data_to_image(*sram_data);

    calculate_ER();
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

void MainWindow::convert_data_to_image(QByteArray byte_data)
{
    char cdata,cmask;

    QImage img(128, 256, QImage::Format_RGB888);
    img.fill(QColor(Qt::white).rgb());

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            if ((selected_block*16*256+x*256+y)<=(byte_data.length()-1)){
                cdata = byte_data.at(selected_block*16*256+x*256+y);
            }
            else {
                cdata = 0;
            }
            cmask = 0x01;
            for (int b = 0; b<8;b++)
            {
                if ((cdata&cmask)==cmask)
                {
                    img.setPixel(16*b+x, 255-y, qRgb(255, 255, 255));
                }
                else
                {
                    img.setPixel(16*b+x, 255-y, qRgb(0, 0, 0));
                }
                cmask = cmask<<1;
                // show data on bitmap
                //ui->label_bitmap->setPixmap(QPixmap::fromImage(img));
                //my_delay(10);
            }
        }
    }

    // show data on bitmap
    ui->label_bitmap->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::my_delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void MainWindow::on_pushButton_DUT_SRAM_Read_clicked()
{
    QString str = "U"; // Read all data from memory (DUT)
    uart_writeData(str.toLocal8Bit());
    flag_kind_ER = 'S';
}

void MainWindow::on_pushButton_DUT_SRAM_Write_clicked()
{
    QString str = "A"; // Write data to memory (DUT)
    uart_writeData(str.toLocal8Bit());
    flag_data_pattern = 0b10101010;
    flag_kind_ER = 'W';
}

void MainWindow::on_pushButton_DUT_SRAM_Write_FF_clicked()
{
    QString str = "F"; // Write data to memory (DUT)
    uart_writeData(str.toLocal8Bit());
    flag_data_pattern = 0b11111111;
    flag_kind_ER = 'W';
}

void MainWindow::on_pushButton_DUT_SRAM_Write_00_clicked()
{
    QString str = "0"; // Write data to memory (DUT)
    uart_writeData(str.toLocal8Bit());
    flag_data_pattern = 0b00000000;
    flag_kind_ER = 'W';
}

void MainWindow::on_pushButton_DUT_SRAM_Write_A5_clicked()
{
    QString str = "5"; // Write data to memory (DUT)
    uart_writeData(str.toLocal8Bit());
    flag_data_pattern = 0b10100101;
    flag_kind_ER = 'W';
}

void MainWindow::calculate_WER(QByteArray byte_data)
{
    for (int i=0;i<17;i++){
        fWER[i]=0;
    }

    // estimate error rate
    char cdata,cmask;
    bool b_array[128][256];
    bool b_cell;
    int i_error;
    // generate reference array
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            cmask = 0x01;
            for (int b = 0; b<8;b++)
            {
                if (flag_data_pattern==0b10100101)
                {
                    if((y%2)==0){
                        if ((0b01011010&cmask)==cmask)
                        {
                            b_array[16*b+x][255-y] = true;
                        }
                        else
                        {
                            b_array[16*b+x][255-y] = false;
                        }
                    }else{
                        if ((flag_data_pattern&cmask)==cmask)
                        {
                            b_array[16*b+x][255-y] = true;
                        }
                        else
                        {
                            b_array[16*b+x][255-y] = false;
                        }
                    }
                }
                else
                {
                    if ((flag_data_pattern&cmask)==cmask)
                    {
                        b_array[16*b+x][255-y] = true;
                    }
                    else
                    {
                        b_array[16*b+x][255-y] = false;
                    }
                }
                cmask = cmask<<1;
            }
        }
    }
    // compare to reference array
    for (int bl=0;bl<17;bl++)
    {
        i_error = 0;
        // get array data
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 255; y++) // do not compare the 255th row
            {
                if ((bl*16*256+x*256+y)<=(byte_data.length()-1)){
                    cdata = byte_data.at(bl*16*256+x*256+y);
                }
                else {
                    cdata = 0;
                }
                cmask = 0x01;
                for (int b = 0; b<8;b++)
                {
                    if ((cdata&cmask)==cmask)
                    {
                        b_cell = true;
                    }
                    else
                    {
                        b_cell = false;
                    }
                    if (b_cell!=b_array[16*b+x][255-y]){
                        i_error +=1;
                    }
                    cmask = cmask<<1;
                }
            }
        }
        fWER[bl]=(float)(i_error); // do not compare the 255th row

    }
    // update data to labels
    ui->label_WER_b0->setText(QString::number( fWER[0], 'f', 0 ));
    ui->label_WER_b1->setText(QString::number( fWER[1], 'f', 0 ));
    ui->label_WER_b2->setText(QString::number( fWER[2], 'f', 0 ));
    ui->label_WER_b3->setText(QString::number( fWER[3], 'f', 0 ));
    ui->label_WER_b4->setText(QString::number( fWER[4], 'f', 0 ));
    ui->label_WER_b5->setText(QString::number( fWER[5], 'f', 0 ));
    ui->label_WER_b6->setText(QString::number( fWER[6], 'f', 0 ));
    ui->label_WER_b7->setText(QString::number( fWER[7], 'f', 0 ));
    ui->label_WER_b8->setText(QString::number( fWER[8], 'f', 0 ));
    ui->label_WER_b9->setText(QString::number( fWER[9], 'f', 0 ));
    ui->label_WER_b10->setText(QString::number( fWER[10], 'f', 0 ));
    ui->label_WER_b11->setText(QString::number( fWER[11], 'f', 0 ));
    ui->label_WER_b12->setText(QString::number( fWER[12], 'f', 0 ));
    ui->label_WER_b13->setText(QString::number( fWER[13], 'f', 0 ));
    ui->label_WER_b14->setText(QString::number( fWER[14], 'f', 0 ));
    ui->label_WER_b15->setText(QString::number( fWER[15], 'f', 0 ));
    ui->label_WER_b1b->setText(QString::number( fWER[16], 'f', 0 ));
}

void MainWindow::calculate_SER(QByteArray byte_data)
{
    for (int i=0;i<17;i++){
        fSER[i]=0;
    }

    // estimate error rate
    char cdata,cmask;
    bool b_array[128][256];
    bool b_cell;
    int i_error;
    // generate reference array
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            cmask = 0x01;
            for (int b = 0; b<8;b++)
            {
                if (flag_data_pattern==0b10100101)
                {
                    if((y%2)==0){
                        if ((0b01011010&cmask)==cmask)
                        {
                            b_array[16*b+x][255-y] = true;
                        }
                        else
                        {
                            b_array[16*b+x][255-y] = false;
                        }
                    }else{
                        if ((flag_data_pattern&cmask)==cmask)
                        {
                            b_array[16*b+x][255-y] = true;
                        }
                        else
                        {
                            b_array[16*b+x][255-y] = false;
                        }
                    }
                }
                else
                {
                    if ((flag_data_pattern&cmask)==cmask)
                    {
                        b_array[16*b+x][255-y] = true;
                    }
                    else
                    {
                        b_array[16*b+x][255-y] = false;
                    }
                }
                cmask = cmask<<1;
            }
        }
    }
    // compare to reference array
    for (int bl=0;bl<17;bl++)
    {
        i_error = 0;
        // get array data
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 255; y++) // do not compare the 255th row
            {
                if ((bl*16*256+x*256+y)<=(byte_data.length()-1)){
                    cdata = byte_data.at(bl*16*256+x*256+y);
                }
                else {
                    cdata = 0;
                }
                cmask = 0x01;
                for (int b = 0; b<8;b++)
                {
                    if ((cdata&cmask)==cmask)
                    {
                        b_cell = true;
                    }
                    else
                    {
                        b_cell = false;
                    }
                    if (b_cell!=b_array[16*b+x][255-y]){
                        i_error +=1;
                    }
                    cmask = cmask<<1;
                }
            }
        }
        fSER[bl]=(float)(i_error); // do not compare the 255th row

    }
    // update data to labels
    ui->label_SER_b0->setText(QString::number( fSER[0], 'f', 0 ));
    ui->label_SER_b1->setText(QString::number( fSER[1], 'f', 0 ));
    ui->label_SER_b2->setText(QString::number( fSER[2], 'f', 0 ));
    ui->label_SER_b3->setText(QString::number( fSER[3], 'f', 0 ));
    ui->label_SER_b4->setText(QString::number( fSER[4], 'f', 0 ));
    ui->label_SER_b5->setText(QString::number( fSER[5], 'f', 0 ));
    ui->label_SER_b6->setText(QString::number( fSER[6], 'f', 0 ));
    ui->label_SER_b7->setText(QString::number( fSER[7], 'f', 0 ));
    ui->label_SER_b8->setText(QString::number( fSER[8], 'f', 0 ));
    ui->label_SER_b9->setText(QString::number( fSER[9], 'f', 0 ));
    ui->label_SER_b10->setText(QString::number( fSER[10], 'f', 0 ));
    ui->label_SER_b11->setText(QString::number( fSER[11], 'f', 0 ));
    ui->label_SER_b12->setText(QString::number( fSER[12], 'f', 0 ));
    ui->label_SER_b13->setText(QString::number( fSER[13], 'f', 0 ));
    ui->label_SER_b14->setText(QString::number( fSER[14], 'f', 0 ));
    ui->label_SER_b15->setText(QString::number( fSER[15], 'f', 0 ));
    ui->label_SER_b1b->setText(QString::number( fSER[16], 'f', 0 ));
}

void MainWindow::calculate_ER()
{
    if(flag_kind_ER=='W'){
        calculate_WER(*sram_data);
    }
    else if(flag_kind_ER=='S')
    {
        calculate_SER(*sram_data);
    }
}
