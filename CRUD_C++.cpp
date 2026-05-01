#include <iostream>
#include <pqxx/pqxx>
#include <string>
using namespace std;

// ===== Conexión =====
pqxx::connection conectar() {
    pqxx::connection conn("dbname=universidad user=postgres password=admin host=localhost port=5432");
    if (!conn.is_open()) {
        throw runtime_error("No se pudo conectar a la base de datos.");
    }
    cout << "Conectado a la base de datos universidad." << endl;
    return conn;
}
// funcion para manejar error de entrada en el menu
int pedirOpcion() {
    int op;
    while (!(cin >> op)) {
        cout << "Error: ¡Debes ingresar un numero, no letras!" << endl;
        cout << "Intenta de nuevo: ";
        cin.clear(); // Limpia el error
        cin.ignore(10000, '\n'); // Borra la basura que escribieron
    }
    return op;
}

// ===== CRUD Alumnos =====
void listarAlumnos(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result r = txn.exec(
        "SELECT a.id_alumnos, a.nombres, a.apellidos, a.carnet, "
        "COALESCE(c.nombre, 'Sin cursos') AS curso, "
        "COALESCE(s.nombre, 'Sin seccion') AS seccion "
        "FROM alumnos a "
        "LEFT JOIN alumnos_cursos ac ON a.id_alumnos = ac.alumno_id "
        "LEFT JOIN cursos c ON ac.curso_id = c.id_cursos "
        "LEFT JOIN alumnos_seccion ase ON a.id_alumnos = ase.alumno_id "
        "LEFT JOIN secciones s ON ase.seccion_id = s.id_secciones "
        "ORDER BY a.id_alumnos;"
    );

    cout << "\n--- Lista de Alumnos ---\n";
    for (auto fila : r) {
        cout << fila[0].as<int>() << " | " << fila[1].as<string>() << " "
            << fila[2].as<string>() << " | Carnet: " << fila[3].as<string>()
            << " | Curso: " << fila[4].as<string>()
            << " | Seccion: " << fila[5].as<string>() << endl;
    }
}

void grabarAlumno(pqxx::connection& conn) {
    string nombres, apellidos, carnet;
    cout << "Ingrese nombres: "; cin >> nombres;
    cout << "Ingrese apellidos: "; cin >> apellidos;
    cout << "Ingrese carnet: "; cin >> carnet;
    pqxx::work txn(conn);
    txn.exec("INSERT INTO alumnos (nombres, apellidos, carnet) VALUES (" +
        txn.quote(nombres) + ", " + txn.quote(apellidos) + ", " + txn.quote(carnet) + ")");
    txn.commit();
    cout << "Alumno grabado correctamente." << endl;
}

void modificarAlumno(pqxx::connection& conn) {
    int id;
    string nuevosNombres, nuevosApellidos, nuevoCarnet;

    cout << "Ingrese ID del alumno a modificar: "; cin >> id;
    cout << "Ingrese nuevos nombres: "; cin >> nuevosNombres;
    cout << "Ingrese nuevos apellidos: "; cin >> nuevosApellidos;
    cout << "Ingrese nuevo carnet: "; cin >> nuevoCarnet;

    pqxx::work txn(conn);
    // Aquí le decimos que actualice los 3 datos al mismo tiempo
    txn.exec("UPDATE alumnos SET nombres = " + txn.quote(nuevosNombres) +
        ", apellidos = " + txn.quote(nuevosApellidos) +
        ", carnet = " + txn.quote(nuevoCarnet) +
        " WHERE id_alumnos = " + txn.quote(id));
    txn.commit();
    cout << "Alumno modificado completamente." << endl;
}

void eliminarAlumno(pqxx::connection& conn) {
    int id;
    cout << "Ingrese ID del alumno a eliminar: "; cin >> id;

    try {
        pqxx::work txn(conn);

        // 1. Primero borramos sus conexiones en las tablas intermedias
        txn.exec("DELETE FROM alumnos_cursos WHERE alumno_id = " + txn.quote(id));
        txn.exec("DELETE FROM alumnos_seccion WHERE alumno_id = " + txn.quote(id));

        // 2. Ahora que está libre, borramos al alumno
        txn.exec("DELETE FROM alumnos WHERE id_alumnos = " + txn.quote(id));

        txn.commit();
        cout << "Alumno y sus asignaciones eliminados correctamente." << endl;
    }
    catch (const exception& e) {
        cout << "Error: No se pudo eliminar. " << e.what() << endl;
    }
}

void menuAlumnos(pqxx::connection& conn) {
    int opcion;
    do {
        cout << "\n===== MODULO ALUMNOS =====\n1. Listar\n2. Grabar\n3. Modificar\n4. Eliminar\n5. Regresar\nSeleccione: ";
        opcion = pedirOpcion();
        switch (opcion) {
        case 1: listarAlumnos(conn); break;
        case 2: grabarAlumno(conn); break;
        case 3: modificarAlumno(conn); break;
        case 4: eliminarAlumno(conn); break;
        case 5: break;
        default: cout << "Opcion fuera de rango." << endl;
        }
    } while (opcion != 5);
}

// ===== CRUD Cursos =====
void listarCursos(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result r = txn.exec("SELECT id_cursos, nombre, codigo FROM cursos;");
    cout << "\n--- Lista de Cursos ---\n";
    for (auto fila : r) {
        cout << fila[0].as<int>() << " | " << fila[1].as<string>() << " | " << fila[2].as<string>() << endl;
    }
}

void grabarCurso(pqxx::connection& conn) {
    string nombre, codigo;
    cout << "Ingrese nombre: "; cin >> nombre;
    cout << "Ingrese codigo: "; cin >> codigo;
    pqxx::work txn(conn);
    txn.exec("INSERT INTO cursos (nombre, codigo) VALUES (" + txn.quote(nombre) + ", " + txn.quote(codigo) + ")");
    txn.commit();
    cout << "Curso grabado correctamente." << endl;
}

void modificarCurso(pqxx::connection& conn) {
    int id;
    string nuevoNombre, nuevoCodigo;

    cout << "Ingrese ID del curso a modificar: "; cin >> id;
    cout << "Ingrese nuevo nombre: "; cin >> nuevoNombre;
    cout << "Ingrese nuevo codigo: "; cin >> nuevoCodigo;

    pqxx::work txn(conn);
    // Actualizamos el nombre y el código
    txn.exec("UPDATE cursos SET nombre = " + txn.quote(nuevoNombre) +
        ", codigo = " + txn.quote(nuevoCodigo) +
        " WHERE id_cursos = " + txn.quote(id));
    txn.commit();
    cout << "Curso modificado completamente." << endl;
}

void eliminarCurso(pqxx::connection& conn) {
    int id; cout << "Ingrese ID del curso: "; cin >> id;
    pqxx::work txn(conn);
    pqxx::result r = txn.exec("SELECT COUNT(*) FROM alumnos_cursos WHERE curso_id = " + txn.quote(id));
    if (r[0][0].as<int>() > 0) {
        cout << "No se puede eliminar: curso asignado a alumnos." << endl;
        return;
    }
    txn.exec("DELETE FROM cursos WHERE id_cursos = " + txn.quote(id));
    txn.commit();
    cout << "Curso eliminado correctamente." << endl;
}

void menuCursos(pqxx::connection& conn) {
    int opcion;
    do {
        cout << "\n===== MODULO CURSOS =====\n1. Listar\n2. Grabar\n3. Modificar\n4. Eliminar\n5. Regresar\nSeleccione: ";
        opcion = pedirOpcion(); 
        switch (opcion) {
            case 1: listarCursos(conn); break;
            case 2: grabarCurso(conn); break;
            case 3: modificarCurso(conn); break;
            case 4: eliminarCurso(conn); break;
            case 5: break;
            default: cout << "Opcion fuera de rango." << endl;
        }
    } while (opcion != 5);
}

// ===== CRUD Secciones =====
void listarSecciones(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result r = txn.exec("SELECT id_secciones, nombre, jornada FROM secciones;");
    cout << "\n--- Lista de Secciones ---\n";
    for (auto fila : r) {
        cout << fila[0].as<int>() << " | " << fila[1].as<string>() << " | " << fila[2].as<string>() << endl;
    }
}

void grabarSeccion(pqxx::connection& conn) {
    string nombre, jornada;
    cout << "Ingrese nombre: "; cin >> nombre;
    cout << "Ingrese jornada: "; cin >> jornada;
    pqxx::work txn(conn);
    txn.exec("INSERT INTO secciones (nombre, jornada) VALUES (" + txn.quote(nombre) + ", " + txn.quote(jornada) + ")");
    txn.commit();
    cout << "Seccion grabada correctamente." << endl;
}

void modificarSeccion(pqxx::connection& conn) {
    int id;
    string nuevoNombre, nuevaJornada;

    cout << "Ingrese ID de la seccion a modificar: "; cin >> id;
    cout << "Ingrese nuevo nombre: "; cin >> nuevoNombre;
    cout << "Ingrese nueva jornada: "; cin >> nuevaJornada;

    pqxx::work txn(conn);
    // Actualizamos el nombre y la jornada
    txn.exec("UPDATE secciones SET nombre = " + txn.quote(nuevoNombre) +
        ", jornada = " + txn.quote(nuevaJornada) +
        " WHERE id_secciones = " + txn.quote(id));
    txn.commit();
    cout << "Seccion modificada completamente." << endl;
}

void eliminarSeccion(pqxx::connection& conn) {
    int id; cout << "Ingrese ID de la seccion: "; cin >> id;
    pqxx::work txn(conn);
    pqxx::result r = txn.exec("SELECT COUNT(*) FROM alumnos_seccion WHERE seccion_id = " + txn.quote(id));
    if (r[0][0].as<int>() > 0) {
        cout << "No se puede eliminar: seccion con alumnos asignados." << endl;
        return;
    }
    txn.exec("DELETE FROM secciones WHERE id_secciones = " + txn.quote(id));
    txn.commit();
    cout << "Seccion eliminada correctamente." << endl;
}

void menuSecciones(pqxx::connection& conn) {
    int opcion;
    do {
        cout << "\n===== MODULO SECCIONES =====\n1. Listar\n2. Grabar\n3. Modificar\n4. Eliminar\n5. Regresar\nSeleccione: ";
        opcion = pedirOpcion();
        switch (opcion) {
        case 1: listarSecciones(conn); break;
        case 2: grabarSeccion(conn); break;
        case 3: modificarSeccion(conn); break;
        case 4: eliminarSeccion(conn); break;
        case 5: break;
        default: cout << "Opcion fuera de rango." << endl;
        }
    } while (opcion != 5);
}
void listarAlumnosCursos(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result r = txn.exec(
        "SELECT ac.id_alumnos_cursos, a.nombres || ' ' || a.apellidos AS alumno, c.nombre AS curso "
        "FROM alumnos_cursos ac "
        "JOIN alumnos a ON ac.alumno_id = a.id_alumnos "
        "JOIN cursos c ON ac.curso_id = c.id_cursos;"
    );
    cout << "\n--- Asignaciones Alumnos-Cursos ---\n";
    for (auto fila : r) {
        cout << fila[0].as<int>() << " | Alumno: " << fila[1].as<string>()
            << " | Curso: " << fila[2].as<string>() << endl;
    }
}

void asignarCursoAlumno(pqxx::connection& conn) {
    int alumno_id, curso_id;
    cout << "Ingrese ID del alumno: "; cin >> alumno_id;
    cout << "Ingrese ID del curso: "; cin >> curso_id;
    pqxx::work txn(conn);
    pqxx::result r = txn.exec("SELECT COUNT(*) FROM alumnos_cursos WHERE alumno_id = " +
        txn.quote(alumno_id) + " AND curso_id = " + txn.quote(curso_id));
    if (r[0][0].as<int>() > 0) {
        cout << "El alumno ya tiene asignado este curso." << endl;
        return;
    }
    txn.exec("INSERT INTO alumnos_cursos (alumno_id, curso_id) VALUES (" +
        txn.quote(alumno_id) + ", " + txn.quote(curso_id) + ")");
    txn.commit();
    cout << "Curso asignado al alumno correctamente." << endl;
}

void eliminarAsignacionCurso(pqxx::connection& conn) {
    int id; cout << "Ingrese ID de la asignacion: "; cin >> id;
    pqxx::work txn(conn);
    txn.exec("DELETE FROM alumnos_cursos WHERE id_alumnos_cursos = " + txn.quote(id));
    txn.commit();
    cout << "Asignacion eliminada correctamente." << endl;
}

void menuAlumnosCursos(pqxx::connection& conn) {
    int opcion;
    do {
        cout << "\n===== MODULO ALUMNOS-CURSOS =====\n1. Listar\n2. Asignar\n3. Eliminar\n4. Regresar\nSeleccione: ";
        opcion = pedirOpcion();
        switch (opcion) {
        case 1: listarAlumnosCursos(conn); break;
        case 2: asignarCursoAlumno(conn); break;
        case 3: eliminarAsignacionCurso(conn); break;
        case 4: break;
        default: cout << "Opcion fuera de rango." << endl;
        }
    } while (opcion != 4);
}
void listarAlumnosSeccion(pqxx::connection& conn) {
    pqxx::work txn(conn);
    pqxx::result r = txn.exec(
        "SELECT ase.id_alumnos_seccion, a.nombres || ' ' || a.apellidos AS alumno, s.nombre AS seccion "
        "FROM alumnos_seccion ase "
        "JOIN alumnos a ON ase.alumno_id = a.id_alumnos "
        "JOIN secciones s ON ase.seccion_id = s.id_secciones;"
    );
    cout << "\n--- Asignaciones Alumnos-Secciones ---\n";
    for (auto fila : r) {
        cout << fila[0].as<int>() << " | Alumno: " << fila[1].as<string>()
            << " | Seccion: " << fila[2].as<string>() << endl;
    }
}

void asignarSeccionAlumno(pqxx::connection& conn) {
    int alumno_id, seccion_id;
    cout << "Ingrese ID del alumno: "; cin >> alumno_id;
    cout << "Ingrese ID de la seccion: "; cin >> seccion_id;
    pqxx::work txn(conn);
    pqxx::result r = txn.exec("SELECT COUNT(*) FROM alumnos_seccion WHERE alumno_id = " + txn.quote(alumno_id));
    if (r[0][0].as<int>() > 0) {
        cout << "El alumno ya tiene una seccion asignada. Use la opcion de modificar." << endl;
        return;
    }
    txn.exec("INSERT INTO alumnos_seccion (alumno_id, seccion_id) VALUES (" +
        txn.quote(alumno_id) + ", " + txn.quote(seccion_id) + ")");
    txn.commit();
    cout << "Seccion asignada al alumno correctamente." << endl;
}

void modificarAsignacionSeccion(pqxx::connection& conn) {
    int alumno_id, nueva_seccion;
    cout << "Ingrese ID del alumno: "; cin >> alumno_id;
    cout << "Ingrese nueva seccion: "; cin >> nueva_seccion;
    pqxx::work txn(conn);
    txn.exec("UPDATE alumnos_seccion SET seccion_id = " + txn.quote(nueva_seccion) +
        " WHERE alumno_id = " + txn.quote(alumno_id));
    txn.commit();
    cout << "Seccion modificada correctamente." << endl;
}

void eliminarAsignacionSeccion(pqxx::connection& conn) {
    int id; cout << "Ingrese ID de la asignacion: "; cin >> id;
    pqxx::work txn(conn);
    txn.exec("DELETE FROM alumnos_seccion WHERE id_alumnos_seccion = " + txn.quote(id));
    txn.commit();
    cout << "Asignacion eliminada correctamente." << endl;
}

void menuAlumnosSeccion(pqxx::connection& conn) {
    int opcion;
    do {
        cout << "\n===== MODULO ALUMNOS-SECCION =====\n1. Listar\n2. Asignar\n3. Modificar\n4. Eliminar\n5. Regresar\nSeleccione: ";
        opcion = pedirOpcion();
        switch (opcion) {
        case 1: listarAlumnosSeccion(conn); break;
        case 2: asignarSeccionAlumno(conn); break;
        case 3: modificarAsignacionSeccion(conn); break;
        case 4: eliminarAsignacionSeccion(conn); break;
        case 5: break;
        default: cout << "Opcion fuera de rango." << endl;
        }
    } while (opcion != 5);
}
void mostrarMenuPrincipal() {
    cout << "\n===== SISTEMA UNIVERSIDAD =====" << endl;
    cout << "1. Alumnos" << endl;
    cout << "2. Cursos" << endl;
    cout << "3. Secciones" << endl;
    cout << "4. Alumnos - Cursos" << endl;
    cout << "5. Alumnos - Seccion" << endl;
    cout << "6. Salir" << endl;
    cout << "Seleccione una opcion: ";
}

int main() {
    try {
        pqxx::connection conn = conectar();
        int opcion;
        bool salir = false;

        while (!salir) {
            mostrarMenuPrincipal();
            opcion = pedirOpcion(); // Usamos la función aquí también

            switch (opcion) {
            case 1: menuAlumnos(conn); break;
            case 2: menuCursos(conn); break;
            case 3: menuSecciones(conn); break;
            case 4: menuAlumnosCursos(conn); break;
            case 5: menuAlumnosSeccion(conn); break;
            case 6: salir = true; break;
            default: cout << "Opcion fuera de rango." << endl; break;
            }
        }
        cout << "Conexion cerrada correctamente." << endl;
    }
    catch (const exception& e) {
        cerr << "Error crítico: " << e.what() << endl;
        return 1;
    }
    return 0;
}