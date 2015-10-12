#include "bookmarkvaluelineedit.h"

#include <QKeyEvent>
#include <QDebug>
#include <QEvent>

#include <QMovie>

BookmarkValueLineEdit::BookmarkValueLineEdit(QWidget *parent) :
    QLineEdit(parent),
    keyPressed(false)
{
    installEventFilter(this);
    connect(this, &QLineEdit::textChanged,
            this, &BookmarkValueLineEdit::onTextChanged);
}

BookmarkValueLineEdit::~BookmarkValueLineEdit()
{

}

bool BookmarkValueLineEdit::isKeyPressed()
{
    return keyPressed;
}

void BookmarkValueLineEdit::onTextChanged(const QString &text)
{
    // This function will be called after event filter, so we can reset the indicator
    if(text.isEmpty())
    {
        qDebug() << "Resetting the LE";
        keyPressed = false;
    }
}

bool BookmarkValueLineEdit::eventFilter(QObject *object, QEvent *event)
{
    if( (!keyPressed) && event->type() == QEvent::KeyRelease && object == this)
    {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        int key = keyEvent->key();
        if(key != Qt::Key_Backspace ||
           key != Qt::Key_Delete)
        {
            // This is something that can change the value inside
            qDebug() << tr("KeyDown (%1)").arg(key);
            keyPressed = true;
        }
    }

    // Do nothing, there is no need to filter any event in this function
    return false;
}

