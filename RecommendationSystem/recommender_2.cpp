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

typedef map < UserId, map <ItemId, RatingVal> > db;

struct predicate
{
    bool operator()(const std::tuple<float, string> &left, const std::tuple<float, string> &right){
         return get<0>(left) > get<0>(right);
    }
};

struct predicate_2
{
    bool operator()(const std::tuple<float, string,int> &left, const std::tuple<float, string,int> &right){
         return get<0>(left) > get<0>(right);
    }
};

void print_vector(vector<tuple<float,string>> n)
{
    for(unsigned int i=0; i< n.size(); i++)
        cout<<"Valor: "<<get<0>(n[i])<<" y película : "<<get<1>(n[i])<<endl;
    cout<<endl;
}

void print_vector(vector<tuple<float,string,int>> n)
{
    for(unsigned int i=0; i< n.size(); i++)
        cout<<"\nPelicula: "<<get<1>(n[i])<<" ,con puntaje: "<<get<0>(n[i])<<" y "<<get<2>(n[i])<<" veces recomendada";
    cout<<endl;
}

vector<string> split_string_nos(const string& s,char delimit)
{
    vector<string> splitted;
    bool flag = false;
    splitted.push_back("");
    for(unsigned int i=0; i<s.size(); ++i)
    {
        if(s[i]=='"')
        {
            flag = flag? false : true;
            continue;
        }
        if(s[i]==delimit && !flag)
            splitted.push_back("");
        else
            splitted[splitted.size()-1] += s[i];
    }
    return splitted;
}

db getBD(string fileName, char delimit, int &ultimo_usuario){

    db BDatos;
    ifstream File(fileName.c_str());

    if (!File) {
    std::cout << "Error, could not open file." << std::endl;
    }

    vector <string> temp;
    temp.reserve(3);
    string line;

    while(getline(File,line))
    {
        stringstream  lineStream(line);

        temp = split_string_nos(line, delimit);

        string QString = temp[2];
        istringstream StrToFloat(QString );
        float floatVar;
        StrToFloat >> floatVar;

        BDatos[temp[0]][temp[1]] = floatVar;
    }

    string QString = temp[0];
    istringstream StrToInt(QString );
    int floatVar;
    StrToInt >> floatVar;

    ultimo_usuario = floatVar;

    File.close();
    return BDatos;
}

class RecomenderSystem{
///define something
public:
    db data;
    int ultimo_usuario;
public:
    db load_data(string filename, char delimit)
    {
        data = getBD(filename,delimit,ultimo_usuario);
        return data;
    }
    void print_db()
    {
        for (auto user: data)
        {
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
            if (data[user2].find(item.first) != data[user2].end())
            {
                distance += fabs(data[user1][item.first] - data[user2][item.first]);
            }
        }
        return distance;
    }

    float euclidean(string user1, string user2)
    {
        float distance = 0;

        for(auto item: data[user1])
        {
            if (data[user2].find(item.first) != data[user2].end())
                distance += fabs(data[user2][item.first] - data[user1][item.first])*fabs(data[user2][item.first] - data[user1][item.first]);
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
        float sum_y2 = 0;
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
            if (algoritmo == "Manhattan")
            {
                for(auto users: data)
                {
                    if (users.first != user)
                        distanciasTodo.push_back(make_tuple(manhattan(user,users.first),users.first));
                }
            }
            else {
                for(auto users: data)
                {
                    if (users.first != user)
                        distanciasTodo.push_back(make_tuple(euclidean(user,users.first),users.first));
                }
            }

            sort(distanciasTodo.begin(),distanciasTodo.end());

            vector<tuple<float,string>> distancias;
            distancias.reserve(k);
            int n = distanciasTodo.size();
            if (n < k)
                k = n;
            for(int i=0; i <k; i++)
                distancias.push_back(distanciasTodo[i]);
            return distancias;

        }
        else if (algoritmo == "Pearson" || algoritmo == "Similitud del Coseno")
        {
            if (algoritmo == "Pearson")
            {
                for(auto users: data)
                {
                    if (users.first != user)
                        distanciasTodo.push_back(make_tuple(pearson(user,users.first),users.first));
                }
            }
            else {
                for(auto users: data)
                {
                    if (users.first != user)
                        distanciasTodo.push_back(make_tuple(sim_coseno(user,users.first),users.first));
                }
            }

            sort(distanciasTodo.rbegin(),distanciasTodo.rend());

            vector<tuple<float,string>> distancias;
            distancias.reserve(k);
            int n = distanciasTodo.size();
            if (n < k)
                k = n;
            for(int i=0; i <k; i++)
                distancias.push_back(distanciasTodo[i]);
            return distancias;
        }
        else{
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
            return distancias;
        }
        else
        {
            cout<<"Ingrese una distancia valida"<<endl;
        }
    }
    /**FIN VECINO CERCANO 2*/

    // ** COMIENZO DEL RATING PROYECTADO  **/
    float probabilidad_item(string user,string movie,int k,string algoritmo)
    {
        try {
            float rating = data[user].at(movie);
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
                total += get<0>(nearestNeighbors[i]);
            }
            vector<float> porcentajes;
            float rating_proyectado = 0;
            for(int i=0; i<calificaiones.size();i++)
                rating_proyectado += calificaiones[i]*(get<0>(nearestNeighbors[i])/total);
            return rating_proyectado;
        }
    }
    //** FIN DEL RATING PROYECTADO **/

    vector<tuple<float,string>> MapToVector( map<ItemId, RatingVal> mapa )
    {
      vector<tuple<float,string>> v;
      for( map<ItemId,RatingVal>::iterator it = mapa.begin(); it != mapa.end(); ++it ) {
        v.push_back( make_tuple(it->second, it->first));
      }
      return v;
    }

    //                    --------- Ingresar nuevo item en la lista de recomendados ----------                      //
    bool IngresarItems(vector <tuple<float,string,int>> &Recomendados, tuple<float,string> &Nuevo,vector<tuple<float,string>> &MasCercanos,float umbral)
    {
        unsigned int s = Recomendados.size();
        for (unsigned int i=0; i<=s; i++){
            if (s!=0 && get<1>(Recomendados[i]) == get<1>(Nuevo)){
              return true;
            }
            else if(s==0 ||i == Recomendados.size()-1){    //Si ya llegó al final y no está
              float puntaje = 0, nuevoPunt = 0, cont = 0;
              for (unsigned int j = 0; j< MasCercanos.size(); j++){
                try{
                  nuevoPunt = data[get<1>(MasCercanos[j])].at(get<1>(Nuevo));
                  // cont = cont +1;                       // COMENTAR ESTO CUANDO SE USE EL UMBRAL , DESCOMENTAR CUANDO NO SE USE EL UMBRAL
                }
                catch (const std::out_of_range& oor){      //Si no encuentra la pelicula en el usuario2
                  nuevoPunt = 0;
                }
                if(nuevoPunt >= umbral){                // DESCOMENTAR ESTO SI CONSIDERAMOS LOS QUE NO SUPEREN EL UMBRAL
                    cont = cont +1;                     //
                    puntaje = puntaje + nuevoPunt;
                }                                       //
              }
              puntaje = (puntaje/cont);
              Recomendados.push_back(make_tuple(puntaje,get<1>(Nuevo),cont));
              return false;
            }
        }
    }

    // ------------ 3) Recomendar pelicula segun k usuarios más parecidos--------//
    // In: Usuario, algoritmo, k-más parecidos
    // Out: vector< pelicula, rating>
    vector <tuple<float,string,int>> RecomendarPorKUsuarios(string usuario, string algoritmo, int k,float umbral)
    {
        vector <tuple< float,string,int> > Recomendados;
        vector <tuple< float,string> > MasCercanos = vecino_cercano(usuario, k, algoritmo);
        vector <tuple<float,string>> ItemsMayorMenor;
        string usuario2;
        float puntaje=0;

        for (unsigned int i=0; i<MasCercanos.size(); i++){
            usuario2 = get<1>(MasCercanos[i]);
            ItemsMayorMenor = MapToVector(data[usuario2]);
            stable_sort(ItemsMayorMenor.begin(),ItemsMayorMenor.end(), predicate());

            for (unsigned int j = 0; j<ItemsMayorMenor.size();j++)
            {
                float p = get<0>(ItemsMayorMenor[j]);  //Puntaje
                string item = get<1>(ItemsMayorMenor[j]);  //Pelicula/Libro
                if(p>=umbral){
                  try{
                    puntaje = data[usuario].at(item);  //Puntaje del usuario1 (al que debemos recomendar) hacia la pelicula
                  }
                  catch (const std::out_of_range& oor){
                    // ----------------------Recomendando todas las peliculas que superen el umbral ----------------------//
                    bool estaRec = IngresarItems(Recomendados,ItemsMayorMenor[j],MasCercanos,umbral);
                    // ----------------------------- Recomendando una pelicula por cada usuario --------------------------//
                    if(estaRec == false)    //Si la pelicula no esta en recomendados, se agregó y ya no se necesita buscar más películas
                        break;
                    else                   // Si la pelicula si esta en recomendados, no se agregó y se sigue con el siguiente de mayor puntaje
                        continue;
                  }
                }
                else{
                    break;
                }
            }
        }
        stable_sort(Recomendados.begin(),Recomendados.end(), predicate_2());
        return Recomendados;
    }
};
