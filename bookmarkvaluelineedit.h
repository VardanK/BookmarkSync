#ifndef BOOKMARKVALUELINEEDIT_H
#define BOOKMARKVALUELINEEDIT_H

#include <QLineEdit>

class QEvent;

class BookmarkValueLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    BookmarkValueLineEdit(QWidget *parent = 0);
    ~BookmarkValueLineEdit();

public:
    bool isKeyPressed();

private slots:
    void onTextChanged(const QString &text);

private:
    bool eventFilter(QObject* object, QEvent* event);

private:
    bool keyPressed;
};

#endif // BOOKMARKVALUELINEEDIT_H
