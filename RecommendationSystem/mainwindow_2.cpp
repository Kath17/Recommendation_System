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
    cout<<"Int: "<<RS.ultimo_usuario<<endl;

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
        if(k > 0)
        {
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
        else {
            ui->lineEdit_5->setText("Ingrese un K mayor a 0");
        }
     }

     else if(opcion == "Recomendar películas")
    {
        if( k>0 )
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
            ui->lineEdit_5->setText("Su Umbral es de :"+QString::number(umbral));
        }
        else {
            ui->lineEdit_5->setText("Ingrese un K mayor a 0");
        }
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
        if(nombre_1 != "" && nombre_2 != "" )
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
        else {
             ui->lineEdit_5->setText("Ingresar a ambos usuarios");
        }
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

void MainWindow::on_pushButton_4_clicked()
{
    // ----------------------__TABLA__-----------------------//
    QStandardItemModel *mod = new QStandardItemModel;
    QStandardItem *it1 = new QStandardItem(QObject::tr("ID"));
    mod->setHorizontalHeaderItem(0,it1);
    QStandardItem *it2 = new QStandardItem(QObject::tr("Pelicula"));
    mod->setHorizontalHeaderItem(1,it2);
    QStandardItem *it3 = new QStandardItem(QObject::tr("Puntaje"));
    mod->setHorizontalHeaderItem(2,it3);
    ui->tableView->setModel(mod);

    //Agregar nuevo usuario
    RS.ultimo_usuario = RS.ultimo_usuario +1;
    QString id = QString::number(RS.ultimo_usuario);

    QStandardItem *item_u;
    item_u = new QStandardItem(id);
    mod->setItem(0,0,item_u);

    // Llenar tabla
//    QStandardItem *item;
    QStandardItem *item1;
    QStandardItem *item2;
//    item = new QStandardItem("");
//    mod->setItem(0,0,item);

    for(int i=0; i<30;i++)
    {
        item1 = new QStandardItem("");
        item2 = new QStandardItem("");
        mod->setItem(i,1,item1);
        mod->setItem(i,2,item2);
    }
    ui->tableView->show();
}

void MainWindow::on_pushButton_3_clicked()
{

    vector <string> temp;
    temp.reserve(4);

    int numFilas = ui->tableView->verticalHeader()->count();
//    QString id = ui->tableView->model()->data(ui->tableView->model()->index(0,0)).toString();
    QString id = QString::number(RS.ultimo_usuario);

    ofstream File(direccion.toStdString().c_str(),std::ios_base::app | std::ios_base::out);
    for (int i=0; i<numFilas; i++)
    {
        QString pelicula = ui->tableView->model()->data(ui->tableView->model()->index(i,1)).toString();
        if(pelicula == "")
            break;
        QString puntaje = ui->tableView->model()->data(ui->tableView->model()->index(i,2)).toString();

        string QString_ = puntaje.toStdString();
        istringstream StrToFloat(QString_ );
        float floatVar;
        StrToFloat >> floatVar;

        cout<<"Temp: "<<pelicula.toStdString()<<endl;
        cout<<"Temp: "<<floatVar<<endl;

        RS.data[id.toStdString()][pelicula.toStdString()] = floatVar;

        File<<id.toStdString()+","+'"'+pelicula.toStdString()+'"'+","+puntaje.toStdString()+"\n";
    }
    File.close( );
    cout<<"Se agrego: " <<RS.data[id.toStdString()]["A"]<<endl;
}
