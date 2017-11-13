#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "keyboard/keyboard.h"

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
    void on_pushButton_Quit_clicked();
    void on_pushButton_uart_getport_clicked();
    void on_pushButton_uart_connect_clicked();
    void on_pushButton_write_start_clicked();
    void run_keyboard_lineEdit();
private:
    Ui::MainWindow *ui;
    Keyboard *lineEditkeyboard;
    QString get_time_string();
    QString file_read(QString filename);
    void file_upload_to_host(QString filename,QString user,QString host_ip_path);
};

#endif // MAINWINDOW_H
