#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "keyboardqwerty/keyboardqwerty.h"

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
    void on_pushButton_uart_getport_clicked();
    void on_pushButton_uart_connect_clicked();
    void on_pushButton_write_start_clicked();
    void run_keyboard_lineEdit();
    void on_actionExit_triggered();

    void on_actionUartConnect_triggered();

private:
    Ui::MainWindow *ui;
    KeyboardQwerty *lineEditkeyboard;
    QString get_time_string();
    QString file_read(QString filename);
    void file_upload_to_host(QString filename,QString user,QString host_ip_path);
};

#endif // MAINWINDOW_H
