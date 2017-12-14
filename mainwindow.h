#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "keyboardqwerty/keyboardqwerty.h"
#include "uart/settingsdialog.h"

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QSlider>
#include <QLabel>

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
    void uart_dc_handleError(QSerialPort::SerialPortError error);

    void on_actionUartConnect_triggered();

    void on_actionUartConfig_triggered();

    void on_pushButton_UartSendCommand_clicked();

    void uart_readData();
    void uart_dc_readData();

    void on_actionQuickTest_triggered();

    void on_pushButton_DUT_SRAM_Read_clicked();

    void on_pushButton_DUT_SRAM_Write_clicked();

    void on_pushButton_DUT_SRAM_Write_FF_clicked();

    void on_pushButton_DUT_SRAM_Write_00_clicked();

    void on_pushButton_DUT_SRAM_Write_A5_clicked();

    void block_select_handle();

    void on_pushButton_DUT_SRAM_Write_55_clicked();

    void on_pushButton_DUT_SRAM_SER_test_clicked();

    void ser_check();

    void on_actionUartConfig_2_triggered();

    void on_actionUartDCConnect_triggered();

    void setVDD(int value);

    void on_pushButton_DUT_SRAM_Write_Worst_2_clicked();

private:
    Ui::MainWindow *ui;
    KeyboardQwerty *lineEditkeyboard;
    SettingsDialog *settings;
    QSerialPort *serial;
    SettingsDialog *settings_uart_dc;
    QSerialPort *serial_uart_dc;
    QByteArray *sram_data;
    int selected_block;
    int iWER[17];
    int iSER[17];
    char flag_kind_ER;
    char flag_data_pattern;
    QTimer *timer_ser;
    bool b_ser_started;
    int i_ser_test_counter;
    bool b_ser_get_array;
    int i_ser_array[17][128][256];
    bool b_ref_array[17][128][256];
    bool b_ref_sa[128]; // reference row (fixed data) for Sense amplifier testing
    bool b_fail_col[17][128]; // error columns profile

    QSlider *slider_VDD;
    QLabel *label_VDD_min;
    QLabel *label_VDD_max;
    QLabel *label_VDD_set;

private:
    void file_upload_to_host(QString filename,QString user,QString host_ip_path);
    QString get_time_string();
    QString file_read(QString filename);

    void uart_writeData(const QByteArray &data);
    void uart_dc_writeData(const QByteArray &data);
    void convert_data_to_image(QByteArray byte_data);
    void my_delay( int millisecondsToWait );
    void radiobutton_block_select_setup();
    void calculate_WER(QByteArray byte_data);
    void calculate_RER(QByteArray byte_data);
    void calculate_SER_Standard(QByteArray byte_data);
    void calculate_SER_Random(QByteArray byte_data);
    void calculate_ER();
    void write_report(QString st_data);
    void write_ser_data();
    void init_ref_sa();
    void init_slider_VDD();
};

#endif // MAINWINDOW_H
