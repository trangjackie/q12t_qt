#include "keyboardqwerty.h"
#include "ui_keyboardqwerty.h"

KeyboardQwerty::KeyboardQwerty(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyboardQwerty)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: rgba(0, 0,0, 70);"
                        "color: rgb(255, 255, 255);"
                        "font: bold 24px FreeSans;"
                        "border-style: outset;"
                        "border-width: 2px;"
                        "border-radius: 10px;"
                        "border-color: beige");
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlags(Qt::FramelessWindowHint);


    connect ( ui->Buttonq, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonw, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttone, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonr, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttont, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttony, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonu, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttoni, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttono, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonp, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonbracketleft, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonbracketright, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonbackslash, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );


    connect ( ui->Buttona, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttons, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttond, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonf, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttong, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonh, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonj, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonk, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonl, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonsemicolon, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonprime, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    connect ( ui->Buttonz, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonx, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonc, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonv, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonb, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonn, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonm, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttoncomma, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttondot, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonstroke, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    connect ( ui->Button0, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button1, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button2, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button3, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button4, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button5, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button6, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button7, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button8, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Button9, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonminor, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );
    connect ( ui->Buttonequal, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    connect ( ui->space, SIGNAL( clicked() ), this, SLOT( keyboardHandler() ) );

    outputText = "";
    shift = false;
    language = 0;
    ui->lineEdit->setFocus();

}

KeyboardQwerty::~KeyboardQwerty()
{
    delete ui;
}


void KeyboardQwerty::keyboardHandler()
{
    QPushButton *button = (QPushButton *)sender();
    //qDebug() << "pressed " << button->text();

    QString inputText = button->text();

    if (inputText == "Space")
    {
        outputText += " ";
    }
    else if(inputText == "&&")
    {
        outputText += "&";
    }
    else if(inputText == "\\")
    {
        outputText += ui->Buttonbackslash->text() ;
    }
    else
    {
            outputText += inputText;
    }


    ui->lineEdit->setText(outputText);

}

void KeyboardQwerty::setLineEdit(QLineEdit * line)
{
    outputLineEdit = line;
}


void KeyboardQwerty::on_shift_clicked()
{
    if (shift==false){
        shift = true;
        ui->shift->setStyleSheet("background-color: rgba(0, 0,0, 100);"
                                 "color: rgb(20, 170, 255);");

    }
    else {
        shift = false;
        ui->shift->setStyleSheet("background-color: rgba(0, 0,0, 70);"
                                 "color: rgb(255, 255, 255);");
    }
    KeyboardQwerty::set_char_button(language,shift);
}



void KeyboardQwerty::on_clear_clicked()
{
    outputText="";
    ui->lineEdit->setText(outputText);
}



void KeyboardQwerty::on_enterButton_clicked()
{
    //qDebug() << "enter";
    outputLineEdit->setText(outputText);
    outputText="";
    ui->lineEdit->setText(outputText);
    close();
}

void KeyboardQwerty::on_lineEdit_textChanged(const QString &arg1)
{
    outputText = arg1;
    ui->lineEdit->setText(outputText);

}

void KeyboardQwerty::on_lineEdit_returnPressed()
{
    //qDebug() << "enter";
    outputLineEdit->setText(outputText);
    outputText="";
    ui->lineEdit->setText(outputText);
    close();
}

void KeyboardQwerty::on_backButton_clicked()
{
    outputText.remove(outputText.length()-1,outputText.length());
    ui->lineEdit->setText(outputText);
}

void KeyboardQwerty::set_char_button(int language, bool shift)
{
    // number and special chars
    if (!shift)
    {
        ui->Button0->setText("0");
        ui->Button1->setText("1");
        ui->Button2->setText("2");
        ui->Button3->setText("3");
        ui->Button4->setText("4");
        ui->Button5->setText("5");
        ui->Button6->setText("6");
        ui->Button7->setText("7");
        ui->Button8->setText("8");
        ui->Button9->setText("9");
        ui->Buttonminor->setText("-");
        ui->Buttonequal->setText("=");

        ui->Buttonbracketleft->setText("[");
        ui->Buttonbracketright->setText("]");
        ui->Buttonbackslash->setText(trUtf8("\\"));

        ui->Buttonsemicolon->setText(";");
        ui->Buttonprime->setText("'");

        ui->Buttoncomma->setText(",");
        ui->Buttondot->setText(".");
        ui->Buttonstroke->setText("/");
    }
    else{
        ui->Button0->setText(")");
        ui->Button1->setText("!");
        ui->Button2->setText("@");
        ui->Button3->setText("#");
        ui->Button4->setText("$");
        ui->Button5->setText("%");
        ui->Button6->setText("^");
        ui->Button7->setText("&&");
        ui->Button8->setText("*");
        ui->Button9->setText("(");
        ui->Buttonminor->setText("_");
        ui->Buttonequal->setText("+");

        ui->Buttonbracketleft->setText("{");
        ui->Buttonbracketright->setText("}");
        ui->Buttonbackslash->setText(trUtf8("|"));

        ui->Buttonsemicolon->setText(":");
        ui->Buttonprime->setText("\"");

        ui->Buttoncomma->setText("<");
        ui->Buttondot->setText(">");
        ui->Buttonstroke->setText("?");
    }

    // alphabet chars
    if (language==0)
    {
        if (!shift)
        {
            ui->Buttonq->setText("q");
            ui->Buttonw->setText("w");
            ui->Buttone->setText("e");
            ui->Buttonr->setText("r");
            ui->Buttont->setText("t");
            ui->Buttony->setText("y");
            ui->Buttonu->setText("u");
            ui->Buttoni->setText("i");
            ui->Buttono->setText("o");
            ui->Buttonp->setText("p");

            ui->Buttona->setText("a");
            ui->Buttons->setText("s");
            ui->Buttond->setText("d");
            ui->Buttonf->setText("f");
            ui->Buttong->setText("g");
            ui->Buttonh->setText("h");
            ui->Buttonj->setText("j");
            ui->Buttonk->setText("k");
            ui->Buttonl->setText("l");

            ui->Buttonz->setText("z");
            ui->Buttonx->setText("x");
            ui->Buttonc->setText("c");
            ui->Buttonv->setText("v");
            ui->Buttonb->setText("b");
            ui->Buttonn->setText("n");
            ui->Buttonm->setText("m");
        }
        else
        {
            ui->Buttonq->setText("Q");
            ui->Buttonw->setText("W");
            ui->Buttone->setText("E");
            ui->Buttonr->setText("R");
            ui->Buttont->setText("T");
            ui->Buttony->setText("Y");
            ui->Buttonu->setText("U");
            ui->Buttoni->setText("I");
            ui->Buttono->setText("O");
            ui->Buttonp->setText("P");


            ui->Buttona->setText("A");
            ui->Buttons->setText("S");
            ui->Buttond->setText("D");
            ui->Buttonf->setText("F");
            ui->Buttong->setText("G");
            ui->Buttonh->setText("H");
            ui->Buttonj->setText("J");
            ui->Buttonk->setText("K");
            ui->Buttonl->setText("L");


            ui->Buttonz->setText("Z");
            ui->Buttonx->setText("X");
            ui->Buttonc->setText("C");
            ui->Buttonv->setText("V");
            ui->Buttonb->setText("B");
            ui->Buttonn->setText("N");
            ui->Buttonm->setText("M");
        }
    }
    else if (language==1)
    {
        if (!shift)
        {
            ui->Buttonq->setText("ㅂ");
            ui->Buttonw->setText("ㅈ");
            ui->Buttone->setText("ㄷ");
            ui->Buttonr->setText("ㄱ");
            ui->Buttont->setText("ㅅ");
            ui->Buttony->setText("ㅛ");
            ui->Buttonu->setText("ㅕ");
            ui->Buttoni->setText("ㅑ");
            ui->Buttono->setText("ㅐ");
            ui->Buttonp->setText("ㅔ");

            ui->Buttona->setText("ㅁ");
            ui->Buttons->setText("ㄴ");
            ui->Buttond->setText("ㅇ");
            ui->Buttonf->setText("ㄹ");
            ui->Buttong->setText("ㅎ");
            ui->Buttonh->setText("ㅗ");
            ui->Buttonj->setText("ㅓ");
            ui->Buttonk->setText("ㅏ");
            ui->Buttonl->setText("ㅣ");

            ui->Buttonz->setText("ㅋ");
            ui->Buttonx->setText("ㅌ");
            ui->Buttonc->setText("ㅊ");
            ui->Buttonv->setText("ㅍ");
            ui->Buttonb->setText("ㅠ");
            ui->Buttonn->setText("ㅜ");
            ui->Buttonm->setText("ㅡ");
        }
        else
        {
            ui->Buttonq->setText("ㅃ");
            ui->Buttonw->setText("ㅉ");
            ui->Buttone->setText("ㄸ");
            ui->Buttonr->setText("ㄲ");
            ui->Buttont->setText("ㅆ");
            ui->Buttony->setText("ㅛ");
            ui->Buttonu->setText("ㅕ");
            ui->Buttoni->setText("ㅑ");
            ui->Buttono->setText("ㅒ");
            ui->Buttonp->setText("ㅖ");


            ui->Buttona->setText("ㅁ");
            ui->Buttons->setText("ㄴ");
            ui->Buttond->setText("ㅇ");
            ui->Buttonf->setText("ㄹ");
            ui->Buttong->setText("ㅎ");
            ui->Buttonh->setText("ㅗ");
            ui->Buttonj->setText("ㅓ");
            ui->Buttonk->setText("ㅏ");
            ui->Buttonl->setText("ㅣ");


            ui->Buttonz->setText("ㅋ");
            ui->Buttonx->setText("ㅌ");
            ui->Buttonc->setText("ㅊ");
            ui->Buttonv->setText("ㅍ");
            ui->Buttonb->setText("ㅠ");
            ui->Buttonn->setText("ㅜ");
            ui->Buttonm->setText("ㅡ");
        }
    }
}

void KeyboardQwerty::on_ButtonLanguage_clicked()
{
    if (language<1)
    {
        language += 1;
    }
    else
    {
        language = 0;
    }
    KeyboardQwerty::set_char_button(language,shift);
}
