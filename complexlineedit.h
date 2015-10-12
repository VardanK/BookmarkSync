#ifndef COMPLEXLINEEDIT_H
#define COMPLEXLINEEDIT_H

#include <QWidget>

namespace Ui {
class ComplexLineEdit;
}

class ComplexLineEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ComplexLineEdit(QWidget *parent = 0);
    ~ComplexLineEdit();

public slots:
    void startAnimation();
    void stopAnimation();

private:
    bool eventFilter(QObject * watched, QEvent * event);
    void animate(bool start = true);

private:
    Ui::ComplexLineEdit *ui;
    QString styleSheet;
};

#endif // COMPLEXLINEEDIT_H
