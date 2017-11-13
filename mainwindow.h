#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
    Ui::MainWindow *ui;
    QString get_time_string();
    QString file_read(QString filename);
};

#endif // MAINWINDOW_H
