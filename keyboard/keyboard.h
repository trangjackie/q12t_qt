#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QtGui>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class Keyboard;
}

class Keyboard : public QWidget
{
    Q_OBJECT
    
public:
    explicit Keyboard(QWidget *parent = 0);
    void setLineEdit(QLineEdit * );
    ~Keyboard();
    QPushButton *enterButton;
    
private slots:
    void keyboardHandler();
    void on_shift_clicked();
    void on_char_2_toggled(bool checked);
    void on_clear_clicked();
    void on_enterButton_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_lineEdit_returnPressed();

    void on_backButton_clicked();
    void set_char_button(int char_type);
private:
    Ui::Keyboard *ui;
    QString outputText;
    QLineEdit *outputLineEdit;
    bool shift;
};

#endif // KEYBOARD_H
