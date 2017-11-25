#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "keyboardqwerty/keyboardqwerty.h"
#include "uart/settingsdialog.h"

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void run_keyboard_lineEdit();
    void on_actionExit_triggered();

    void handleError(QSerialPort::SerialPortError error);

    void on_actionUartConnect_triggered();

    void on_actionUartConfig_triggered();

    void on_pushButton_UartSendCommand_clicked();

    void uart_readData();

    void on_actionQuickTest_triggered();

    void on_pushButton_DUT_SRAM_Read_clicked();

    void on_pushButton_DUT_SRAM_Write_clicked();

    void on_pushButton_DUT_SRAM_Write_FF_clicked();

    void on_pushButton_DUT_SRAM_Write_00_clicked();

    void on_pushButton_DUT_SRAM_Write_A5_clicked();

private:
    Ui::MainWindow *ui;
    KeyboardQwerty *lineEditkeyboard;
    SettingsDialog *settings;
    QSerialPort *serial;

private:
    void file_upload_to_host(QString filename,QString user,QString host_ip_path);
    QString get_time_string();
    QString file_read(QString filename);

    void uart_writeData(const QByteArray &data);
    void convert_data_to_image(QByteArray byte_data);
    void my_delay( int millisecondsToWait );
};

#endif // MAINWINDOW_H
