#ifndef UIPHOTOWIDGET_H
#define UIPHOTOWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QPixmap;
class QLabel;
QT_END_NAMESPACE

class uiPhotoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit uiPhotoWidget(const QPixmap &photo, QWidget *parent = nullptr);
    ~uiPhotoWidget();
    QPixmap getPhotoData() const;

private:
    QPixmap photoData;
    QLabel *photoLabel;

signals:
    void onClick();

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};

#endif // UIPHOTOWIDGET_H
