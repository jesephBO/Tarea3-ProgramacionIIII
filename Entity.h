//
// Created by burgo on 16/4/2026.
//

#ifndef TAREA3_ENTITY_H
#define TAREA3_ENTITY_H

#include <iostream>
#include <random>
#include <list>
#include <map>
#include <functional>
#include <vector>
#include <algorithm>
using namespace std;

using Command = function<void(const list<string>&)>;

//Definicion de la clase Entity
class Entity {
    //Estado interno (atributos privados)
    int posX; //Coordenada 'X' de la posicion donde se encuentra la entidad
    int posY;   //Coordenada 'Y' de la posicion donde se encuentra la entidad
    int vida;   //puntos de vida de la entidad
    int energia; //puntos de energia de la entidad
    string nombre; //nombre de la entidad
    int nivel; //nivel de la entidad
    vector<string> inventario; //Inventario de la entidad
public:
    //Metodos publicos que permiten acceder y modificar el estado de la entidad
    Entity(string _nom);
    void move(int x, int y);
    void setPosicion(int x, int y);
    void modifyHeal(int pts);
    void setHeal(int pts);
    void modifyEnergy(int pts);
    void setEnergy(int pts);
    int getEnergy();
    void modifyLevel(int pts);
    void setLevel(int pts);
    int getLevel();
    string getNombre();
    void AddInventary(string inv);
    void deleteInventary(string inv);
    void setInventary(vector<string> inv);
    vector<string>& getInventary();
    string status();
};

//Definicion del motor de comandos
class CommandCenter {
    Entity& entity; //Instancia de Entity
    map<string,Command> comandos; //Mapa para registro de comandos
    list<string> historial; //Lista para mantener un historial de las ejecuciones de los comandos
    map<string,list<pair<string,list<string>>>> macros; //Mapa para registro de macro-comandos
public:
    CommandCenter(Entity& _ent):entity(_ent){} //En el constructor se pasa por referencia a la entidad sobre la que se operará
    void registrarComando(const string& nombre, Command _comand);
    void eliminarComando(const string& nombre);
    void execute(const string& comando,const list<string>& vals);
    void registerMacro(const string& name, const list<pair<string,list<string>>>& steps);
    void executeMacro(const string& name);

};


//Definicion de 'FUNCTORES'
class CambiarPos { //El functor permite que la entidad se mueva por una determinada direccion (coordenadas x,y)
    Entity& entidad;
    int cantVeces; //Estado interno propio del functor. Se usa para contabilizar la cantidad de ejecuciones del comando
public:
    CambiarPos(Entity& _entidad):entidad(_entidad),cantVeces(0){}
    void operator()(const list<string>& vals);
};

class PanicEscape { //Este functor representa una huida de emergencia en un videojuego
    Entity& entidad;
    int limite; //Estado interno propio del functor. Se usa para verificar que la entidad no exceda el limite usos del comando
public:
    PanicEscape(Entity& _entidad):entidad(_entidad),limite(0){}
    void operator()(const list<string>& vals);
};

class Eliminado { //Este functor representa la eliminacion de la entidad. Como un videojuego, sus atributos se restablecen a los default.
    Entity& entidad;
    int contVeces; //Estado interno propio del functor. Se usa para contabilizar la cantidad de ejecuciones del comando
public:
    Eliminado(Entity& _entidad):entidad(_entidad),contVeces(0){}
    void operator()(const list<string>& vals);
};

class Dasheo { //El comando dasheo hace referencia a un ataque rapido hacia una cierta direccion
    Entity& entidad;
    vector<vector<int>> histDasheo; //Estado interno propio.Se usa como historial de los lugares a los que hace el dahseo (coordenadas x,y)
public:
    Dasheo(Entity& _entidad):entidad(_entidad){}
    void operator()(const list<string>& vals);
};


//Definicion de 'Funciones libres'
void AumentarVida(Entity& entidad, const list<string>& vals); //Esta funcion le permite a la entidad aumentar sus puntos de vida
void GastarEnergia(Entity& entidad, const list<string>& vals); //Esta funcion hace que la entidad pierda puntos de energia
void RecargarEnergia(Entity& entidad, const list<string>& vals); //Esta funcion permite que la entidad aumente puntos de energia
void Damage(Entity& entidad, const list<string>& vals); //Esta funcion permite que la entidad reciba daño y se reduzcan sus puntos de vida
void AumentarNivel(Entity& entidad, const list<string>& vals); //Esta funcion permite que la entidad aumente sus puntos de nivel



#endif //TAREA3_ENTITY_H
