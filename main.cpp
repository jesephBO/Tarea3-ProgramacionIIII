#include "Entity.h"

int main() {
    Entity pj1("Superheroe"); //Creacion de la entidad
    CommandCenter center(pj1); //Creacion del CommandCenter

    //Registro de comandos
    //Comandos con functores
    center.registrarComando("move",CambiarPos(pj1));
    center.registrarComando("panicEscape",PanicEscape(pj1));
    center.registrarComando("eliminado",Eliminado(pj1));
    center.registrarComando("dasheo",Dasheo(pj1));

    //Comandos con funciones libres
    //Se pasan usando funciones lambda porque se ha definido que las funciones deben recibir list<string>&. No obstante estas funciones libres tambien deben recibir la entidad
    center.registrarComando("aumentarVida",[&pj1](const list<string>& vals){AumentarVida(pj1,vals);});
    center.registrarComando("gastarEnergia",[&pj1](const list<string>& vals){GastarEnergia(pj1,vals);});
    center.registrarComando("recargarEnergia",[&pj1](const list<string>& vals){RecargarEnergia(pj1,vals);});
    center.registrarComando("aumentarNivel",[&pj1](const list<string>& vals){AumentarNivel(pj1,vals);});
    center.registrarComando("damage",[&pj1](const list<string>& vals){Damage(pj1,vals);});

    //Comandos con funciones lambda
    center.registrarComando("agregarInventario",[&pj1](const list<string>& vals) {
            if (!vals.empty()) {
                auto& inv = pj1.getInventary();
                for (auto i:vals) {
                    if (find(inv.begin(),inv.end(),i)==inv.end()) {
                        pj1.AddInventary(i);
                        cout<<"Se ha agregado correctamente el elemento '"<<i<<"'"<<endl;
                    }
                    else
                        cout<<"El elemento '"<<i<<"' ya se encuentra en el inventario\n";
                }
            }
            else
                cout<<"!ERROR! no hay elementos por agregar\n";
        });
    center.registrarComando("borrarElemento",[&pj1](const list<string>& vals) {
        if (!vals.empty()) {
            auto& inv = pj1.getInventary();
            for (auto i:vals) {
                auto it = find(inv.begin(),inv.end(),i);
                if (it == inv.end())
                    cout<<"!ERROR! El elemento '"<<i<< "' no se enuentra en el inventario\n";
                else {
                    pj1.deleteInventary(i);
                    cout<<"Se ha eliminado correctamente el elemento '"<<i<<"'"<<endl;
                }
            }
        }
        else
            cout<<"!ERROR! Se necesita ingresar algun elemento\n";
    });
    center.registrarComando("status",[&pj1](const list<string>& vals) {
        if (vals.empty()) {
            cout<<"---------------------------------\nEstado de la entidad "<<pj1.getNombre()<<" :\n";
            cout<<pj1.status()<<"\n---------------------------------\n";
        }
        else
            cout<<"!ERROR! El comando no requiere parametros adicionales\n";
        });

    //EJECUCION DE COMANDOS VALIDOS
    cout<<"\n-- TESTEO DE EJECUCION DE COMANDOS --\n";
    cout<<"\n-- verificacion del estado inicial --\n";
    center.execute("status",{});
    center.execute("aumentarNivel",{"25"});
    center.execute("aumentarVida",{"50"});
    center.execute("status",{});
    center.execute("recargarEnergia",{"95"});
    center.execute("status",{});
    center.execute("gastarEnergia",{"92"});
    center.execute("status",{});
    center.execute("damage",{"67"});
    center.execute("status",{});

    cout<<"\n-- TESTEO DE COMANDOS DE MOVIMIENTO --\n\n";
    center.execute("move",{"60","45"});
    center.execute("status",{});
    center.execute("move",{"15","90"});
    center.execute("status",{});
    center.execute("dasheo",{"10","10"});
    center.execute("status",{});

    cout<<"\n-- TESTEO DE RESTRICCIONES DEL DASHEO --\n\n";
    center.execute("dasheo",{"7","14"});
    center.execute("dasheo",{"41","13"});
    center.execute("dasheo",{"5","5"});

    cout<<"\n-- TESTEO DE COMANDOS QUE MODIFICAN EL INVENTARIO --\n\n";
    center.execute("agregarInventario",{"hacha"});
    center.execute("status",{});
    //Se verifica que el comando 'agregarInventario' puede recibir un vector de strings
    center.execute("agregarInventario",{"madera","espada","vendas"});
    center.execute("status",{});
    //Se verifica que el comando 'agregarInventario' no agrega elementos que ya estan en el inventario
    center.execute("agregarInventario",{"hacha","vendas"});
    center.execute("status",{});
    //Verificacion de las restricciones de la eliminacion de elementos del inventario
    center.execute("borrarElemento",{"espada"});
    center.execute("status",{});
    //Se verifica que el comando no acepta elementos que no estan en el inventario
    center.execute("borrarElemento",{"libros"});
    center.execute("borrarElemento",{"katana","orbe"});
    center.execute("status",{});

    cout<<"\n-- TESTEO DE RESTRICCIONES DE PANIC_ESCAPE --\n\n";
    //Se verifica el limite de usos del comando a traves de su estado interno
    center.execute("panicEscape",{});
    center.execute("status",{});
    center.execute("panicEscape",{});
    center.execute("status",{});
    center.execute("panicEscape",{});

    cout<<"\n-- TESTEO DEl COMANDO ELIMINADO --\n\n";
    //Se verifica el contador de ejecuciones a traves de su estado interno
    center.execute("eliminado",{});
    center.execute("eliminado",{});
    center.execute("eliminado",{});
    center.execute("status",{});

    cout<<"\n-- TESTEO DE ELIMINACION DE COMANDOS --\n\n";
    center.eliminarComando("move");
    center.execute("move",{"10","10"});
    center.registrarComando("move",CambiarPos(pj1));
    center.execute("move",{"10","10"});
    center.execute("status",{});

    //EJECUCION DE COMANDOS INVALIDOS
    //Las funciones se ha diseñado para tratar los errores de manera interna, lanzar mensajes de error y no generar errores internos en la entidad
    cout<<"\n-- TESTEO DE COMANDOS INVALIDOS --\n\n";
    center.execute("eliminado",{"escape"}); //Se genera un error ya que el comando no requiere parametros
    center.execute("aumentarVida",{"ladrillo"}); //Se genera un error porque el valor ingresado no puede ser transformado en numero
    center.execute("gastarEnergia",{"ABCDE"}); //Se genera un error porque el valor ingresado no puede ser transformado en numero
    center.execute("dasheo",{"3"}); //Se genera un error porque el comando requiere dos parametros para generar una coordenada
    center.execute("move",{"3","UTEC"}); //Se genera un error porque el comando requiere que los parametros se puedan convertir a numeros
    center.execute("move",{"3","4","5"}); //Se genera un error porque el comando solo requiere dos parametros (coordenadas x,y)
    center.execute("status",{"informacion"}); //Se genera un error ya que el comando no requiere parametros
    center.execute("comando_desconocido",{}); //Se genera un error ya que el comando no ha sido registrado. Por tanto no se puede ejecutar
    center.eliminarComando("comando_desconocido"); //Se genera un error ya que el comando no esta registrado. Por tanto no se puede eliminar

    cout<<"\n-- TESTEO DE MACRO COMANDOS --\n\n";
    //Para mejorar la claridad en el programa registramos los macro-comands en varias lineas de codigo
    center.registerMacro("kit",{
        {"agregarInventario",{"botiquin"}},
        {"agregarInventario",{"espada"}},
        {"agregarInventario",{"madera"}}});
    center.registerMacro("boost",{
        {"aumentarNivel",{"70"}},
        {"recargarEnergia",{"100"}},
        {"aumentarVida",{"50"}}});
    center.registerMacro("batalla",{
        {"move",{"5","0"}},
        {"dasheo",{"10","10"}},
        {"damage",{"67"}},
        {"move",{"0","5"}},
        {"damage",{"10"}}});
    center.registerMacro("mineria",{
        {"agregarInventario",{"pico"}},
        {"move",{"10","0"}},
        {"agregarInventario",{"oro"}},
        {"move",{"5","0"}},
        {"agregarInventario",{"diamante"}},
        {"move",{"0","5"}},
        {"agregarInventario",{"hierro"}},
        {"move",{"0","10"}},
        {"agregarInventario",{"carbon"}},
        {"gastarEnergia",{"30"}},
        {"borrarElemento",{"pico"}}});

    cout<<"\n-- Verificacion de estado inicial --\n";
    center.execute("status",{});
    center.executeMacro("kit");
    center.execute("status",{});
    center.executeMacro("mineria");
    center.execute("status",{});
    center.executeMacro("boost");
    center.execute("status",{});
    center.executeMacro("batalla");
    center.execute("status",{});


    cout<<"\n-- EJECUCIONES INVALIDAS CON MACRO-COMANDOS --\n\n";
    center.execute("eliminado",{});
    center.execute("status",{});
    center.executeMacro("kit");
    center.executeMacro("kit");
    //Se genera un error ya que el comando 'agregarInventario' no permite duplicidad de elementos
    center.executeMacro("nuevo_macro_comando"); //Se genera un error ya que el macro-comando no ha sido registrado
    center.executeMacro("batalla");
    //En este caso no se ejecuta el comando 'dasheo' (incluido en el macro-comando 'batalla')ya que la entidad no tiene el nivel suficiente.
    //Pese a ello, el macro-comando se sigue ejecutando, unicamente 'salta' este comando que se puede ejecutar.

    //Verificacion del caso en que el macro-comando utiliza un comando que no ha sido agregado
    cout<<"\n-- MACRO-COMANDO CON COMANDO NO AGREGADO --\n\n";
    center.registerMacro("farmeo",{
       {"move",{"5","0"}},
        {"agregarInventario",{"ramas"}},
        {"move",{"0","5"}},
        {"nuevo_comando",{}},
        {"agregarInventario",{"roca"}},
        {"move",{"0","5"}}});
    center.executeMacro("farmeo");
    //Al llegar al comando que no se ha registrado, el programa se detiene y envia un mensaje de error
    //No se ejecutan los comandos posteriores, a pesar que estos si esten registrados y sean correctos

    return 0;
}
