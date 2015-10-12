#include "complexlineedit.h"
#include "ui_complexlineedit.h"

#include <QMouseEvent>
#include <QMovie>


ComplexLineEdit::ComplexLineEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComplexLineEdit)
{
    ui->setupUi(this);

    // Configure the spinner
    // TODO: This is temporary code, POC only
    int size = ui->lineEdit->geometry().height() / 2;

    QSize defaultSize(12, 12);
    QMovie *spinnerMovie = new QMovie(":/new/main/spinner.gif");
    spinnerMovie->setScaledSize(defaultSize);
    spinnerMovie->setSpeed(85);
    ui->labelSpinBox->setMinimumSize(defaultSize);
    ui->labelSpinBox->setMovie(spinnerMovie);
    ui->labelSpinBox->hide();

    styleSheet =
            "QLabel#labelSpinBox {padding: 4px;}\n"
            "QFrame#lineEditFrame {border: 1px solid; border-color: rgb(%1);}\n"
            "QFrame#lineEditFrame:hover {border-color: rgb(128,180,234);}";

    setStyleSheet(styleSheet.arg("171,173,179"));

    ui->lineEdit->installEventFilter(this);
    ui->labelSpinBox->installEventFilter(this);
}

ComplexLineEdit::~ComplexLineEdit()
{
    delete ui;
}

bool ComplexLineEdit::eventFilter(QObject * object, QEvent * event)
{
    if(object == ui->lineEdit)
    {
        if(event->type() == QEvent::FocusIn)
        {
            setStyleSheet(styleSheet.arg("86,157,229"));
            startAnimation();
        }
        else if(event->type() == QEvent::FocusOut)
        {
            setStyleSheet(styleSheet.arg("171,173,179"));
            animate(false);
        }
    }
    else if(object == ui->labelSpinBox)
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
            if(mouseEvent && (mouseEvent->button() == Qt::LeftButton))
            {
                ui->lineEdit->clearFocus();
                ui->lineEdit->setFocus();
            }
        }
    }

    return false;
}

void ComplexLineEdit::startAnimation()
{
    animate(true);
}



void ComplexLineEdit::stopAnimation()
{
    animate(false);
}

void ComplexLineEdit::animate(bool start)
{
    QMovie *movie = ui->labelSpinBox->movie();
    if(movie)
    {
        if(start)
        {
            ui->labelSpinBox->show();
            movie->start();
        }
        else
        {
            movie->stop();
            ui->labelSpinBox->hide();
        }
    }
}
