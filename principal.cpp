#include "principal.h"
#include "ui_principal.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "registro.h"
#include <QRegExp>
#include <QMessageBox>

Principal::Principal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Principal)
{
    ui->setupUi(this);
    connect(ui->inEmail, SIGNAL(editingFinished()), this, SLOT(email()));
     connect(ui->inNombre, SIGNAL(editingFinished()), this, SLOT(cliente()));
     connect(ui->inTelefono, SIGNAL(editingFinished()), this, SLOT(telefono()));
     connect(ui->btnRealizar, SIGNAL(clicked(bool)), this, SLOT(terminado()));
     connect(ui->mnuProductos, SIGNAL(triggered(bool)), this, SLOT(imprimirInfo()));
     connect(ui->btnConsumidorFinal, SIGNAL(clicked(bool)), this, SLOT(consumidor()));
     connect(ui->btnNotaventa, SIGNAL(clicked(bool)), this, SLOT(notaVenta()));
     ui->inCedula->setValidator(new QIntValidator);
     ui->inNombre->setEnabled(false);
     ui->inTelefono->setEnabled(false);
     ui->inEmail->setEnabled(false);
     ui->inDireccion->setEnabled(false);
    inicializarDatos();
     m_subtotal = 0;
}

Principal::~Principal()
{
    delete ui;
}

void Principal::mostrarPrecio(int index)
{
   //Obtener el precio del producto seleccionado
   float precio = m_productos.at(index)->precio();
   //Colocar el precio en la etiqueta correspondiente
   ui->outPrecio->setText("$ " + QString::number(precio));
}

void Principal::agregarProducto()
{
    //Obteniendo datos desde la interfaz
    int index = ui->inProducto->currentIndex();
    Producto *p = m_productos.at(index);
    int cantidad = ui->inCantidad->value();
    if(cantidad==0){
          return;
    }
    //Calcular subtotal
    float subtotal = cantidad * p->precio();
    int posicion = ui->outDetalle->rowCount();

    if (!registrar(p,cantidad)){


           //agregar datos a la tabla
           ui->outDetalle->insertRow(posicion);
           ui->outDetalle->setItem(posicion,0,new QTableWidgetItem(QString::number(cantidad)));
           ui->outDetalle->setItem(posicion,1,new QTableWidgetItem(p->nombre()));
           ui->outDetalle->setItem(posicion,2,new QTableWidgetItem("$  "+QString::number(subtotal)));
       }

       //Limpiar Datos
       ui->inCantidad->setValue(0);
       ui->inProducto->setFocus();


       //Calcular valores y mostrar
       calcular(subtotal);

   }

   void Principal::terminado()
   {

       QMessageBox msgBox;
       Notaventa *dlgNotaventa = new Notaventa() ;
       QString subtotal = ui->outSubtota->text();
       QString iva = ui->outIva->text();
       QString total = ui->outTotal->text();
       QString nombre = ui->inNombre->text();
       QString cedula = ui->inCedula->text();
       QString telefono = ui->inTelefono->text();
       QString correo = ui->inEmail->text();
       QString direccion = ui->inDireccion->toPlainText();
       if(nombre == "\0" || cedula== "\0" || telefono== "\0" || correo== "\0" || direccion== "\0"){
           msgBox.setText("Datos incompletos");
           msgBox.exec();
           return;
       }
       QString resultado = "Cedula: " + cedula + "\n";
       resultado += "Cliente: " + nombre + "\n";
       resultado += "Telefono: " + telefono + "\n";
       resultado += "E-mail: " + correo + "\n" +
               "Direccion: " + direccion;

       QStringList lista ;
       QString listaP;
       for(int columna = 0; columna<ui->outDetalle->columnCount(); ++columna){
           lista << ui->outDetalle->horizontalHeaderItem(columna)->data(Qt::DisplayRole).toString();
       }
       lista << "\n";
       for(int fila =0; fila<ui->outDetalle->rowCount();  ++fila){
           for (int columna = 0;columna<ui->outDetalle->columnCount(); ++columna) {
               lista <<  ui->outDetalle->item(fila, columna)->text();
               QTableWidgetItem *item = ui->outDetalle->item(fila, columna);
               if(!item || item->text().isEmpty()){
                   ui->outDetalle->setItem(fila , columna, new QTableWidgetItem("0"));
               }
               listaP = lista.join("     ");
               lista << "   ";
           }
           lista << "\n";
           dlgNotaventa->setCantidad(listaP);
       }
       listaP = lista.join(";");
       dlgNotaventa->setInfo(resultado);
       dlgNotaventa->setSubTotal(subtotal);
       dlgNotaventa->setIva(iva);
       dlgNotaventa->setTotal(total);
       dlgNotaventa->show();
   }

   void Principal::email()
   {
       QString value = ui->inEmail->text();
       QRegularExpression regex("[A-z0-9_\\.-]+@[A-z0-9\\.]+\\.[A-Za-z]{2,4}\\b");
       if(regex.match(value).hasMatch())
       {
           ui->inDireccion->setFocus();
           ui->inEmail->setStyleSheet("color: green");
       }else{
           ui->inEmail->setStyleSheet("color: red");
           ui->statusbar->showMessage("E-mail está incorrecto",5000);
           ui->inEmail->selectAll();
       }
   }

   void Principal::cliente()
   {
       QString cliente = ui->inNombre->text();
       QRegularExpression regex("^[a-zA-Z0-9]+$");
       if(!(regex.match(cliente).hasMatch()))
       {
           ui->inTelefono->setFocus();
           ui->inNombre->setStyleSheet("color: green");
       }else{
           ui->inNombre->setStyleSheet("color: red");
           ui->statusbar->showMessage("Nombre está incorrecto",5000);
           ui->inNombre->selectAll();
       }
   }

   void Principal::telefono()
   {
       QString telefono = ui->inTelefono->text();
       if( telefono.size() < 10){
           ui->inTelefono->setStyleSheet("color : red");
           ui->statusbar->showMessage("Número de telefono incorrecto",5000);
           ui->inTelefono->selectAll();
       }else{
           ui->inTelefono->setStyleSheet("color : green");
           ui->inEmail->setFocus();
       }

   }

   void Principal::imprimirIni()
   {
       Registro *dlgRegistro = new Registro();
       QStringList inicio = {"Cantidad", "Producto", "Sub Total"};
       dlgRegistro->setIni(inicio);
   }

  void Principal::imprimirInfo()
   {
       imprimirIni();
       Registro *dlgRegistro = new Registro();

       // Abrir archivos
       QFile archivo("productos.csv");
       qDebug() << archivo.exists();

       // Leer el archivo de productos
       if(archivo.open(QFile::ReadOnly)){
           QTextStream in(&archivo);
           while (!in.atEnd()){
               QString linea = in.readLine();

               QStringList datos = linea.split(';');
               int codigo = datos[0].toInt();
               QString producto = datos[1];
               float precio = datos[2].toFloat();
               qDebug() << codigo << producto << precio;
               dlgRegistro->setProductos(codigo, producto, precio);
           }
       }else{
           qDebug() << "Error al abrir el archivo";
       }

       archivo.close();
       dlgRegistro->show();
   }


   void Principal::consumidor()
   {
       ui->inCedula->setText("1234567890");
       ui->inNombre->setText("Consumidor Final");
       ui->inTelefono->setText("9999999999");
       ui->inEmail->setText("consumidor@example.com");
       ui->inDireccion->setPlainText("*--------------*");
       ui->inCedula->setEnabled(false);
   }

   void Principal::notaVenta()
   {
       ui->inCedula->setEnabled(true);
       cedula();
       ui->inCedula->clear();
       email();
       ui->inEmail->clear();
       cliente();
       ui->inNombre->clear();
       telefono();
       ui->inTelefono->clear();
       ui->inDireccion->clear();
   }

   void Principal::calcular(float subtotal)
   {
       m_subtotal+=subtotal;
       float iva= m_subtotal+0.12;
       float total = m_subtotal+iva;

       ui->outSubtota->setText(QString::number(m_subtotal));
       ui->outIva->setText(QString::number(iva));
       ui->outTotal->setText(QString::number(total));
   }

   void Principal::cedula()
   {
       QString parCedula = ui->inCedula->text();
       qDebug() << "";
       parCedula=ui->inCedula->text();
       QString digitoRegion=parCedula.mid(0,2);
       int digRegion=digitoRegion.toInt();
       int total=0,acu=0, decena=0,bandera=0;
       QString v_val;
       if (parCedula.size() == 10)
       {
           if(digRegion>0 && digRegion<25){
               bandera=1;
               // VALICIÓN DE CEDULA
               if (bandera==1){
                   for (int i =0;i<=8;i++)
                   {
                       QChar C_cedulaa=parCedula.at(i);
                       QString C_cedulaa2=C_cedulaa;
                       if(i%2==0)
                       {
                           if ((C_cedulaa2.toInt()*2)>9)
                           {
                               acu = acu+( C_cedulaa2.toInt()*2)-9;
                           }
                           else
                           {
                               acu= acu+ (C_cedulaa2.toInt()*2);
                           }
                       }
                       else
                       {
                           acu= acu+ (C_cedulaa2.toInt());
                       }
                   }
                   v_val= (QString::number(acu)).mid(0,1);
                   decena= (v_val.toInt()+1)*10;
                   total=decena-acu;


                   if(total==(digitoRegion=parCedula.mid(9,10)).toInt())
                   {
                       ui->inNombre->setEnabled(true);
                       ui->inEmail->setEnabled(true);
                       ui->inTelefono->setEnabled(true);
                       ui->inDireccion->setEnabled(true);
                       ui->inNombre->setFocus();
                       ui->inCedula->setStyleSheet("color: green");
                       ui->inCedula->setEnabled(false);

                   }
                   else
                   {
                       ui->statusbar->showMessage("Número de cédula incorrecto",5000);
                       ui->inCedula->setStyleSheet("color: red");
                       ui->inCedula->selectAll();
                   }
               }

           }else{

               ui->statusbar->showMessage("Número de cédula incorrecto",5000);
               ui->inCedula->setStyleSheet("color: red");
               ui->inCedula->selectAll();
           }


       }else{
           ui->statusbar->showMessage("Número de cédula incorrecto",5000);
           ui->inCedula->setStyleSheet("color: red");
           ui->inCedula->selectAll();
       }
   }


   void Principal::inicializarDatos()
   {
       // abrir archivos
       QFile archivo("productos.csv");

       // leer el archivo de productos
       if(archivo.open(QFile::ReadOnly)){
           QTextStream in(&archivo);
           while (!in.atEnd()){
               QString linea = in.readLine();
               //separar datos por ;
               QStringList datos = linea.split(';');
               int codigo = datos[0].toInt();
               float precio = datos[2].toFloat();
               // Crear productos a la lista
               m_productos.append(new Producto(codigo,datos[1],precio));
           }

       }else{
           qDebug() << "Error al abrir el archivo";
       }

       archivo.close();
       //Inicializar los widgets
       inicializarWidgets();

   }

   void Principal::inicializarWidgets()
   {
       // Agregar nombre de los productos al combo box
       for (int i = 0; i < m_productos.length(); ++i) {
           ui->inProducto->addItem(m_productos.at(i)->nombre());
       }

       // Colocar cabecera en la tabla
       QStringList cabecera = {"Cantidad", "Producto", "Sub Total"};
       ui->outDetalle->setColumnCount(3);
       ui->outDetalle->setHorizontalHeaderLabels(cabecera);

       // Eventos
       connect(ui->inProducto, SIGNAL(currentIndexChanged(int)), this, SLOT(mostrarPrecio(int)));
       connect(ui->cmdAgregar, SIGNAL(clicked(bool)), this, SLOT(agregarProducto()));
       connect(ui->inCedula,SIGNAL(editingFinished()),this,SLOT(cedula()));


       // Mostrar el precio del primer producto
       mostrarPrecio(0);
   }

   bool Principal::registrar(Producto *producto, int cantidad)
   {
       //Recorrer la tabla
       int numFilas = ui->outDetalle->rowCount();
       for(int i=0;i<numFilas;i++){
           QTableWidgetItem *item =ui->outDetalle->item(i,1);
           QString dato=item->data(Qt::DisplayRole).toString();
           if(dato==producto->nombre()){
               //Obtener la cantidad del producto que ya esta en el detalle
               QTableWidgetItem *item= ui->outDetalle->item(i,0);
               int cantActual =item->data(Qt::DisplayRole).toInt();
               //Sumar productos
               int cantNueva = cantActual+cantidad;
               //Calcular nuevo Subtotal
               float subtotal = cantNueva*producto->precio();
               //Actualizar
               ui->outDetalle->setItem(i,0,new QTableWidgetItem(QString::number(cantNueva)));

               ui->outDetalle->setItem(i,2,new QTableWidgetItem("$  "+QString::number(subtotal)));
               return true;

           }
       }
       return false;
   }
