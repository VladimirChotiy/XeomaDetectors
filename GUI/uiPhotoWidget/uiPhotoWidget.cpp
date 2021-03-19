#include "uiPhotoWidget.h"
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDebug>

uiPhotoWidget::uiPhotoWidget(const QPixmap &photo, QWidget *parent) :
    QWidget(parent), photoData(photo)
{
    this->setWindowFlags(Qt::Sheet | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    this->setWindowIcon(QIcon(":/Icons/icons/storage.ico"));
    this->setWindowTitle("Просмотр изображения");
    this->setObjectName("fullPhotoWidget");
    this->setWindowModality(Qt::NonModal);
    this->resize(photoData.size());

    QHBoxLayout *photoLayout = new QHBoxLayout(this);
    photoLabel = new QLabel(this);
    photoLayout->addWidget(photoLabel);
    photoLayout->setSpacing(0);
    photoLayout->setMargin(0);
    this->setLayout(photoLayout);
    photoLabel->setPixmap(photoData);
    photoLabel->setScaledContents(true);
    QObject::connect(this, &uiPhotoWidget::onClick, this, &uiPhotoWidget::close);
}

uiPhotoWidget::~uiPhotoWidget()
{
}

QPixmap uiPhotoWidget::getPhotoData() const
{
    return photoData;
}


void uiPhotoWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit this->onClick();
    }
}
