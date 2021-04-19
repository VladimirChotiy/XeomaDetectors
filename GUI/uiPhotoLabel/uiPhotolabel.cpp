#include "uiPhotolabel.h"
#include <QMouseEvent>

uiPhotolabel::uiPhotolabel(QWidget *parent, Qt::WindowFlags f) :
    QLabel(parent)
{
    Q_UNUSED(f)
    isViewImage = false;
}

uiPhotolabel::~uiPhotolabel()
{

}

void uiPhotolabel::setPhoto(const QPixmap &photo)
{
    if (isViewImage) {
        this->setPixmap(photo);
        this->setContextMenuPolicy(Qt::CustomContextMenu);
    }
}

void uiPhotolabel::viewImage(int mode)
{
    if (mode == 0) {
        isViewImage = false;
    }else {
        isViewImage = true;
    }

    if (isViewImage) {
        this->setText("Ожидание изображения");
    }else {
        this->setContextMenuPolicy(Qt::NoContextMenu);
        this->setPixmap(QPixmap());
        this->setText("Изображения отключены");
    }
}

void uiPhotolabel::mousePressEvent(QMouseEvent *event)
{
    if (isViewImage & (this->pixmap() != nullptr)) {
        if (event->button() == Qt::MouseButton::LeftButton) {
        emit this->clicked();
        }
    }
}
