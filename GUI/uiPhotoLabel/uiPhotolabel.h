#ifndef UIPHOTOLABEL_H
#define UIPHOTOLABEL_H

#include <QLabel>
#include <QWidget>

class uiPhotolabel : public QLabel
{
    Q_OBJECT
public:
    explicit uiPhotolabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~uiPhotolabel();

signals:
    void clicked();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
};

#endif // UIPHOTOLABEL_H
