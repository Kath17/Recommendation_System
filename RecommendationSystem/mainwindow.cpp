#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "recommender.cpp"
#include <QStandardItem>
#include <QTableView>
#include <QFileDialog>

RecomenderSystem RS;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Boton de ejecutar acción
void MainWindow::on_pushButton_2_clicked()
{
    k = ui->spinBox->value();
    nombre1 = ui->lineEdit_2->text();
    nombre2 = ui->lineEdit_3->text();
    item = ui->lineEdit_4->text();
    umbral = ui->doubleSpinBox->value();

    string nombre_1 = nombre1.toStdString();
    string nombre_2 = nombre2.toStdString();
    string item_ = item.toStdString();

    // ----------------------__TABLA__-----------------------//
    QStandardItemModel *mod = new QStandardItemModel;
    QStandardItem *it = new QStandardItem(QObject::tr("Vecino"));
    mod->setHorizontalHeaderItem(0,it);
    QStandardItem *it1 = new QStandardItem(QObject::tr("Distancia"));
    mod->setHorizontalHeaderItem(1,it1);
    QStandardItem *it2 = new QStandardItem(QObject::tr("Pelicula"));
    mod->setHorizontalHeaderItem(2,it2);
    QStandardItem *it3 = new QStandardItem(QObject::tr("Puntaje"));
    mod->setHorizontalHeaderItem(3,it3);
    ui->tableView->setModel(mod);

    // Llenar tabla
    QStandardItem *item1;
    QStandardItem *item2;

    // ---------------------------------- OPERACIONES ---------------------------------------//

    std::clock_t start2;
    double duration2;
    start2 = std::clock();


    if(opcion=="K-vecinos más cercanos"){
        vector<tuple<float,string>> Vecinos = RS.vecino_cercano(nombre_1,k,distancia.toStdString());
//        print_vector(Vecinos);
        for(int i=0; i<Vecinos.size(); i++)
        {
            item1 = new QStandardItem(QString::fromStdString(get<1>(Vecinos[i])));
            item2 = new QStandardItem(QString::number(get<0>(Vecinos[i])));
            mod->setItem(i,0,item1);
            mod->setItem(i,1,item2);
            ui->tableView->show();
        }
        ui->lineEdit_5->setText("");
     }

     else if(opcion == "Recomendar películas")
    {
       vector <tuple<float,string,int>> rec = RS.RecomendarPorKUsuarios(nombre_1,distancia.toStdString(),k,umbral);
       print_vector(rec);
       for(int i=0; i<rec.size(); i++)
       {
           item1 = new QStandardItem(QString::fromStdString(get<1>(rec[i])));
           item2 = new QStandardItem(QString::number(get<0>(rec[i])));
           mod->setItem(i,2,item1);
           mod->setItem(i,3,item2);
           ui->tableView->show();
       }
       ui->lineEdit_5->setText("");
    }

    else if(opcion == "Hallar puntaje aprox.")
    {
        float res = RS.probabilidad_item(nombre_1,item_,k,distancia.toStdString());
//        QString a = QString::number(res);
        if(res == 0)
        {
            ui->lineEdit_5->setText("El usuario ya vio la película y la calificó con: "+QString::number(RS.data[nombre_1][item_]));
        }
        else {
            ui->lineEdit_5->setText("El puntaje aproximado de "+nombre1+" al item: "+item+" es: "+ QString::number(res));
        }
    }

    else if(opcion=="Comparar dos usuarios")
    {
        float res;
        if(distancia == "Manhattan")
        {
            res = RS.manhattan(nombre_1,nombre_2);
        }
        else if(distancia == "Euclidean")
        {
            res = RS.euclidean(nombre_1,nombre_2);
        }
        else if(distancia == "Pearson")
        {
            res = RS.pearson(nombre_1,nombre_2);
        }
        else if(distancia == "Similitud del Coseno")
        {
            res = RS.sim_coseno(nombre_1,nombre_2);
        }
        ui->lineEdit_5->setText("La distancia de "+distancia+" es: "+QString::number(res));
    }

    duration2 = ( std::clock() - start2 ) / (double) CLOCKS_PER_SEC;
    QString duracion2 = QString::number(duration2);
    ui->label_10->setText("Se realizó la operación en: ");
    ui->lineEdit_7->setText(duracion2+" s");
}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    if(arg1 == "K-vecinos más cercanos")
    {
        opcion = "K-vecinos más cercanos";
    }
    else if(arg1 == "Recomendar películas")
    {
        opcion = "Recomendar películas";
    }
    else if(arg1 == "Hallar puntaje aprox.")
    {
        opcion = "Hallar puntaje aprox.";
    }
    else if(arg1 == "Comparar dos usuarios")
    {
        opcion = "Comparar dos usuarios";
    }
}

void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
    distancia = arg1;
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{

}

void MainWindow::on_pushButton_clicked()
{
    direccion = QFileDialog::getOpenFileName();
    ui->lineEdit->setText(direccion);

    //  ----------------TIMER -------------------//
//    RecomenderSystem RS;

    std::clock_t start;
    double duration;
    start = std::clock();

    RS.load_data(direccion.toStdString(),',');

    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    QString duracion = QString::number(duration);
    ui->label_8->setText("Se cargó la Base de datos en: ");
    ui->lineEdit_6->setText(duracion+" s");
    // ---------------- FINALIZA -------------------//
}
