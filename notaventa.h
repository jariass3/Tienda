#ifndef NOTAVENTA_H
#define NOTAVENTA_H

#include <QDialog>

namespace Ui {
class Notaventa;
}

class Notaventa : public QDialog
{
    Q_OBJECT

public:
    explicit Notaventa(QWidget *parent = nullptr);
    ~Notaventa();
    void setInfo(QString info);
    void setSubTotal(QString precioSub);
    void setIva(QString precioIva);
    void setTotal(QString precioTot);
    void setCantidad(QString cantidad);

private:
    Ui::Notaventa *ui;
};

#endif // NOTAVENTA_H
