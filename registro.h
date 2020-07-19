#ifndef REGISTRO_H
#define REGISTRO_H

#include <QDialog>

#include"principal.h"

namespace Ui {
class Registro;
}

class Registro : public QDialog
{
    Q_OBJECT

public:
    explicit Registro(QWidget *parent = nullptr);
    ~Registro();
    void setIni(QStringList);
    void setProductos(int, QString, float);
    QString archivosGuardado;

public slots:
    void ingresar();
    void eliminar();
    void guardar();

private:
    Ui::Registro *ui;
};

#endif // REGISTRO_H
