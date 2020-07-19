#ifndef PRINCIPAL_H
#define PRINCIPAL_H

#include <QMainWindow>

#include "producto.h"

#include "notaventa.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Principal; }
QT_END_NAMESPACE

class Principal : public QMainWindow
{
    Q_OBJECT

public:
    Principal(QWidget *parent = nullptr);
    ~Principal();
    QString acum;

public slots:
    void mostrarPrecio(int);
    void agregarProducto();
    void cedula();         //Validar cedula
    void terminado();
    void email();          //Validar email
    void cliente();        //Validar el conmre del cliente
    void telefono();       //Validar para el ingreso del numero de telefono
    void imprimirIni();
    void imprimirInfo();
    void consumidor();
    void notaVenta();

private:
    Ui::Principal *ui;
    QList<Producto*> m_productos;

    void inicializarDatos();
    void inicializarWidgets();
    float m_subtotal;
    void calcular(float subtotal);
    bool registrar(Producto *producto, int cantidad);


};
#endif // PRINCIPAL_H
