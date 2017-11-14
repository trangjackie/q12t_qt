#ifndef KEYBOARDQWERTY_H
#define KEYBOARDQWERTY_H

#include <QWidget>
#include <QtGui>
#include <QLineEdit>
#include <QPushButton>

namespace Ui {
class KeyboardQwerty;
}

class KeyboardQwerty : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardQwerty(QWidget *parent = 0);
    void setLineEdit(QLineEdit * );
    ~KeyboardQwerty();
    QPushButton *enterButton;

private slots:
    void keyboardHandler();
    void on_shift_clicked();
    void on_clear_clicked();
    void on_enterButton_clicked();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_lineEdit_returnPressed();
    void on_backButton_clicked();


    void on_ButtonLanguage_clicked();

private:
    Ui::KeyboardQwerty *ui;
    void set_char_button(int language, bool shift);
    QString outputText;
    QLineEdit *outputLineEdit;
    bool shift;
    int language;
};

#endif // KEYBOARDQWERTY_H
