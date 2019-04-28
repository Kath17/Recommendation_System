#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <sstream>

using namespace std;

typedef string UserId;
typedef string ItemId;
typedef float RatingVal;

typedef map < UserId, map<ItemId, RatingVal>> db;
//typedef map<string,map<string,float>> db;

struct predicate
{
    bool operator()(const std::tuple<float, string> &left, const std::tuple<float, string> &right)
    {
         return get<0>(left) > get<0>(right);
    }
};

void print_vector(vector<tuple<float,string>> n)
{
    int m = n.size();
    for(int i=0; i< m; i++)
        cout<<"Valor: "<<get<0>(n[i])<<" y vecino : "<<get<1>(n[i])<<endl;
    cout<<endl;
}

void print_vector(vector<tuple<float,string,int>> n)
{
    int m = n.size();
    for(int i=0; i< m; i++)
        cout<<"\nPelicula: "<<get<1>(n[i])<<" ,con puntaje: "<<get<0>(n[i])<<" y "<<get<2>(n[i])<<" veces recomendada";
    cout<<endl;
}

vector<string> split_string_nos(const string& s,char delimit)
{
    vector<string> splitted;
    bool flag = false;
    splitted.push_back("");
    for(int i=0; i<s.size(); ++i)
    {
        if(s[i]=='"')
        {
          // cout<<"Entro a "" "<<endl;
            flag = flag? false : true;
            // splitted[splitted.size()-1] += s[i];  //Comentar para no agregar ""
            continue;
        }

        if(s[i]==delimit && !flag)
            splitted.push_back("");
        else
            splitted[splitted.size()-1] += s[i];
    }
    return splitted;
}

db getBD(string fileName, char delimit){

  db BDatos;
  ifstream File(fileName.c_str());

  if (!File) {
    std::cout << "Error, could not open file." << std::endl;
  }

  vector <string> temp;
  temp.reserve(3);
  string line;

  // ofstream myfile;
  // myfile.open(output);

//  cout<<"Antes de while"<<endl;
  while(getline(File,line))
  {
//      cout<<"Entro al while"<<endl;
      stringstream  lineStream(line);
      string        cell;

      temp = split_string_nos(line, delimit);

      string QString = temp[2];
      istringstream StrToFloat(QString );
      float floatVar;
      StrToFloat >> floatVar;

//      cout<<"Temp: "<<temp[0]<<endl;
//      cout<<"Temp: "<<temp[1]<<endl;
//      cout<<"Temp: "<<temp[2]<<endl;

      BDatos[temp[0]][temp[1]] = floatVar;
      temp.clear();
  }

    File.close();
  // myfile.close();
  return BDatos;
}

class RecomenderSystem{
///define something
public:
    db data;
public:
    db load_data(string filename, char delimit)
    {
        data = getBD(filename,delimit);
        return data;
    }
    void print_db()
    {
        for (auto user: data)
        {
            cout << user.first<<": \n";
            for(auto ratings: user.second)
                cout << "\t"<<ratings.first << " : "<<ratings.second <<endl;
            cout<<endl;
        }
    }

    float manhattan(string user1, string user2)
    {
        float distance = 0;

        for(auto item: data[user1])
        {
            //if (data[user2][item.first] != 0)
            if (data[user2].find(item.first) != data[user2].end())
            {
                distance += fabs(data[user1][item.first] - data[user2][item.first]);
            }
            //cout<<"Distancia: "<<distance<<endl;
        }
        return distance;
    }

    float euclidean(string user1, string user2)
    {
        float distance = 0;

        for(auto item: data[user1])
        {
            if (data[user2][item.first] != 0)
            if (data[user2].find(item.first) != data[user2].end())
                distance += fabs(data[user2][item.first] - data[user1][item.first])*fabs(data[user2][item.first] - data[user1][item.first]);

//            try{
//                distance += fabs(data[user2][item.first] - data[user1][item.first])*fabs(data[user2][item.first] - data[user1][item.first]);
//            }
//            catch (const std::out_of_range& oor){

//            }
        }
        return sqrt(distance);
    }

    float pearson(string user1, string user2)
    {
        float distance = 0;
        float sum_xy = 0;
        float sum_x = 0;
        float sum_y = 0;
        float sum_x2 = 0;
        float sum_x_2 = 0;
        float sum_y2 = 0;
        float sum_y_2 = 0;
        int n = 0;
        for(auto item: data[user1])
        {
            if (data[user2].find(item.first) != data[user2].end())
            {
                n = n +1;
                float x = data[user1][item.first];
                float y = data[user2][item.first];
                sum_xy = sum_xy + x*y;
                sum_x = sum_x + x;
                sum_y = sum_y + y;
                sum_x2 = sum_x2 + x*x;
                sum_y2 = sum_y2 + y*y;
            }
        }
        if (n == 0)
            return 0;
        float denominator = sqrt(sum_x2 - (sum_x*sum_x)/n)*sqrt(sum_y2 - (sum_y*sum_y)/n);
        if (denominator == 0)
            return 0;
        distance = (sum_xy-(sum_x * sum_y)/n)/denominator;
        return distance;
    }

    float sim_coseno(string user1, string user2)
    {
        float distance = 0;
        float dot_xy = 0;
        float length_x = 0;
        float length_y = 0;
        for(auto item: data[user1])
        {
            dot_xy = dot_xy + data[user1][item.first]*data[user2][item.first];
            length_x = length_x + data[user1][item.first]*data[user1][item.first];
        }
        for(auto item: data[user2])
        {
            length_y = length_y + data[user2][item.first]*data[user2][item.first];
        }
        length_x = sqrt(length_x);
        length_y = sqrt(length_y);
        float denominator = length_x * length_y;
        if (denominator == 0)
            return 0;
        distance = dot_xy/denominator;
        return distance;
    }

    vector<tuple<float,string>> vecino_cercano(string user,int k,string algoritmo)
    {
        vector<tuple<float,string>> distanciasTodo;
        if(algoritmo == "Manhattan" || algoritmo == "Euclidean")
        {
            for(auto users: data)
            {
                if (users.first != user)
                    if (algoritmo == "Manhattan")
                        distanciasTodo.push_back(make_tuple(manhattan(user,users.first),users.first));
                    else
                        distanciasTodo.push_back(make_tuple(euclidean(user,users.first),users.first));
            }

            sort(distanciasTodo.begin(),distanciasTodo.end());

            vector<tuple<float,string>> distancias;
            distancias.reserve(k);
            int n = distanciasTodo.size();
            if (n < k)
                k = n;
            for(int i=0; i <k; i++)
                distancias.push_back(distanciasTodo[i]);
//            print_vector(distancias);
            return distancias;

        }
        else if (algoritmo == "Pearson" || algoritmo == "Similitud del Coseno")
        {
            for(auto users: data)
            {
                if (users.first != user)
                    if (algoritmo == "Pearson")
                        distanciasTodo.push_back(make_tuple(pearson(user,users.first),users.first));
                    else
                        distanciasTodo.push_back(make_tuple(sim_coseno(user,users.first),users.first));
            }

            sort(distanciasTodo.rbegin(),distanciasTodo.rend());

            vector<tuple<float,string>> distancias;
            distancias.reserve(k);
            int n = distanciasTodo.size();
            if (n < k)
                k = n;
            for(int i=0; i <k; i++)
                distancias.push_back(distanciasTodo[i]);

//            print_vector(distancias);
            return distancias;
        }
        else
        {
            cout<<"Ingrese una distancia valida"<<endl;

        }
    }

    /** VECINO CERCANO 2*/
    vector<tuple<float,string>> vecino_cercano(string user,int k,string algoritmo,string movie)
    {
        vector<tuple<float,string>> distanciasTodo;
        if(algoritmo == "Manhattan" || algoritmo == "Euclidean")
        {
            for(auto users: data)
            {
                if (users.first != user)
                    if (algoritmo == "Manhattan")
                        distanciasTodo.push_back(make_tuple(manhattan(user,users.first),users.first));
                    else
                        distanciasTodo.push_back(make_tuple(euclidean(user,users.first),users.first));
            }

            sort(distanciasTodo.begin(),distanciasTodo.end());

            vector<tuple<float,string>> distancias;
            distancias.reserve(k);
            int n = distanciasTodo.size();
            if (n < k)
                k = n;
            for(int i=0; i <k; i++)
                distancias.push_back(distanciasTodo[i]);
            //print_vector(distancias);
            return distancias;

        }
        else if (algoritmo == "Pearson" || algoritmo == "Similitud del Coseno")
        {
            for(auto users: data)
            {
                if (users.first != user)
                    if (algoritmo == "Pearson")
                        distanciasTodo.push_back(make_tuple(pearson(user,users.first),users.first));
                    else
                        distanciasTodo.push_back(make_tuple(sim_coseno(user,users.first),users.first));
            }

            sort(distanciasTodo.rbegin(),distanciasTodo.rend());
            //print_vector(distanciasTodo);
            vector<tuple<float,string>> distancias;
            distancias.reserve(k);
            int n = distanciasTodo.size();
            if (n < k)
                k = n;
            int counter = 0;
            while(distancias.size() != k)
            {
                try {
                    float rate = data[get<1>(distanciasTodo[counter])].at(movie);
                    distancias.push_back(distanciasTodo[counter]);

                }
                catch (const std::out_of_range& oor){
                    cout<<"El vecino "<<get<1>(distanciasTodo[counter])<<" no califico la pelicula "<<movie<<endl;

                }
                counter ++;
                if(counter == n)
                    break;

            }
            //print_vector(distancias);
            return distancias;
        }
        else
        {
            cout<<"Ingrese una distancia valida"<<endl;

        }
    }
    /**FIN VECINO CERCANO 2*/

    float probabilidad_item(string user,string movie,int k,string algoritmo)
    {
        try {
            float rating = data[user].at(movie);
            //cout<<"El usuario ya calificó la pelicula con: "<<rating<<endl;
            return 0;
        }
        catch (const std::out_of_range& oor){
            vector<tuple<float,string>> nearestNeighbors = vecino_cercano(user,k,algoritmo,movie);
            print_vector(nearestNeighbors);
            vector<float> calificaiones;
            float total = 0;
            for(int i=0; i<k ; i++)
            {
                float rate = data[get<1>(nearestNeighbors[i])].at(movie);
                calificaiones.push_back(rate);
                //cout<<"El vecino "<<get<1>(nearestNeighbors[i]) << " califico "<<movie<<" con "<<rate<<endl;
                total += get<0>(nearestNeighbors[i]);
            }
            vector<float> porcentajes;
            float rating_proyectado = 0;
            for(int i=0; i<calificaiones.size();i++)
                rating_proyectado += calificaiones[i]*(get<0>(nearestNeighbors[i])/total);
            //cout<<"RATING: "<<rating_proyectado<<endl;
            return rating_proyectado;
        }
    }

    vector<tuple<float,string>> MapToVector( map<ItemId, RatingVal> mapa )
    {
      vector<tuple<float,string>> v;
      for( map<ItemId,RatingVal>::iterator it = mapa.begin(); it != mapa.end(); ++it ) {
        v.push_back( make_tuple(it->second, it->first));
      }
      return v;
    }

    //Ingresar nuevo item en la lista de recomendados
    //Si no esta entonces calcular puntaje con los puntajes dado por los k-usuarios
    //Si ya esta no hacer nada
    void IngresarItems(vector <tuple<float,string,int>> &Recomendados, tuple<float,string> &Nuevo, vector<tuple<float,string>> &MasCercanos)
    {
      if(Recomendados.size()==0){
        //Puntaje de la pelicual get<0>
        float puntaje = 0, nuevoPunt = 0, cont = 0;
        for (unsigned int j = 0; j< MasCercanos.size(); j++)
        {
          try{
            // saca el nombre del usuario cercano y el nombre de la pelicula y lo agrega a nuevo puntaje
            nuevoPunt = data[get<1>(MasCercanos[j])].at(get<1>(Nuevo));
            cont = cont +1;
          }
          catch (const std::out_of_range& oor){
            //Si no encuentra la pelicula en el usuario2
            nuevoPunt = 0;
          }
          puntaje = puntaje + nuevoPunt;
        }
        puntaje = (puntaje*1.0)/(cont*1.0);
        Recomendados.push_back(make_tuple(puntaje,get<1>(Nuevo),cont));
      }
      else
      {
        for (unsigned int i=0; i<Recomendados.size(); i++)
        {
          // Pelicula : get<1> , get<0> rating
          if(get<1>(Recomendados[i]) == get<1>(Nuevo))
          {
            //Si ya esta el item en los recomendados
            cout<<" Ya esta "<<get<1>(Nuevo)<<" en los recomendados"<<endl;
            break;
          }
          else if(i == Recomendados.size()-1)
          {
            float puntaje = 0,nuevoPunt = 0, cont = 0;
            for (unsigned int j = 0; j< MasCercanos.size(); j++)
            {
              try{
                // saca el nombre del usuario cercano y el nombre de la pelicula y lo agrega a nuevo puntaje
                nuevoPunt = data[get<1>(MasCercanos[j])].at(get<1>(Nuevo));
                cont = cont +1;
              }
              catch (const std::out_of_range& oor){
                //Si no encuentra la pelicula en el usuario2
                nuevoPunt = 0;
              }
              puntaje = puntaje + nuevoPunt;
            }
            puntaje = (puntaje*1.0)/(cont*1.0);
            Recomendados.push_back(make_tuple(puntaje,get<1>(Nuevo),cont));
            break;
          }
        }
      }
    }

    // ------------ 3) Recomendar pelicula segun k usuarios más parecidos--------//
    // In: Usuario, algoritmo, k-más parecidos
    //Out: vector< pelicula, rating>
    vector <tuple<float,string,int>> RecomendarPorKUsuarios(string usuario, string algoritmo, int k,float umbral)
    {
      //Recomendados guarda: Puntuacion, Nombre, y numero de veces que ha sido recomendado
      vector <tuple<float,string,int>> Recomendados;
      vector<tuple<float,string>> MasCercanos = vecino_cercano(usuario, k, algoritmo);
      string usuario2;
      // Se guardan los items ordenados de mayor puntuacion a menor puntuacion, en orden alfabetico
      vector <tuple<float,string>> ItemsMayorMenor;

      cout<<"Tamano: "<<MasCercanos.size()<<endl;

      for (unsigned int i=0; i<MasCercanos.size();i++)
      {
        usuario2 = get<1>(MasCercanos[i]);
        // Ordenar peliculas del usuario2 de acuerdo a su puntaje y en orden alfabetico
        ItemsMayorMenor = MapToVector(data[usuario2]);
        stable_sort(ItemsMayorMenor.begin(),ItemsMayorMenor.end(), predicate());
        print_vector(ItemsMayorMenor);

        for (unsigned int j=0; j<ItemsMayorMenor.size();j++)
        {
          //Puntaje , pelicula/libro
          if(get<0>(ItemsMayorMenor[j])>=umbral)
          {
            float puntaje=0;
            try{
              //get <0> Puntajes  get<1>  Pelicula
              puntaje = data[usuario].at(get<1>(ItemsMayorMenor[j]));
              // cout<<usuario<<" ya vio: " <<get<1>(ItemsMayorMenor[j])<<" su puntaje es de: "<<puntaje<<endl;
            }
            catch (const std::out_of_range& oor){
              IngresarItems(Recomendados,ItemsMayorMenor[j],MasCercanos);
            }
          }
        }
        ItemsMayorMenor.clear();
      }
      return Recomendados;
    }
};


//int main()
//{
//    RecomenderSystem r;
//    r.load_data("BDs/BD_1_pre.csv",';');
//    // r.load_data("BDs/BDLibros_pre.csv",';');
//    // r.load_data("BDs/ratings_20m_pre.csv",',');
//    ///r.print_db();

//    ///r.print_db();
//    //cout<<"Manhattan: "<<r.sim_coseno("Angelica","Veronica")<<endl;
//    //cout<<r.sim_coseno("Angelica","Hailey");

//    cout<<"Comenzando la distancia"<<endl;
//    vector<string> distancias = {"manhattan","euclidean","pearson","sim_coseno"};
//    vector <tuple<float,string,int>> respuesta = r.RecomendarPorKUsuarios("Hailey","euclidean",4,4);
//    print_vector(respuesta);
//    // 276736;3257224281;8
//    // cout<<"Manhattan: "<<r.manhattan("276736","100")<<endl;
//    // r.vecino_cercano("Angelica",3,"sim_coseno");
//    // r.vecino_cercano("276736",100,"manhattan");

//    //r.vecino_cercano("Hailey",4,"manhattan");
//    //r.prob_movie("Angelica","Deadmau5",3,"sim_coseno");

//    return 0;
//}
