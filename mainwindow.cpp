#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QList>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
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
    settings = new SettingsDialog;
    settings_uart_dc = new SettingsDialog;
    settings_uart_dc->setdefault(1,0);
    settings->setdefault(2,3);
    // UART
    serial = new QSerialPort(this);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial, SIGNAL(readyRead()), this, SLOT(uart_readData()));

    // UART for power DC
    serial_uart_dc = new QSerialPort(this);

    connect(serial_uart_dc, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(uart_dc_handleError(QSerialPort::SerialPortError)));
    connect(serial_uart_dc, SIGNAL(readyRead()), this, SLOT(uart_dc_readData()));

    // Timer for SER auto test
    timer_ser = new QTimer();
    connect(timer_ser, SIGNAL(timeout()), this, SLOT(ser_check()));
    // connect fuction for on-screen keyboard
    connect(ui->lineEdit_host_ip ,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_host_user,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_host_passwd,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_UartCommand,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_Temp,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    connect(ui->lineEdit_VDD,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    // Currently, onscreen keyboard only support line edit
    //connect(ui->plainTextEdit_textbox,SIGNAL(selectionChanged()),this,SLOT(run_keyboard_lineEdit()));
    // SRAM data storage
    sram_data = new QByteArray();
    selected_block = 0;
    radiobutton_block_select_setup();
    flag_kind_ER = 'X';
    flag_data_pattern = 0b00000000;
    init_ref_sa();
    init_slider_VDD();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init_slider_VDD()
{
    label_VDD_max = new QLabel(tr("1.1V"));
    label_VDD_min = new QLabel(tr("0.2V"));
    label_VDD_set = new QLabel(tr("1.0V"));
    slider_VDD = new QSlider(Qt::Horizontal);
    slider_VDD->setTickPosition(QSlider::TicksBelow);
    slider_VDD->setTickInterval(2);
    slider_VDD->setSingleStep(1);
    slider_VDD->setMaximumWidth(380);
    slider_VDD->setValue(2);
    slider_VDD->setMaximum(18);
    slider_VDD->setMinimum(0);
    slider_VDD->setPageStep(1);
    connect(slider_VDD, SIGNAL(valueChanged(int)), this, SLOT(setVDD(int)));
    ui->mainToolBar->addWidget(label_VDD_max);
    ui->mainToolBar->addWidget(slider_VDD);
    ui->mainToolBar->addWidget(label_VDD_min);
    ui->mainToolBar->addWidget(label_VDD_set);
}

void MainWindow::setVDD(int value)
{
    QString str;
    float vdd;
    float step;
    step = 0.05;
    vdd = 1.1 - value*step;
    str = "VSET2:"+QString::number(vdd,'f',3)+"\n";
    qDebug() << str;
    // update set value label
    label_VDD_set->setText(QString::number(vdd)+"V");
    ui->lineEdit_VDD->setText(QString::number(vdd,'f',3));
    // control the DC power
    uart_dc_writeData(str.toLocal8Bit());
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
    if (ui->radioButton_b1tid->isChecked()){selected_block = 16; }

    // show bitmap of selected block (if have)
    if (sram_data->length()>0)
    {
        convert_data_to_image(*sram_data);
    }
}

void MainWindow::write_report(QString st_data)
{
    QString filename = "SRAM_Log_"+ui->lineEdit_host_user->text()+".txt";
    QString st_time = get_time_string();
    QFile file( filename );
    QFileInfo check_file(filename);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) { // add more data to file
        if (file.open(QFile::Append))
        {
                QTextStream stream(&file);
                stream << st_time+" "+st_data+"\n" << endl;
        }
    } else { // open new file
        if ( file.open(QIODevice::ReadWrite) )
        {
            QTextStream stream( &file );
            stream << st_time+" "+st_data+"\n" << endl;
        }
    }
    file.close();
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

void MainWindow::uart_dc_writeData(const QByteArray &data)
{
    serial_uart_dc->write(data);
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

    calculate_ER();
    convert_data_to_image(*sram_data);
}
void MainWindow::uart_dc_readData()
{
    QByteArray data;
    data.append( serial_uart_dc->readAll());
    while (serial_uart_dc->waitForReadyRead(50))
            data.append(serial_uart_dc->readAll());

    if (serial_uart_dc->error() == QSerialPort::ReadError) {
        qDebug("Failed to read from port "+serial_uart_dc->portName().toLatin1()+", error: "+serial_uart_dc->errorString().toLatin1());
    } else if (serial_uart_dc->error() == QSerialPort::TimeoutError && data.isEmpty()) {
        qDebug("No data was currently available for reading from port "+serial_uart_dc->portName().toLatin1());
    }
    qDebug("Data successfully received from port "+serial_uart_dc->portName().toLatin1());
    ui->statusBar->showMessage("Data length "+QString::number(data.length(),10));
    ui->plainTextEdit_textbox->clear();
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

void MainWindow::on_actionUartDCConnect_triggered()
{
    if (ui->actionUartDCConnect->isChecked()) // have not connected
    {
        SettingsDialog::Settings p = settings_uart_dc->settings();
        serial_uart_dc->setPortName(p.name);
        serial_uart_dc->setBaudRate(p.baudRate);
        serial_uart_dc->setDataBits(p.dataBits);
        serial_uart_dc->setParity(p.parity);
        serial_uart_dc->setStopBits(p.stopBits);
        serial_uart_dc->setFlowControl(p.flowControl);
        if (serial_uart_dc->open(QIODevice::ReadWrite)) {
            ui->actionUartDCConnect->setIcon(QIcon(":/new/prefix1/gtk-connect.png"));
            ui->actionUartDCConnect->setToolTip("Click to disconnect UART.");
            ui->actionUartDCConnect->setChecked(true);
            ui->actionUartConfig_2->setEnabled(false);
            ui->statusBar->showMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                              .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                              .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        } else {
            QMessageBox::critical(this, tr("Error"), serial_uart_dc->errorString());
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
            if (serial_uart_dc->isOpen())
                serial_uart_dc->close();
            //console->setEnabled(false);
            ui->actionUartDCConnect->setIcon(QIcon(":/new/prefix1/gtk-disconnect.png"));
            ui->actionUartDCConnect->setToolTip("Click to connect UART.");
            ui->actionUartDCConnect->setChecked(false);
            ui->actionUartConfig_2->setEnabled(true);
            ui->statusBar->showMessage("Disconnected");
              break;
          case QMessageBox::Cancel:
            // Cancel was clicked
            ui->actionUartDCConnect->setChecked(true);
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

void MainWindow::on_actionUartConfig_2_triggered()
{
    settings_uart_dc->show();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
    QMessageBox::critical(this, QString("Critical Error"), serial->errorString());
    }

}
void MainWindow::uart_dc_handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
    QMessageBox::critical(this, QString("Critical Error"), serial_uart_dc->errorString());
    }

}


void MainWindow::on_pushButton_UartSendCommand_clicked()
{
    QString str = ui->lineEdit_UartCommand->text()+"\n";
    uart_dc_writeData(str.toLocal8Bit());
}

void MainWindow::on_actionQuickTest_triggered()
{
    QString str;
    float vdd;
    float step;
    int value = slider_VDD->value();
    step = 0.05;
    vdd = 1.1 - value*step;


    if (ui->actionQuickTest->isChecked()) // turned OFF then turn it ON
    {

        //QString str = "T"; // Test command in FPGA board
         str = "VSET1:1.5\n"; // set Chanel 1 is 1.5V for DUT IO
        uart_dc_writeData(str.toLocal8Bit());
        // set Chanel 2 is xV for DUT core
        str = "VSET2:"+QString::number(vdd,'f',3)+"\n";
        qDebug() << str;
        uart_dc_writeData(str.toLocal8Bit());
        str = "OUT1\n"; // output on
        uart_dc_writeData(str.toLocal8Bit());

            ui->actionQuickTest->setIcon(QIcon(":/new/prefix1/media-playback-stop.svg"));
            ui->actionQuickTest->setChecked(true);
            ui->statusBar->showMessage("DC Power ON");

    }
    else // Turn ON already then turn it OFF
    {
        str = "OUT0\n"; // output  off
        uart_dc_writeData(str.toLocal8Bit());
            ui->actionQuickTest->setIcon(QIcon(":/new/prefix1/media-record.svg"));
            ui->actionQuickTest->setChecked(false);

            ui->statusBar->showMessage("DC Power OFF");

    }
}

void MainWindow::convert_data_to_image(QByteArray byte_data)
{
    char cdata,cmask;

    QImage img(128, 259, QImage::Format_RGB888);
    img.fill(QColor(Qt::white).rgb());

    // data array
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

    // Column profile
    for (int n=0;n<128;n++)
    {
        if (b_fail_col[selected_block][n]){
            img.setPixel(n,256,qRgb(255,0,0)); // if column is fail, red
            img.setPixel(n,257,qRgb(255,0,0)); // if column is fail, red
            img.setPixel(n,258,qRgb(255,0,0)); // if column is fail, red
        }
        else{
            img.setPixel(n,256,qRgb(0,0,255)); // if column is ok, blue
            img.setPixel(n,257,qRgb(0,0,255)); // if column is ok, blue
            img.setPixel(n,258,qRgb(0,0,255)); // if column is ok, blue
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
    flag_kind_ER = 'R';
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

void MainWindow::on_pushButton_DUT_SRAM_Write_55_clicked()
{
    QString str = "6"; // Write data to memory (DUT)
    uart_writeData(str.toLocal8Bit());
    flag_data_pattern = 0b01010101;
    flag_kind_ER = 'W';
}

void MainWindow::calculate_WER(QByteArray byte_data)
{
    for (int i=0;i<17;i++){
        iWER[i]=0;
        for (int x=0;x<128;x++) // init error column profile
        {
            b_fail_col[i][x] = false; // none fail column
        }
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
                        if ((flag_data_pattern&cmask)==cmask)
                        {
                            b_array[16*b+x][255-y] = true;
                        }
                        else
                        {
                            b_array[16*b+x][255-y] = false;
                        }
                    }else{
                        if ((0b01011010&cmask)==cmask)
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
    // First: compare reference row to buil fail column profile
    for (int bl=0;bl<17;bl++)
    {
        for (int x = 0; x < 16; x++)
        {
            if ((bl*16*256+x*256+255)<=(byte_data.length()-1)){
                cdata = byte_data.at(bl*16*256+x*256+255);
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
                if (b_cell!=b_ref_sa[16*b+x]){
                    b_fail_col[bl][16*b+x] = true;
                }
                else{
                    b_fail_col[bl][16*b+x] = false;
                }
                cmask = cmask<<1;
            }
        }
    }
    // Second:
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
                    //if ((b_cell!=b_array[16*b+x][255-y])&(!b_fail_col[bl][16*b+x])){ // do not count the error on fail columns
                    if (b_cell!=b_array[16*b+x][255-y]){
                        i_error +=1;
                    }
                    cmask = cmask<<1;
                }
            }
        }
        iWER[bl]=i_error; // do not compare the 255th row

    }
    // update data to labels
    ui->label_WER_b0->setText(QString::number( iWER[0]));
    ui->label_WER_b1->setText(QString::number( iWER[1]));
    ui->label_WER_b2->setText(QString::number( iWER[2]));
    ui->label_WER_b3->setText(QString::number( iWER[3]));
    ui->label_WER_b4->setText(QString::number( iWER[4]));
    ui->label_WER_b5->setText(QString::number( iWER[5]));
    ui->label_WER_b6->setText(QString::number( iWER[6]));
    ui->label_WER_b7->setText(QString::number( iWER[7]));
    ui->label_WER_b8->setText(QString::number( iWER[8]));
    ui->label_WER_b9->setText(QString::number( iWER[9]));
    ui->label_WER_b10->setText(QString::number( iWER[10]));
    ui->label_WER_b11->setText(QString::number( iWER[11]));
    ui->label_WER_b12->setText(QString::number( iWER[12]));
    ui->label_WER_b13->setText(QString::number( iWER[13]));
    ui->label_WER_b14->setText(QString::number( iWER[14]));
    ui->label_WER_b15->setText(QString::number( iWER[15]));
    ui->label_WER_b1b->setText(QString::number( iWER[16]));
}

void MainWindow::calculate_RER(QByteArray byte_data)
{
    for (int i=0;i<17;i++){
        iSER[i]=0;
        for (int x=0;x<128;x++) // init error column profile
        {
            b_fail_col[i][x] = false; // none fail column
        }
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
                        if ((flag_data_pattern&cmask)==cmask)
                        {
                            b_array[16*b+x][255-y] = true;
                        }
                        else
                        {
                            b_array[16*b+x][255-y] = false;
                        }
                    }else{
                        if ((0b01011010&cmask)==cmask)
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
    // if any column is fail, the missmatch on that colunm is not counted.
    // First: compare reference row to buil fail column profile
    for (int bl=0;bl<17;bl++)
    {
        for (int x = 0; x < 16; x++)
        {
            if ((bl*16*256+x*256+255)<=(byte_data.length()-1)){
                cdata = byte_data.at(bl*16*256+x*256+255);
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
                if (b_cell!=b_ref_sa[16*b+x]){
                    b_fail_col[bl][16*b+x] = true;
                }
                else{
                    b_fail_col[bl][16*b+x] = false;
                }
                cmask = cmask<<1;
            }
        }
    }
    // Second: compare data
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
                    //if ((b_cell!=b_array[16*b+x][255-y])&(!b_fail_col[bl][16*b+x])){
                    if (b_cell!=b_array[16*b+x][255-y]){
                        i_error +=1;
                    }
                    cmask = cmask<<1;
                }
            }
        }
        iSER[bl]=i_error; // do not compare the 255th row

    }
    // update data to labels
    ui->label_SER_b0->setText(QString::number( iSER[0]));
    ui->label_SER_b1->setText(QString::number( iSER[1]));
    ui->label_SER_b2->setText(QString::number( iSER[2]));
    ui->label_SER_b3->setText(QString::number( iSER[3]));
    ui->label_SER_b4->setText(QString::number( iSER[4]));
    ui->label_SER_b5->setText(QString::number( iSER[5]));
    ui->label_SER_b6->setText(QString::number( iSER[6]));
    ui->label_SER_b7->setText(QString::number( iSER[7]));
    ui->label_SER_b8->setText(QString::number( iSER[8]));
    ui->label_SER_b9->setText(QString::number( iSER[9]));
    ui->label_SER_b10->setText(QString::number( iSER[10]));
    ui->label_SER_b11->setText(QString::number( iSER[11]));
    ui->label_SER_b12->setText(QString::number( iSER[12]));
    ui->label_SER_b13->setText(QString::number( iSER[13]));
    ui->label_SER_b14->setText(QString::number( iSER[14]));
    ui->label_SER_b15->setText(QString::number( iSER[15]));
    ui->label_SER_b1b->setText(QString::number( iSER[16]));
}

void MainWindow::calculate_ER()
{
    QString st_data;
    st_data = ui->lineEdit_Temp->text() +" "+ ui->lineEdit_VDD->text() + " ";
    if(flag_kind_ER=='W'){
        calculate_WER(*sram_data);
        st_data += "W "+QString::number(flag_data_pattern,2);
        for (int i=0;i<17;i++){
            st_data += " "+QString::number(iWER[i]);
        }
        write_report(st_data);
    }
    else if(flag_kind_ER=='R')
    {
        calculate_RER(*sram_data);
        st_data += "R "+QString::number(flag_data_pattern,2);
        for (int i=0;i<17;i++){
            st_data += " "+QString::number(iSER[i]);
        }
        write_report(st_data);
    }
    else if(flag_kind_ER=='E')
    {
        calculate_SER_Standard(*sram_data);
    }
    else if(flag_kind_ER=='e')
    {
        calculate_SER_Random(*sram_data);
    }

}



void MainWindow::on_pushButton_DUT_SRAM_SER_test_clicked()
{
    QString st_data;
    if (timer_ser->isActive())
    {
        timer_ser->stop();
        st_data += "E "+ QString::number(i_ser_test_counter);
        for (int i=0;i<17;i++){
            st_data += " "+QString::number(iSER[i]);
        }
        write_report(st_data);
        write_ser_data();
    }
    else
    {
        // get timer time set
        int t;
        bool bOk;
        t= ui->lineEdit_timer->text().toInt(&bOk);
        if (!bOk|(t<10))
        {
            t = 10;
        }
        flag_kind_ER = 'E';
        // Start counter
        i_ser_test_counter = 0;
        b_ser_get_array = true; // First time reading data is for reference, do not calculate SER
        timer_ser->start(t*1000);
    }
}

void MainWindow::on_pushButton_DUT_SRAM_Write_Worst_2_clicked()
{
    QString st_data;
    if (timer_ser->isActive())
    {
        timer_ser->stop();
        st_data += "Rand "+ QString::number(i_ser_test_counter);
        for (int i=0;i<17;i++){
            st_data += " "+QString::number(iSER[i]);
        }
        write_report(st_data);
        write_ser_data();
    }
    else
    {
        // get timer time set
        int t;
        bool bOk;
        t= ui->lineEdit_timer->text().toInt(&bOk);
        if (!bOk|(t<10))
        {
            t = 10;
        }
        flag_kind_ER = 'e';
        // Start counter
        i_ser_test_counter = 0;
        b_ser_get_array = true; // First time reading data is for reference, do not calculate SER
        timer_ser->start(t*1000);
    }
}

void MainWindow::ser_check()
{

    i_ser_test_counter += 1;
    QString strx = QString::number(i_ser_test_counter);
    ui->label_SER_counted_test->setText(strx);

    // Send "ser" command to FPGA
    QString str;
    if (flag_kind_ER == 'E')
    {
        str = "E"; // check SER with standard data pattern
    }
    else
    {
        str = "e"; // check SER with updated data
    }

    uart_writeData(str.toLocal8Bit());

}

void MainWindow::calculate_SER_Standard(QByteArray byte_data)
{
    // estimate error rate
    char cdata,cmask;
    bool b_cell;
    bool b_ref;
    int i_error;

    if (b_ser_get_array)
    {
        b_ser_get_array = false;
        for (int i=0;i<17;i++){
            iSER[i]=0;
        }



        for (int bl=0;bl<17;bl++)
        {
            for (int x = 0; x < 16; x++)
            {
                for (int y = 0; y < 256; y++)
                {
                    for (int b = 0; b<8;b++)
                    {
                        i_ser_array[bl][16*b+x][255-y] = 0;
                    }
                }
            }
        }

    }



    // compare to reference array
    // if any column is fail, the missmatch on that colunm is not counted.
    // First: compare reference row to buil fail column profile
    for (int bl=0;bl<17;bl++)
    {
        for (int x = 0; x < 16; x++)
        {
            if ((bl*16*256+x*256+255)<=(byte_data.length()-1)){
                cdata = byte_data.at(bl*16*256+x*256+255);
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
                if (b_cell!=b_ref_sa[16*b+x]){
                    b_fail_col[bl][16*b+x] = true;
                }
                else{
                    b_fail_col[bl][16*b+x] = false;
                }
                cmask = cmask<<1;
            }
        }
    }
    // Second: compare data
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
                    if (flag_data_pattern==0b10100101)
                    {
                        if((y%2)==0){
                            if ((flag_data_pattern&cmask)==cmask)
                            {
                                b_ref = true;
                            }
                            else
                            {
                                b_ref = false;
                            }
                        }else{
                            if ((0b01011010&cmask)==cmask)
                            {
                                b_ref = true;
                            }
                            else
                            {
                                b_ref = false;
                            }
                        }
                    }
                    else
                    {
                        if ((flag_data_pattern&cmask)==cmask)
                        {
                            b_ref = true;
                        }
                        else
                        {
                            b_ref = false;
                        }
                    }

                    if ((cdata&cmask)==cmask)
                    {
                        b_cell = true;
                    }
                    else
                    {
                        b_cell = false;
                    }
                    //if ((b_cell!=b_array[16*b+x][255-y])&(!b_fail_col[bl][16*b+x])){
                    if (b_cell!=b_ref){
                        i_ser_array[bl][16*b+x][255-y] += 1;
                        iSER[bl] +=1;
                    }
                    cmask = cmask<<1;
                }
            }
        }


    }
    // update data to labels
    ui->label_SER_b0->setText(QString::number( iSER[0]));
    ui->label_SER_b1->setText(QString::number( iSER[1]));
    ui->label_SER_b2->setText(QString::number( iSER[2]));
    ui->label_SER_b3->setText(QString::number( iSER[3]));
    ui->label_SER_b4->setText(QString::number( iSER[4]));
    ui->label_SER_b5->setText(QString::number( iSER[5]));
    ui->label_SER_b6->setText(QString::number( iSER[6]));
    ui->label_SER_b7->setText(QString::number( iSER[7]));
    ui->label_SER_b8->setText(QString::number( iSER[8]));
    ui->label_SER_b9->setText(QString::number( iSER[9]));
    ui->label_SER_b10->setText(QString::number( iSER[10]));
    ui->label_SER_b11->setText(QString::number( iSER[11]));
    ui->label_SER_b12->setText(QString::number( iSER[12]));
    ui->label_SER_b13->setText(QString::number( iSER[13]));
    ui->label_SER_b14->setText(QString::number( iSER[14]));
    ui->label_SER_b15->setText(QString::number( iSER[15]));
    ui->label_SER_b1b->setText(QString::number( iSER[16]));
}
void MainWindow::calculate_SER_Random(QByteArray byte_data)
{
    char cdata,cmask;

    bool b_cell;

    if (b_ser_get_array)
    {
        b_ser_get_array = false;
        for (int i=0;i<17;i++){
            iSER[i]=0;
        }

        for (int bl=0;bl<17;bl++)
        {
            // get array data
            for (int x = 0; x < 16; x++)
            {
                for (int y = 0; y < 256; y++)
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
                        b_ref_array[bl][16*b+x][255-y] = b_cell;
                        i_ser_array[bl][16*b+x][255-y] = 0;
                        cmask = cmask<<1;
                    }
                }
            }
        }
    }
    else  // compare new data and update ref_array
    {
        for (int bl=0;bl<17;bl++)
        {
            // get array data
            for (int x = 0; x < 16; x++)
            {
                for (int y = 0; y < 255; y++) // Do not compare the 255th row
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

                        if (b_ref_array[bl][16*b+x][255-y] != b_cell)
                        {
                            i_ser_array[bl][16*b+x][255-y] += 1;
                            b_ref_array[bl][16*b+x][255-y] = b_cell;
                            iSER[bl] += 1;
                        }
                        cmask = cmask<<1;
                    }
                }
            }
        }
    }

    // update data to labels
    ui->label_SER_b0->setText(QString::number( iSER[0]));
    ui->label_SER_b1->setText(QString::number( iSER[1]));
    ui->label_SER_b2->setText(QString::number( iSER[2]));
    ui->label_SER_b3->setText(QString::number( iSER[3]));
    ui->label_SER_b4->setText(QString::number( iSER[4]));
    ui->label_SER_b5->setText(QString::number( iSER[5]));
    ui->label_SER_b6->setText(QString::number( iSER[6]));
    ui->label_SER_b7->setText(QString::number( iSER[7]));
    ui->label_SER_b8->setText(QString::number( iSER[8]));
    ui->label_SER_b9->setText(QString::number( iSER[9]));
    ui->label_SER_b10->setText(QString::number( iSER[10]));
    ui->label_SER_b11->setText(QString::number( iSER[11]));
    ui->label_SER_b12->setText(QString::number( iSER[12]));
    ui->label_SER_b13->setText(QString::number( iSER[13]));
    ui->label_SER_b14->setText(QString::number( iSER[14]));
    ui->label_SER_b15->setText(QString::number( iSER[15]));
    ui->label_SER_b1b->setText(QString::number( iSER[16]));
}

void MainWindow::write_ser_data( )
{
    QString st_data;
    QString st_time = get_time_string();
    QString filename = "SER_data_"+ui->lineEdit_host_user->text()+"_"+ui->lineEdit_Temp->text()+"_"+ui->lineEdit_VDD->text()+"_"+st_time+".txt";
    QFile file( filename );
    QFileInfo check_file(filename);
    bool b_file_opened;
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) { // add more data to file
        b_file_opened = file.open(QFile::Append);
    } else { // open new file
        b_file_opened = file.open(QFile::ReadWrite);
    }
    if ( b_file_opened)
    {
        for (int i=0;i<17;i++)
        {
            st_data = "block_"+QString::number(i);
            QTextStream stream( &file );
            stream << st_data << endl;
            for (int y=255;y>=0;y--)
            {
                st_data ="";
                for (int x=0;x<128;x++)
                {
                    st_data += " "+QString::number(i_ser_array[i][x][y]);
                }
                QTextStream stream( &file );
                stream << st_data << endl;
            }
        }
    }
    file.close();
}

void MainWindow::init_ref_sa()
{
    bool b_value;
    b_value = false;
    for (int x=0;x<32;x++)
    {
        for (int b=0;b<4;b++)
        {
            b_ref_sa[x*4+b] = b_value;
        }
        b_value = !b_value;
    }
    for (int i=0;i<17;i++){
        for (int x=0;x<128;x++) // init error column profile
        {
            b_fail_col[i][x] = false; // none fail column
        }
    }
}






