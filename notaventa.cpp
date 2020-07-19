#include "notaventa.h"
#include "ui_notaventa.h"

#include "principal.h"
#include "producto.h"
#include <QDebug>

Notaventa::Notaventa(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Notaventa)
{
    ui->setupUi(this);
}

Notaventa::~Notaventa()
{
    delete ui;
}

void Notaventa::setInfo(QString info)
{
    ui->outInfo->setPlainText(info);
}

void Notaventa::setSubTotal(QString precioSub)
{
    ui->outSubtotal->setText(precioSub);
}

void Notaventa::setIva(QString precioIva)
{
    ui->outIva->setText(precioIva);
}

void Notaventa::setTotal(QString precioTot)
{
    ui->outTotal->setText(precioTot);
}

void Notaventa::setCantidad(QString cantidad)
{
    ui->outDetalle->setPlainText(cantidad);

}

