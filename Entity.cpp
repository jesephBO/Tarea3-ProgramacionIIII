//
// Created by burgo on 16/4/2026.
//

#include "Entity.h"

//Constructor de la clase Entity
Entity::Entity(string _nom):nombre(_nom) {
    vida = 100;
    energia = 50;
    nivel = 1;
    //La entidad es generada en coordenadas aleatorias en un cierto rango de spawn
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(1,500);
    posX = distr(gen);
    posY = distr(gen);
}


//Metodos public que modifican el estado de la entidad
void Entity::move(int x, int y) {
    posX+=x;
    posY+=y;
}

void Entity::setPosicion(int x, int y) {
    posX = x;
    posY = y;
}


void Entity::modifyHeal(int pts) {
    vida+=pts;
    if (vida<1)
        vida=1;
}

void Entity::modifyLevel(int pts) {
    nivel+=pts;
    if (nivel<1)
        nivel = 1;
}

void Entity::modifyEnergy(int pts) {
    energia+=pts;
    if (energia<1)
        energia=1;
}

void Entity::AddInventary(string inv) {
    inventario.push_back(inv);
}

void Entity::deleteInventary(string inv) {
    erase(inventario,inv);
}

//Metodo que retorna el estado actual de la entidad (valores de sus atributos)
string Entity::status() {
    string _inv = "";
    for (auto i:inventario)
        _inv+=" "+i;
    return "Vida: " + to_string(vida)+
        " | Nivel: " + to_string(nivel)+
            " | Energia: "+to_string(energia)+
            " | Posicion: ("+to_string(posX)+","+to_string(posY)+
                ") | Inventario: "+_inv;
}

void Entity::setHeal(int pts) {
    vida = pts;
}

void Entity::setEnergy(int pts) {
    energia = pts;
}

void Entity::setLevel(int pts) {
    nivel = pts;
}

void Entity::setInventary(vector<string> inv) {
    inventario = inv;
}

int Entity::getEnergy() {
    return energia;
}

int Entity::getLevel() {
    return nivel;
}

string Entity::getNombre() {
    return nombre;
}

vector<string>& Entity::getInventary() {
    return inventario;
}


//Funciones del CommandCenter

//Funcion para registrar los nuevos comandos
void CommandCenter::registrarComando(const string& nombre, Command _comand) {
    comandos[nombre] = _comand;
    cout<<"El comando '"<<nombre<<"' ha sido agregado."<<endl;
}

//Funcion para eliminar los comandos que ya han sido registrados
void CommandCenter::eliminarComando(const string& nombre) {
    auto it = comandos.find(nombre);
    if (it!=comandos.end()) { //Verifica que el comando se encuentre registrado
        comandos.erase(nombre);
        cout<<"El comando '"<<nombre<<"' ha sido eliminado."<<endl;
    }
    else
        cout<<"!ERROR! El comando "<<nombre<< " no esta registrado"<<endl;
}

//Funcion para ejecutar los comandos en la entidad
void CommandCenter::execute(const string& comando, const list<string>& vals) {
    auto it = comandos.find(comando);
    if (it==comandos.end()) { //Verifica que el comando se encuentre registrado
        cout<<"!ERROR! El comando "<<comando<< " no fue encontrado"<<endl;
        return;
    }
    string antes = entity.status(); //Guarda el estado de la entidad antes de ejecutar el comando
    it->second(vals); //Accede a los comandos a traves de la clase de dato (second) del mapa
    string despues = entity.status(); //Guarda el estado de la entidad despues de ejecutar el comando
    historial.push_back("Comando: "+comando+"-> |Antes:  "+antes+"| Despues: "+despues+" |");
    //Se guarda el historial de la ejecucion con el nombre del comando y su impacto sobre la entidad
}

//Funcion para registrar los macro-comandos
void CommandCenter::registerMacro(const string& name, const list<pair<string, list<string>>>& steps) {
    macros[name] = steps;
    cout<<"El macro-comando '"<<name<<"' ha sido agregado."<<endl;
}

//Funcion para ejecutar los macro-comandos en la entidad
void CommandCenter::executeMacro(const string &name) {
    auto it = macros.find(name);
    if (it!=macros.end()) { //Verifica que el macro-comando este registrado
        auto steps = macros[name];
        //Para ejecutar el macro-comando recorremos los 'setps' ingresados y hacemos ejecuciones individuales de cada comando
        for (auto it_macro = steps.begin();it_macro!=steps.end();++it_macro) {
            auto comando = it_macro->first; //Usamos 'first' para acceder al nombre del comando
            auto valores = it_macro->second; //Usamos 'second' para acceder a los valores que se pasan al comando
            if (comandos.find(comando)==comandos.end()) { //Verifica que el comando se encuentre registrado
                cout<<"!ERROR! El comando '"<<comando<<"' no fue encontrado"<<endl;
                return;
            }
            execute(comando,valores);
        }
    }
    else
        cout<<"!ERROR! El macro-comando no fue encontrado"<<endl;
}



//FUNCTORES

void CambiarPos::operator()(const list<string> &vals) {
    //Verifica que solo se ingresen dos valores correspondientes a las coordenadas (x,y)
    //Tambien sirve para verificar que no ingrese una lista vacia
    if (vals.size()!=2) {
        cout<<"!ERROR! No se han ingresado la cantidad de parametros necesarios para ejecutar el comando"<<endl;
        return;
    }
    auto it = vals.begin();
    //Se usa la estructura try-catch para los casos en que los datos ingresados no se puedan convertir en numeros (coordenadas)
    //En caso de error, se envia un mensaje correspondiente y el programa no se rompe
    try {
        int posX = stoi(*it);
        int posY = stoi(*(++it));
        entidad.move(posX,posY);
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
        return;
    }
    cantVeces++;
    cout<<"La posicion se ha modificado.\nTotal de ejecuciones: "<<cantVeces<<endl;
}


void PanicEscape::operator()(const list<string> &vals) {
    if (!vals.empty()) { //Verifica que no se hayan ingresado parametros
        cout<<"!ERROR! La funcion no requiere parametros adicionales"<<endl;
        return;
    }
    //Este comando solo puede aplicar 2 veces por entidad
    //Se verifica si la entidad ya excedio este limite
    if (limite<2) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distr(500,1000);
        //La entidad es llevada a una nueva posicion aleatoria
        int nueva_x = distr(gen);
        int nueva_y = distr(gen);
        entidad.setPosicion(nueva_x,nueva_y);
        entidad.setHeal(10); //Se modifican los puntos de vida de la entidad
        entidad.setEnergy(30); //Se modifican los puntos de energia de la entidad
        limite++;
        cout<<"Se ha aplicado correctamente el PanicEscape.\nUsos restantes: "<<2-limite<<endl;
    }
    else
        cout<<"!ERROR! Ya ha alcanzado del limite de usos del PanicEscape"<<endl;
}


void Eliminado::operator()(const list<string> &vals) {
    if (!vals.empty()) { //Verifica que no se hayan ingresado parametros
        cout<<"!ERROR! La funcion no requiere parametros adicionales"<<endl;
        return;
    }
    entidad.setPosicion(0,0);
    entidad.setHeal(100);
    entidad.setEnergy(50);
    entidad.setLevel(1);
    entidad.setInventary({});
    contVeces++;
    cout<<"El personaje ha sido eliminado "<<contVeces<<" veces"<<endl;
}


void Dasheo::operator()(const list<string> &vals) {
    if (vals.size()!=2) { //Verifica que solo se ingresen dos valores, que corresponden a las coordenadas (x,y)
        cout<<"!ERROR! No se han ingresado los valores necesarios para ejecutar el comando"<<endl;
        return;
    }
    if (entidad.getLevel()<20) { //Para ejecutar este comando es necesario que la entidad tenga un nivel superio a 20
        cout<<"!ERROR! La entidad el nivel suficiente para realizar el dasheo\n";
        return;
    }
    if (entidad.getEnergy()<0.15*entidad.getLevel()) { //Para ejecutar este comando es necesario que la energia de la entidad sea mayor al 15% de su nivel
        cout<<"!ERROR! La entidad no tiene la energia suficiente para realizar el dasheo\n";
        return;
    }
    //Se usa la estructura try-catch para los casos en que los datos ingresados no se puedan convertir en numeros (coordenadas)
    try {
        auto it = vals.begin();
        int posX = stoi(*it);
        int posY = stoi(*(++it));
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> distr(1,5);
        //Aleatoriamente el movimiento de la entidad puede tener un multiplicador en un rango [1-5]
        posX*=distr(gen);
        posY*=distr(gen);
        entidad.move(posX,posY);
        entidad.modifyEnergy(-20);
        histDasheo.push_back({posX,posY});
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
        return;
    }
    cout<<"Se ha ejecutado correctamente el dasheo\n";
    cout<<"Historial de direcciones dasheadas\n";
    for (auto a:histDasheo)
        cout<<"x: "<<a[0]<<" | y: "<<a[1]<<endl;
}



void AumentarVida(Entity& entidad, const list<string>& vals) {
    if (vals.size()!=1) { //Verifica que el comando solo reciba un parametro, correspondiente a los puntos de vida
        cout<<"!ERROR! no se han ingresado la cantidad apropiada de valores\n";
        return;
    }
    //Se usa la estructura try-catch para el caso en que el dato ingresado no se pueda convertir en un numero
    try {
        auto it = vals.begin();
        int pts = stoi(*it);
        entidad.modifyHeal(pts);
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
        return;
    }
    cout<<"Se ha ejecutado correctamente el comando 'AumentarVida'\n";
}


void GastarEnergia(Entity& entidad, const list<string>& vals) {
    if (vals.size()!=1) { //Verifica que el comando solo reciba un parametro, correspondiente a los puntos de energia
        cout<<"!ERROR! no se han ingresado la cantidad apropiada de valores\n";
        return;
    }
    //Se usa la estructura try-catch para el caso en que el dato ingresado no se pueda convertir en un numero
    try {
        auto it = vals.begin();
        int pts = stoi(*it)*-1;
        entidad.modifyEnergy(pts);
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
        return;
    }
    cout<<"Se ha ejecutado correctamente el comando 'GastarEnergia'\n";
}

void RecargarEnergia(Entity& entidad, const list<string>& vals) {
    if (vals.size()!=1) { //Verifica que el comando solo reciba un parametro, correspondiente a los puntos de energia
        cout<<"!ERROR! no se han ingresado la cantidad apropiada de valores\n";
        return;
    }
    if (entidad.getLevel()<10) { //Para ejecutar este comando el nivel de la entidad debe ser mayor a 10
        cout<<"!ERROR! la entidad no tiene el nivel suficiente para ejecutar este comando\n";
        return;
    }
    //Se usa la estructura try-catch para el caso en que el dato ingresado no se pueda convertir en un numero
    try {
        auto it = vals.begin();
        int pts = stoi(*it);
        entidad.modifyEnergy(pts);
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
        return;
    }
    cout<<"Se ha ejecutado correctamente el comando 'RecargarEnergia'\n";
}

void AumentarNivel(Entity& entidad, const list<string>& vals) {
    if (vals.size()!=1) { //Verifica que el comando solo reciba un parametro, correspondiente a los puntos de nivel
        cout<<"!ERROR! no se han ingresado la cantidad apropiada de valores\n";
        return;
    }
    //Se usa la estructura try-catch para el caso en que el dato ingresado no se pueda convertir en un numero
    try {
        auto it = vals.begin();
        int pts = stoi(*it);
        entidad.modifyLevel(pts);
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
        return;
    }
    cout<<"Se ha ejecutado correctamente el comando 'AumentarNivel'\n";
}

void Damage(Entity &entidad, const list<string> &vals) {
    if (vals.size()!=1) { //Verifica que el comando solo reciba un parametro, correspondiente a los puntos de daño
        cout<<"!ERROR! no se han ingresado la cantidad apropiada de valores\n";
        return;
    }
    //Se usa la estructura try-catch para el caso en que el dato ingresado no se pueda convertir en un numero
    try {
        auto it = vals.begin();
        int pts = stoi(*it);
        if (pts>50) { //En el caso que el daño sea mayor a 50, este se duplica (DAÑO CRITICO)
            pts*=-2;
            entidad.modifyHeal(pts);
            cout<<"!DANIO CRITICO! El valor del danio se ha duplicado\n";
        }
        else { //Si el daño es menor a 50, no se realiza otro proceso extra. Unicamente se resta en la vida de la entidad
            pts*=-1;
            //Se multiplica por (-1) ya que la funcion 'modifyHeal' suma el valor recibido con la vida de la entidad
            //Pero en este caso al ser un daño deberia restarse
            entidad.modifyHeal(pts);
            cout<<"Se ejecuto correctamente el comando 'damage'. La entidad perdio vida\n";
        }
    }
    catch (const invalid_argument&) {
        cout<<"!ERROR! El valor ingresado no es un numero\n";
    }
}





