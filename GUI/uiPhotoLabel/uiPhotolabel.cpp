#include "uiPhotolabel.h"
#include <QMouseEvent>

uiPhotolabel::uiPhotolabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent)
{
    Q_UNUSED(f)
}

uiPhotolabel::~uiPhotolabel()
{

}

void uiPhotolabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        emit this->clicked();
    }
}
