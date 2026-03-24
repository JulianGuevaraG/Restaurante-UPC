/*
 ╔══════════════════════════════════════════════════════╗
 ║   RESTAURANTE «DONDE SIEMPRE»                        ║
 ║   Sistema de Gestion — PGSR v1.0                     ║
 ╠══════════════════════════════════════════════════════╣
 ║   Universidad Popular del Cesar                      ║
 ║   Facultad de Ingenierias y Tecnologicas             ║
 ║   Ingenieria de Sistemas — Estructura de Datos       ║
 ║   Parcial 1 — Grupo 06 — 2026                        ║
 ╚══════════════════════════════════════════════════════╝
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <numeric>
#include <limits>
#include <random>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ──────────────────────────────────────────────────────
//  COLORES ANSI
// ──────────────────────────────────────────────────────
const string RST= "\033[0m";
const string BLD= "\033[1m";
const string RED= "\033[91m";
const string GRN= "\033[92m";
const string YEL= "\033[93m";
const string BLU= "\033[94m";
const string MAG= "\033[95m";
const string CYN= "\033[96m";
const string WHT= "\033[97m";
const string GRY= "\033[90m";

// ──────────────────────────────────────────────────────
//  CONSTANTES
// ──────────────────────────────────────────────────────
// Decretos 1469 y 1470 del 29 de diciembre de 2025
const double SMLV       = 1750905.0;
const double AUX_TRANSP = 249095.0;

// ──────────────────────────────────────────────────────
//  ESTRUCTURAS DE DATOS
// ──────────────────────────────────────────────────────
// MODULO 1 — Ingrediente (materia prima)
struct Ingrediente {
    int    id;
    string nombre, categoria, unidad, creado;
    double precio, stock;
    bool   activo;
};

// MODULO 2 — Plato del menu
struct IngPlato {               // ingrediente dentro de un plato
    int    ing_id;
    string nombre;
    double cantidad;
};

struct Plato {
    int              id, stock;
    string           nombre, creado;
    double           precio_venta;
    vector<IngPlato> ingredientes;
    bool             activo;
};

struct Proveedor {
    int    id;
    string nombre, contacto, telefono, email, productos_sum, creado;
    bool   activo;
};

struct ItemCompra {
    int    prod_id;
    double cant;
    string nombre;
    double pu, sub;
};

struct Compra {
    int              id, proveedor_id;
    string           proveedor, fecha, estado, notas, fecha_recepcion;
    vector<ItemCompra> items;
    double           total;
};

struct Mesa {
    int    id, numero, capacidad;
    string ubicacion, estado;
    bool   activo;
};

struct Cliente {
    int    id, visitas, puntos;
    string nombre, telefono, email, direccion, nivel, creado;
    double gastado;
    bool   activo;
};

struct Reserva {
    int    id, cliente_id, mesa_id, mesa_num, personas;
    string cliente, fecha, hora, especiales, estado, creado;
};

struct ItemPedido {
    int    prod_id, cant;
    string nombre, nota;
    double pu, sub;
};

struct Pedido {
    int              id, cliente_id, mesa_id, mesa_num;
    string           tipo, fecha, estado, cliente, direccion, notas;
    vector<ItemPedido> items;
    double           total;
};

struct Empleado {
    int    id;
    string nombre, documento, telefono, email, rol, contrato, ingreso;
    double salario;
    bool   activo;
};

struct Nomina {
    int    id, emp_id;
    string empleado, rol, periodo, generado;
    double salario_base, aux_transporte;
    double ded_salud, ded_pension, total_ded, salario_neto;
    double apt_salud, apt_pension, arl, caja, icbf, sena, total_apt;
    double cesantias, int_ces, prima, vacaciones, total_pres;
    double costo_total;
};

// ──────────────────────────────────────────────────────
//  BASE DE DATOS EN MEMORIA
// ──────────────────────────────────────────────────────
vector<Ingrediente> ingredientes;
vector<Plato>       platos;
vector<Proveedor>   proveedores;
vector<Compra>      compras;
vector<Mesa>        mesas;
vector<Cliente>     clientes;
vector<Reserva>     reservas;
vector<Pedido>      pedidos;
vector<Empleado>    empleados;
vector<Nomina>      nominas;

int id_ing=0, id_plato=0, id_prov=0, id_comp=0, id_mesa=0,
    id_cli=0, id_res=0,   id_ped=0,  id_emp=0,  id_nom=0;

// ──────────────────────────────────────────────────────
//  UTILIDADES
// ──────────────────────────────────────────────────────
void limpiar() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    cout << "\n" << GRY << "Presione ENTER para continuar..." << RST;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string readline(const string& prompt = "") {
    if (!prompt.empty()) cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

int pint(const string& prompt, int mn = INT_MIN, int mx = INT_MAX) {
    while (true) {
        cout << prompt;
        string line; getline(cin, line);
        try {
            if (line.empty()) { cout << RED << "✗ Ingrese un numero\n" << RST; continue; }
            int v = stoi(line);
            if (v < mn) { cout << RED << "✗ Minimo " << mn << "\n" << RST; continue; }
            if (v > mx) { cout << RED << "✗ Maximo " << mx << "\n" << RST; continue; }
            return v;
        } catch (...) { cout << RED << "✗ Ingrese un numero entero\n" << RST; }
    }
}

double pfloat(const string& prompt, double mn = 0.0) {
    while (true) {
        cout << prompt;
        string line; getline(cin, line);
        try {
            if (line.empty()) { cout << RED << "✗ Ingrese un numero\n" << RST; continue; }
            double v = stod(line);
            if (v < mn) { cout << RED << "✗ Minimo " << fixed << setprecision(0) << mn << "\n" << RST; continue; }
            return v;
        } catch (...) { cout << RED << "✗ Ingrese un numero valido\n" << RST; }
    }
}

string popc(const vector<string>& opts) {
    while (true) {
        cout << "Opcion: ";
        string o; getline(cin, o);
        for (auto& x : opts) if (o == x) return o;
        cout << RED << "✗ Opcion invalida\n" << RST;
    }
}

string mmenu(const vector<pair<string,string>>& ops) {
    cout << "\n";
    vector<string> keys;
    for (auto& [k, label] : ops) {
        if (k == "0") cout << "\n  " << RED << " 0." << RST << " " << label << "\n";
        else          cout << "  " << CYN << setw(2) << right << k << "." << RST << " " << label << "\n";
        keys.push_back(k);
    }
    cout << "\n";
    return popc(keys);
}

void titulo(const string& t) {
    string up = t;
    transform(up.begin(), up.end(), up.begin(), ::toupper);
    cout << "\n" << BLD << BLU << string(58,'=') << RST << "\n";
    cout << BLD << CYN << "  " << up << RST << "\n";
    cout << BLD << BLU << string(58,'=') << RST << "\n\n";
}

void sub(const string& t) {
    cout << "\n" << BLD << YEL << "── " << t << " ──" << RST << "\n\n";
}

void ok(const string& m)   { cout << GRN << "✓ " << m << RST << "\n"; }
void err(const string& m)  { cout << RED << "✗ " << m << RST << "\n"; }
void warn(const string& m) { cout << YEL << "⚠ " << m << RST << "\n"; }

string pesos(double v) {
    // Format as $1.234.567
    ostringstream oss;
    long long iv = (long long)round(v);
    string s = to_string(iv);
    string result;
    int cnt = 0;
    for (int i = (int)s.size()-1; i >= 0; i--) {
        if (cnt > 0 && cnt % 3 == 0) result = "." + result;
        result = s[i] + result;
        cnt++;
    }
    return "$" + result;
}

string hoy() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", t);
    return string(buf);
}

string ahora() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[17];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", t);
    return string(buf);
}

string hoy_col() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%d/%m/%Y", t);
    return string(buf);
}

string pedir_fecha_col(const string& prompt) {
    while (true) {
        string val = readline(prompt);
        if (val.empty()) return hoy_col();
        // Validate DD/MM/YYYY
        if (val.size() == 10 && val[2]=='/' && val[5]=='/') {
            try {
                int d = stoi(val.substr(0,2));
                int m = stoi(val.substr(3,2));
                int y = stoi(val.substr(6,4));
                if (d>=1&&d<=31 && m>=1&&m<=12 && y>=2000) return val;
            } catch (...) {}
        }
        err("Formato invalido. Use DD/MM/AAAA (ej: 24/03/2026)");
    }
}

string pedir_fecha_reserva(const string& prompt) {
    // Valida que la fecha sea hoy hasta 31 dias adelante
    time_t now = time(nullptr);
    tm* tn = localtime(&now);
    // hoy como numeros
    int hd = tn->tm_mday, hm = tn->tm_mon+1, hy = tn->tm_year+1900;
    // max: now + 31 dias
    time_t t31 = now + 31*86400;
    tm* tm31 = localtime(&t31);
    int md = tm31->tm_mday, mm_ = tm31->tm_mon+1, my = tm31->tm_year+1900;
    char buf[11]; strftime(buf,sizeof(buf),"%d/%m/%Y",tm31);
    string smax = string(buf);
    while (true) {
        string val = readline(prompt);
        if (val.empty()) val = hoy_col();
        if (val.size()==10 && val[2]=='/' && val[5]=='/') {
            try {
                int d=stoi(val.substr(0,2)), m=stoi(val.substr(3,2)), y=stoi(val.substr(6,4));
                if (d>=1&&d<=31&&m>=1&&m<=12&&y>=2000) {
                    // Comparar fechas
                    int fval = y*10000+m*100+d;
                    int fhoy = hy*10000+hm*100+hd;
                    int fmax = my*10000+mm_*100+md;
                    if (fval < fhoy) { err("La fecha no puede ser en el pasado."); continue; }
                    if (fval > fmax) { err("Maximo 31 dias adelante ("+smax+")."); continue; }
                    return val;
                }
            } catch (...) {}
        }
        err("Formato invalido. Use DD/MM/AAAA (ej: 24/03/2026)");
    }
}

string pedir_periodo_col(const string& prompt) {
    // Pide periodo en formato MM/AA con validacion
    time_t now = time(nullptr); tm* t = localtime(&now);
    char buf[6]; strftime(buf,sizeof(buf),"%m/%y",t);
    string def = string(buf);
    while (true) {
        cout << prompt << "(Enter=" << def << "): ";
        string val = readline();
        if (val.empty()) val = def;
        if (val.size()==5 && val[2]=='/') {
            try {
                int mm = stoi(val.substr(0,2));
                stoi(val.substr(3,2));
                if (mm>=1 && mm<=12) return val;
            } catch (...) {}
        }
        err("Formato invalido. Use MM/AA (ej: 03/26)");
    }
}

string unidad_sim(const string& cat) {
    if (cat=="Plato Principal"||cat=="Entrada"||cat=="Postre") return "porcion";
    if (cat=="Bebida") {
        static const vector<string> ops={"unidad","litro","ml"};
        return ops[rand()%3];
    }
    if (cat=="Insumo") {
        static const vector<string> ops={"kg","g","litro","ml"};
        return ops[rand()%4];
    }
    return "unidad";
}

template<typename T>
T* bid(vector<T>& vec, int id) {
    for (auto& x : vec) if (x.id == id) return &x;
    return nullptr;
}

string barra(double v, double mx, int w = 25) {
    if (mx <= 0) return "";
    int f = (int)((v/mx)*w);
    f = min(f, w);
    return CYN + string(f,'#') + GRY + string(w-f,'-') + RST;
    // fallback ASCII
}

string barraASCII(double v, double mx, int w = 25) {
    if (mx <= 0) return string(w, ' ');
    int f = (int)((v/mx)*w);
    f = min(f, w);
    return CYN + string(f, '#') + GRY + string(w-f, '-') + RST;
}

string esc(const string& s) {
    // Reemplaza | por [P] y ~ por [T] para evitar conflictos
    string r = s;
    for (auto& c : r) { if (c=='|') c='\x01'; if (c=='~') c='\x02'; }
    return r;
}
string unesc(const string& s) {
    string r = s;
    for (auto& c : r) { if (c=='\x01') c='|'; if (c=='\x02') c='~'; }
    return r;
}

vector<string> split(const string& s, char delim) {
    vector<string> tokens;
    string token;
    istringstream ss(s);
    while (getline(ss, token, delim)) tokens.push_back(token);
    return tokens;
}

string encodeIngPlato(const vector<IngPlato>& ings) {
    string r;
    for (size_t i=0; i<ings.size(); i++) {
        if (i>0) r += "~";
        r += to_string(ings[i].ing_id) + ";" + esc(ings[i].nombre) + ";" + to_string(ings[i].cantidad);
    }
    return r;
}

vector<IngPlato> decodeIngPlato(const string& s) {
    vector<IngPlato> ings;
    if (s.empty()) return ings;
    for (auto& part : split(s, '~')) {
        auto f = split(part, ';');
        if (f.size() < 3) continue;
        IngPlato ip; ip.ing_id=stoi(f[0]); ip.nombre=unesc(f[1]); ip.cantidad=stod(f[2]);
        ings.push_back(ip);
    }
    return ings;
}

string encodeItemsCompra(const vector<ItemCompra>& items) {
    string r;
    for (size_t i = 0; i < items.size(); i++) {
        if (i>0) r += "~";
        r += to_string(items[i].prod_id) + ";" + esc(items[i].nombre) + ";" +
             to_string(items[i].cant) + ";" + to_string(items[i].pu) + ";" + to_string(items[i].sub);
    }
    return r;
}

string encodeItemsPedido(const vector<ItemPedido>& items) {
    string r;
    for (size_t i = 0; i < items.size(); i++) {
        if (i>0) r += "~";
        r += to_string(items[i].prod_id) + ";" + esc(items[i].nombre) + ";" +
             to_string(items[i].cant) + ";" + to_string(items[i].pu) + ";" +
             to_string(items[i].sub) + ";" + esc(items[i].nota);
    }
    return r;
}

vector<ItemCompra> decodeItemsCompra(const string& s) {
    vector<ItemCompra> items;
    if (s.empty()) return items;
    for (auto& part : split(s, '~')) {
        auto f = split(part, ';');
        if (f.size() < 5) continue;
        ItemCompra it;
        it.prod_id = stoi(f[0]); it.nombre = unesc(f[1]);
        it.cant    = stod(f[2]); it.pu = stod(f[3]); it.sub = stod(f[4]);
        items.push_back(it);
    }
    return items;
}

vector<ItemPedido> decodeItemsPedido(const string& s) {
    vector<ItemPedido> items;
    if (s.empty()) return items;
    for (auto& part : split(s, '~')) {
        auto f = split(part, ';');
        if (f.size() < 6) continue;
        ItemPedido it;
        it.prod_id = stoi(f[0]); it.nombre = unesc(f[1]);
        it.cant    = stoi(f[2]); it.pu = stod(f[3]);
        it.sub     = stod(f[4]); it.nota = unesc(f[5]);
        items.push_back(it);
    }
    return items;
}

void guardar() {
    // IDs
    { ofstream f("ids.dat");
      f << "ingredientes:"<<id_ing<<"|platos:"<<id_plato<<"|proveedores:"<<id_prov<<"|compras:"<<id_comp<<"|mesas:"<<id_mesa<<"|"
        << "clientes:"<<id_cli<<"|reservas:"<<id_res<<"|pedidos:"<<id_ped<<"|empleados:"<<id_emp<<"|nominas:"<<id_nom; }
    // Ingredientes
    { ofstream f("ingredientes.dat");
      for (auto& i : ingredientes)
        f << i.id<<"|"<<esc(i.nombre)<<"|"<<esc(i.categoria)<<"|"<<esc(i.unidad)<<"|"
          <<i.precio<<"|"<<i.stock<<"|"<<(i.activo?1:0)<<"|"<<esc(i.creado)<<"\n"; }
    // Platos
    { ofstream f("platos.dat");
      for (auto& d : platos)
        f << d.id<<"|"<<esc(d.nombre)<<"|"<<d.precio_venta<<"|"<<d.stock<<"|"
          <<(d.activo?1:0)<<"|"<<esc(d.creado)<<"|"<<encodeIngPlato(d.ingredientes)<<"\n"; }
    // Proveedores
    { ofstream f("prov.dat");
      for (auto& p : proveedores)
        f << p.id<<"|"<<esc(p.nombre)<<"|"<<esc(p.contacto)<<"|"<<esc(p.telefono)<<"|"
          <<esc(p.email)<<"|"<<esc(p.productos_sum)<<"|"<<(p.activo?1:0)<<"|"<<esc(p.creado)<<"\n"; }
    // Compras
    { ofstream f("compras.dat");
      for (auto& c : compras)
        f << c.id<<"|"<<c.proveedor_id<<"|"<<esc(c.proveedor)<<"|"<<c.fecha<<"|"
          <<c.total<<"|"<<esc(c.estado)<<"|"<<esc(c.notas)<<"|"<<esc(c.fecha_recepcion)<<"|"
          <<encodeItemsCompra(c.items)<<"\n"; }
    // Mesas
    { ofstream f("mesas.dat");
      for (auto& m : mesas)
        f << m.id<<"|"<<m.numero<<"|"<<m.capacidad<<"|"<<esc(m.ubicacion)<<"|"
          <<esc(m.estado)<<"|"<<(m.activo?1:0)<<"\n"; }
    // Clientes
    { ofstream f("clientes.dat");
      for (auto& c : clientes)
        f << c.id<<"|"<<esc(c.nombre)<<"|"<<esc(c.telefono)<<"|"<<esc(c.email)<<"|"
          <<esc(c.direccion)<<"|"<<c.visitas<<"|"<<c.gastado<<"|"<<c.puntos<<"|"
          <<esc(c.nivel)<<"|"<<(c.activo?1:0)<<"|"<<esc(c.creado)<<"\n"; }
    // Reservas
    { ofstream f("reservas.dat");
      for (auto& r : reservas)
        f << r.id<<"|"<<r.cliente_id<<"|"<<esc(r.cliente)<<"|"<<r.mesa_id<<"|"<<r.mesa_num<<"|"
          <<esc(r.fecha)<<"|"<<esc(r.hora)<<"|"<<r.personas<<"|"<<esc(r.especiales)<<"|"
          <<esc(r.estado)<<"|"<<esc(r.creado)<<"\n"; }
    // Pedidos
    { ofstream f("pedidos.dat");
      for (auto& p : pedidos)
        f << p.id<<"|"<<p.cliente_id<<"|"<<esc(p.cliente)<<"|"<<esc(p.tipo)<<"|"
          <<esc(p.fecha)<<"|"<<p.total<<"|"<<esc(p.estado)<<"|"<<p.mesa_id<<"|"<<p.mesa_num<<"|"
          <<esc(p.direccion)<<"|"<<esc(p.notas)<<"|"<<encodeItemsPedido(p.items)<<"\n"; }
    // Empleados
    { ofstream f("empleados.dat");
      for (auto& e : empleados)
        f << e.id<<"|"<<esc(e.nombre)<<"|"<<esc(e.documento)<<"|"<<esc(e.telefono)<<"|"
          <<esc(e.email)<<"|"<<esc(e.rol)<<"|"<<e.salario<<"|"<<esc(e.contrato)<<"|"
          <<esc(e.ingreso)<<"|"<<(e.activo?1:0)<<"\n"; }
    // Nominas
    { ofstream f("nominas.dat");
      for (auto& n : nominas)
        f << n.id<<"|"<<n.emp_id<<"|"<<esc(n.empleado)<<"|"<<esc(n.rol)<<"|"<<esc(n.periodo)<<"|"
          <<esc(n.generado)<<"|"<<n.salario_base<<"|"<<n.aux_transporte<<"|"
          <<n.ded_salud<<"|"<<n.ded_pension<<"|"<<n.total_ded<<"|"<<n.salario_neto<<"|"
          <<n.apt_salud<<"|"<<n.apt_pension<<"|"<<n.arl<<"|"<<n.caja<<"|"<<n.icbf<<"|"<<n.sena<<"|"
          <<n.total_apt<<"|"<<n.cesantias<<"|"<<n.int_ces<<"|"<<n.prima<<"|"
          <<n.vacaciones<<"|"<<n.total_pres<<"|"<<n.costo_total<<"\n"; }
    ok("Datos guardados correctamente");
}

bool cargar() {
    ifstream ids("ids.dat");
    if (!ids.is_open()) { warn("Archivo no encontrado. Iniciando vacio."); return false; }
    { string line; getline(ids, line);
      auto f = split(line, '|');
      if (!f.empty() && f[0].find(':') != string::npos) {
        // Formato etiquetado: "productos:5|proveedores:3|..."
        map<string,int*> id_map = {
            {"ingredientes",&id_ing},{"platos",&id_plato},
            {"proveedores",&id_prov},{"compras",&id_comp},{"mesas",&id_mesa},
            {"clientes",&id_cli},{"reservas",&id_res},{"pedidos",&id_ped},{"empleados",&id_emp},{"nominas",&id_nom}
        };
        for (auto& part : f) {
            auto sep = part.find(':');
            if (sep != string::npos) {
                string key = part.substr(0, sep);
                if (id_map.count(key)) *id_map[key] = stoi(part.substr(sep+1));
            }
        }
      } else if (f.size() >= 9) {
        // Formato legacy (sin etiquetas) — ignorado, se usa formato etiquetado
        id_prov=stoi(f[1]); id_comp=stoi(f[2]); id_mesa=stoi(f[3]);
        id_cli=stoi(f[4]);  id_res=stoi(f[5]);   id_ped=stoi(f[6]);
        id_emp=stoi(f[7]);  id_nom=stoi(f[8]);
      }
    }
    // Ingredientes
    { ifstream f("ingredientes.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<8) continue;
        Ingrediente i; i.id=stoi(t[0]); i.nombre=unesc(t[1]); i.categoria=unesc(t[2]);
        i.unidad=unesc(t[3]); i.precio=stod(t[4]); i.stock=stod(t[5]);
        i.activo=stoi(t[6]); i.creado=unesc(t[7]); ingredientes.push_back(i); }}
    // Platos
    { ifstream f("platos.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<7) continue;
        Plato d; d.id=stoi(t[0]); d.nombre=unesc(t[1]); d.precio_venta=stod(t[2]);
        d.stock=stoi(t[3]); d.activo=stoi(t[4]); d.creado=unesc(t[5]);
        d.ingredientes=decodeIngPlato(t[6]); platos.push_back(d); }}
    // Proveedores
    { ifstream f("prov.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<8) continue;
        Proveedor p; p.id=stoi(t[0]); p.nombre=unesc(t[1]); p.contacto=unesc(t[2]);
        p.telefono=unesc(t[3]); p.email=unesc(t[4]); p.productos_sum=unesc(t[5]);
        p.activo=stoi(t[6]); p.creado=unesc(t[7]); proveedores.push_back(p); }}
    // Compras
    { ifstream f("compras.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<9) continue;
        Compra c; c.id=stoi(t[0]); c.proveedor_id=stoi(t[1]); c.proveedor=unesc(t[2]);
        c.fecha=t[3]; c.total=stod(t[4]); c.estado=unesc(t[5]); c.notas=unesc(t[6]);
        c.fecha_recepcion=unesc(t[7]); c.items=decodeItemsCompra(t[8]); compras.push_back(c); }}
    // Mesas
    { ifstream f("mesas.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<6) continue;
        Mesa m; m.id=stoi(t[0]); m.numero=stoi(t[1]); m.capacidad=stoi(t[2]);
        m.ubicacion=unesc(t[3]); m.estado=unesc(t[4]); m.activo=stoi(t[5]); mesas.push_back(m); }}
    // Clientes
    { ifstream f("clientes.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<11) continue;
        Cliente c; c.id=stoi(t[0]); c.nombre=unesc(t[1]); c.telefono=unesc(t[2]);
        c.email=unesc(t[3]); c.direccion=unesc(t[4]); c.visitas=stoi(t[5]);
        c.gastado=stod(t[6]); c.puntos=stoi(t[7]); c.nivel=unesc(t[8]);
        c.activo=stoi(t[9]); c.creado=unesc(t[10]); clientes.push_back(c); }}
    // Reservas
    { ifstream f("reservas.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<11) continue;
        Reserva r; r.id=stoi(t[0]); r.cliente_id=stoi(t[1]); r.cliente=unesc(t[2]);
        r.mesa_id=stoi(t[3]); r.mesa_num=stoi(t[4]); r.fecha=unesc(t[5]); r.hora=unesc(t[6]);
        r.personas=stoi(t[7]); r.especiales=unesc(t[8]); r.estado=unesc(t[9]);
        r.creado=unesc(t[10]); reservas.push_back(r); }}
    // Pedidos
    { ifstream f("pedidos.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<12) continue;
        Pedido p; p.id=stoi(t[0]); p.cliente_id=stoi(t[1]); p.cliente=unesc(t[2]);
        p.tipo=unesc(t[3]); p.fecha=unesc(t[4]); p.total=stod(t[5]); p.estado=unesc(t[6]);
        p.mesa_id=stoi(t[7]); p.mesa_num=stoi(t[8]); p.direccion=unesc(t[9]);
        p.notas=unesc(t[10]); p.items=decodeItemsPedido(t[11]); pedidos.push_back(p); }}
    // Empleados
    { ifstream f("empleados.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<10) continue;
        Empleado e; e.id=stoi(t[0]); e.nombre=unesc(t[1]); e.documento=unesc(t[2]);
        e.telefono=unesc(t[3]); e.email=unesc(t[4]); e.rol=unesc(t[5]);
        e.salario=stod(t[6]); e.contrato=unesc(t[7]); e.ingreso=unesc(t[8]);
        e.activo=stoi(t[9]); empleados.push_back(e); }}
    // Nominas
    { ifstream f("nominas.dat"); string line;
      while (getline(f, line)) { auto t=split(line,'|'); if(t.size()<25) continue;
        Nomina n; n.id=stoi(t[0]); n.emp_id=stoi(t[1]); n.empleado=unesc(t[2]);
        n.rol=unesc(t[3]); n.periodo=unesc(t[4]); n.generado=unesc(t[5]);
        n.salario_base=stod(t[6]); n.aux_transporte=stod(t[7]);
        n.ded_salud=stod(t[8]); n.ded_pension=stod(t[9]); n.total_ded=stod(t[10]);
        n.salario_neto=stod(t[11]); n.apt_salud=stod(t[12]); n.apt_pension=stod(t[13]);
        n.arl=stod(t[14]); n.caja=stod(t[15]); n.icbf=stod(t[16]); n.sena=stod(t[17]);
        n.total_apt=stod(t[18]); n.cesantias=stod(t[19]); n.int_ces=stod(t[20]);
        n.prima=stod(t[21]); n.vacaciones=stod(t[22]); n.total_pres=stod(t[23]);
        n.costo_total=stod(t[24]); nominas.push_back(n); }}
    ok("Datos cargados correctamente");
    return true;
}

// ──────────────────────────────────────────────────────
//  MODULO 1 — INGREDIENTES
// ──────────────────────────────────────────────────────
const vector<string> CATS_ING = {"verduras","carnes","aceites","granos","lacteos","frutas","condimentos"};
const vector<string> UNDS_ING = {"kg","litro","unidad","gramos"};

void ing_listar(bool activos = true) {
    vector<Ingrediente*> rows;
    for (auto& i : ingredientes) if (!activos || i.activo) rows.push_back(&i);
    if (rows.empty()) { warn("Sin ingredientes."); return; }
    cout << "\n" << left
         << setw(5)<<"ID" << setw(24)<<"Nombre" << setw(14)<<"Categoria"
         << setw(9)<<"Unidad" << right << setw(12)<<"Precio/u" << setw(10)<<"Stock"
         << "  Estado\n";
    cout << string(82,'-') << "\n";
    for (auto* i : rows) {
        string st = i->activo ? GRN+"Activo"+RST : RED+"Inactivo"+RST;
        cout << left << setw(5)<<i->id << setw(24)<<i->nombre.substr(0,23)
             << setw(14)<<i->categoria << setw(9)<<i->unidad
             << right << setw(12)<<pesos(i->precio) << setw(10)<<fixed<<setprecision(2)<<i->stock
             << "  " << st << "\n";
    }
}

void ing_crear() {
    titulo("Nuevo Ingrediente");
    Ingrediente i; i.id=++id_ing; i.activo=true; i.creado=hoy();
    i.nombre = readline("Nombre del ingrediente: ");
    cout << "\n";
    for (size_t k=0; k<CATS_ING.size(); k++) cout << "  " << CYN << k+1 << "." << RST << " " << CATS_ING[k] << "\n";
    cout << "\n";
    i.categoria = CATS_ING[pint("Categoria #: ",1,(int)CATS_ING.size())-1];
    cout << "\n";
    for (size_t k=0; k<UNDS_ING.size(); k++) cout << "  " << CYN << k+1 << "." << RST << " " << UNDS_ING[k] << "\n";
    cout << "\n";
    i.unidad = UNDS_ING[pint("Unidad #: ",1,(int)UNDS_ING.size())-1];
    i.precio = pfloat("Precio por unidad ($): ", 0.01);
    i.stock  = pfloat("Cantidad en stock: ", 0);
    ingredientes.push_back(i);
    ok("Ingrediente '" + i.nombre + "' creado ID#" + to_string(i.id));
}

void ing_modificar() {
    ing_listar(false);
    if (ingredientes.empty()) return;
    Ingrediente* i = bid(ingredientes, pint("ID: ",1));
    if (!i) { err("No encontrado"); return; }
    sub("Modificando: " + i->nombre);
    cout << "  Categoria : " << i->categoria << "\n";
    cout << "  Unidad    : " << i->unidad    << "\n";
    cout << "  Precio    : " << pesos(i->precio) << "\n";
    cout << "  Stock     : " << i->stock     << "\n";
    string o = mmenu({{"1","Nombre"},{"2","Categoria"},{"3","Unidad"},
                      {"4","Precio por unidad"},{"5","Ajuste stock (+/-)"},{"0","Cancelar"}});
    if (o=="1") i->nombre = readline("Nuevo nombre: ");
    else if (o=="2") {
        for (size_t k=0; k<CATS_ING.size(); k++) cout << "  " << CYN << k+1 << "." << RST << " " << CATS_ING[k] << "\n";
        i->categoria = CATS_ING[pint("Cat #: ",1,(int)CATS_ING.size())-1];
    }
    else if (o=="3") {
        for (size_t k=0; k<UNDS_ING.size(); k++) cout << "  " << CYN << k+1 << "." << RST << " " << UNDS_ING[k] << "\n";
        i->unidad = UNDS_ING[pint("Unidad #: ",1,(int)UNDS_ING.size())-1];
    }
    else if (o=="4") i->precio = pfloat("Nuevo precio ($): ", 0.01);
    else if (o=="5") {
        double d = pfloat("Ajuste (+/-): ");
        i->stock = max(0.0, i->stock + d);
        ok("Stock actualizado: " + to_string(i->stock));
    }
    if (o!="0") ok("Ingrediente actualizado");
}

void menu_ingredientes() {
    while (true) {
        titulo("Modulo 1 — Ingredientes");
        string o = mmenu({{"1","Nuevo ingrediente"},{"2","Listar todos"},
                          {"3","Modificar ingrediente"},{"4","Activar / Desactivar"},
                          {"5","Eliminar ingrediente"},{"0","Volver"}});
        if (o=="1") ing_crear();
        else if (o=="2") { ing_listar(false); pausar(); }
        else if (o=="3") ing_modificar();
        else if (o=="4") {
            ing_listar(false);
            if (!ingredientes.empty()) {
                Ingrediente* i = bid(ingredientes, pint("ID: ",1));
                if (i) { i->activo = !i->activo; ok(i->activo?"Activado":"Desactivado"); }
                else err("No encontrado");
            }
        }
        else if (o=="5") {
            ing_listar(false);
            if (!ingredientes.empty()) {
                int id = pint("ID: ",1);
                auto it = find_if(ingredientes.begin(), ingredientes.end(), [id](auto& x){ return x.id==id; });
                if (it != ingredientes.end()) {
                    cout << RED << "Eliminar? (s/n): " << RST;
                    string r; getline(cin,r);
                    if (r=="s") { ingredientes.erase(it); ok("Eliminado"); }
                } else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 2 — PLATOS
// ──────────────────────────────────────────────────────
void plato_listar(bool activos = true) {
    vector<Plato*> rows;
    for (auto& d : platos) if (!activos || d.activo) rows.push_back(&d);
    if (rows.empty()) { warn("Sin platos."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(28)<<"Nombre"
         << right << setw(14)<<"Precio venta" << setw(8)<<"Stock" << "  Estado\n";
    cout << string(64,'-') << "\n";
    for (auto* d : rows) {
        string st = d->activo ? GRN+"Activo"+RST : RED+"Inactivo"+RST;
        cout << left << setw(5)<<d->id << setw(28)<<d->nombre.substr(0,27)
             << right << setw(14)<<pesos(d->precio_venta) << setw(8)<<d->stock
             << "  " << st << "\n";
        for (auto& ing : d->ingredientes)
            cout << "     " << GRY << "  -> " << left << setw(22)<<ing.nombre
                 << fixed<<setprecision(2)<<ing.cantidad << RST << "\n";
    }
}

void plato_crear() {
    titulo("Nuevo Plato");
    if (ingredientes.empty()) { warn("No hay ingredientes. Registra ingredientes primero."); return; }
    Plato d; d.id=++id_plato; d.activo=true; d.creado=hoy();
    d.nombre = readline("Nombre del plato: ");
    while (true) {
        ing_listar();
        string ans = readline("\nAnadir ingrediente al plato? (s/n): ");
        if (ans!="s") break;
        Ingrediente* ing = bid(ingredientes, pint("ID ingrediente: ",1));
        if (!ing || !ing->activo) { err("Ingrediente no encontrado o inactivo"); continue; }
        bool ya = false;
        for (auto& x : d.ingredientes) if (x.ing_id==ing->id) { ya=true; break; }
        if (ya) { warn("'" + ing->nombre + "' ya esta en el plato"); continue; }
        double cant = pfloat("Cantidad de " + ing->nombre + " (" + ing->unidad + "): ", 0.01);
        d.ingredientes.push_back({ing->id, ing->nombre, cant});
        ok("  +" + to_string(cant) + " " + ing->unidad + " de " + ing->nombre);
    }
    if (d.ingredientes.empty()) {
        warn("El plato debe tener al menos un ingrediente, cancelado.");
        id_plato--; return;
    }
    d.precio_venta = pfloat("Precio de venta ($): ", 0.01);
    d.stock        = pint("Stock disponible (unidades): ", 0);
    platos.push_back(d);
    ok("Plato '" + d.nombre + "' creado ID#" + to_string(d.id));
}

void plato_modificar() {
    plato_listar(false);
    if (platos.empty()) return;
    Plato* d = bid(platos, pint("ID: ",1));
    if (!d) { err("No encontrado"); return; }
    sub("Modificando: " + d->nombre);
    cout << "  Precio venta : " << pesos(d->precio_venta) << "\n";
    cout << "  Stock        : " << d->stock << "\n";
    string o = mmenu({{"1","Nombre"},{"2","Precio de venta"},{"3","Stock"},
                      {"4","Anadir ingrediente"},{"5","Quitar ingrediente"},{"0","Cancelar"}});
    if (o=="1") d->nombre = readline("Nuevo nombre: ");
    else if (o=="2") d->precio_venta = pfloat("Nuevo precio ($): ", 0.01);
    else if (o=="3") d->stock = pint("Nuevo stock: ", 0);
    else if (o=="4") {
        ing_listar();
        Ingrediente* ing = bid(ingredientes, pint("ID ingrediente: ",1));
        if (ing && ing->activo) {
            double cant = pfloat("Cantidad (" + ing->unidad + "): ", 0.01);
            d->ingredientes.push_back({ing->id, ing->nombre, cant});
            ok("Ingrediente anadido");
        } else err("Ingrediente no encontrado o inactivo");
    }
    else if (o=="5") {
        if (d->ingredientes.empty()) { warn("Sin ingredientes en este plato"); return; }
        for (size_t k=0; k<d->ingredientes.size(); k++)
            cout << "  " << CYN << k+1 << "." << RST << " " << d->ingredientes[k].nombre
                 << " (x" << d->ingredientes[k].cantidad << ")\n";
        int idx = pint("# a eliminar: ",1,(int)d->ingredientes.size())-1;
        string nm = d->ingredientes[idx].nombre;
        d->ingredientes.erase(d->ingredientes.begin()+idx);
        ok("'" + nm + "' eliminado del plato");
    }
    if (o!="0") ok("Plato actualizado");
}

void menu_platos() {
    while (true) {
        titulo("Modulo 2 — Platos");
        string o = mmenu({{"1","Nuevo plato"},{"2","Listar todos"},
                          {"3","Modificar plato"},{"4","Activar / Desactivar"},
                          {"5","Eliminar plato"},{"0","Volver"}});
        if (o=="1") plato_crear();
        else if (o=="2") { plato_listar(false); pausar(); }
        else if (o=="3") plato_modificar();
        else if (o=="4") {
            plato_listar(false);
            if (!platos.empty()) {
                Plato* d = bid(platos, pint("ID: ",1));
                if (d) { d->activo = !d->activo; ok(d->activo?"Activado":"Desactivado"); }
                else err("No encontrado");
            }
        }
        else if (o=="5") {
            plato_listar(false);
            if (!platos.empty()) {
                int id = pint("ID: ",1);
                auto it = find_if(platos.begin(), platos.end(), [id](auto& x){ return x.id==id; });
                if (it != platos.end()) {
                    cout << RED << "Eliminar plato? (s/n): " << RST;
                    string r; getline(cin,r);
                    if (r=="s") { platos.erase(it); ok("Eliminado"); }
                } else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 2 — PROVEEDORES Y PEDIDOS A PROVEEDOR
// ──────────────────────────────────────────────────────
void prov_listar(bool activos = true) {
    vector<Proveedor*> rows;
    for (auto& p : proveedores) if (!activos || p.activo) rows.push_back(&p);
    if (rows.empty()) { warn("Sin proveedores."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(28)<<"Nombre"
         << setw(20)<<"Contacto" << setw(15)<<"Telefono" << "Estado\n";
    cout << string(76,'-') << "\n";
    for (auto* p : rows) {
        string st = p->activo ? GRN+"Activo"+RST : RED+"Inactivo"+RST;
        cout << left << setw(5)<<p->id << setw(28)<<p->nombre.substr(0,27)
             << setw(20)<<p->contacto.substr(0,19) << setw(15)<<p->telefono << st << "\n";
    }
}

void prov_crear() {
    titulo("Nuevo Proveedor");
    Proveedor p; p.id = ++id_prov; p.activo = true; p.creado = hoy();
    p.nombre       = readline("Empresa: ");
    p.contacto     = readline("Contacto: ");
    p.telefono     = readline("Telefono: ");
    p.email        = readline("Email: ");
    p.productos_sum= readline("Productos que suministra: ");
    proveedores.push_back(p);
    ok("Proveedor '" + p.nombre + "' ID#" + to_string(p.id));
}

void compra_crear() {
    titulo("Nuevo Pedido a Proveedor");
    prov_listar();
    if (proveedores.empty()) return;
    Proveedor* sv = bid(proveedores, pint("ID proveedor: ",1));
    if (!sv) { err("No encontrado"); return; }
    Compra c; c.id = ++id_comp; c.proveedor_id = sv->id;
    c.proveedor = sv->nombre; c.fecha = hoy(); c.estado = "pendiente";
    c.total = 0.0; c.notas = readline("Notas: ");
    if (ingredientes.empty()) { warn("No hay ingredientes registrados."); id_comp--; return; }
    while (true) {
        ing_listar();
        string ans = readline("\nAnadir ingrediente? (s/n): ");
        if (ans != "s") break;
        Ingrediente* ing = bid(ingredientes, pint("ID ingrediente: ",1));
        if (!ing || !ing->activo) { err("Ingrediente no encontrado o inactivo"); continue; }
        ItemCompra it; it.prod_id = ing->id; it.nombre = ing->nombre;
        it.cant = pfloat("Cantidad (" + ing->unidad + "): ", 0.01);
        it.pu   = ing->precio;
        it.sub  = it.cant * it.pu;
        c.items.push_back(it); c.total += it.sub;
        cout << GRN << "  Subtotal: " << pesos(it.sub) << RST << "\n";
        cout << YEL << "  Total acumulado: " << pesos(c.total) << RST << "\n";
    }
    if (c.items.empty()) { warn("Pedido vacio, cancelado."); id_comp--; return; }
    cout << "\n" << BLD << GRN << "  Total general: " << pesos(c.total) << RST << "\n";
    compras.push_back(c);
    ok("Pedido #" + to_string(c.id) + " creado — " + pesos(c.total));
}

void compra_listar() {
    if (compras.empty()) { warn("Sin pedidos."); return; }
    for (auto& c : compras) {
        string cc = (c.estado=="pendiente")?YEL:(c.estado=="recibido"?GRN:RED);
        cout << "\n" << BLD << "Pedido #" << c.id << RST
             << " | " << c.proveedor << " | " << c.fecha
             << " | " << cc << c.estado << RST << " | " << BLD << pesos(c.total) << RST << "\n";
        if (!c.notas.empty()) cout << "  Notas: " << c.notas << "\n";
        cout << "  " << left << setw(24)<<"Ingrediente" << setw(10)<<"Unidad"
             << right << setw(10)<<"Cantidad" << setw(14)<<"Precio/u" << setw(14)<<"Subtotal\n";
        cout << "  " << string(72,'-') << "\n";
        for (auto& it : c.items) {
            Ingrediente* ing = bid(ingredientes, it.prod_id);
            string unidad = ing ? ing->unidad : "";
            cout << "  " << left << setw(24)<<it.nombre.substr(0,23) << setw(10)<<unidad
                 << right << setw(10)<<fixed<<setprecision(2)<<it.cant
                 << setw(14)<<pesos(it.pu) << setw(14)<<pesos(it.sub) << "\n";
        }
        cout << "  " << string(72,'-') << "\n";
        cout << "  " << right << setw(56) << "TOTAL: " << BLD << pesos(c.total) << RST << "\n";
    }
}

void compra_recibir() {
    bool found = false;
    for (auto& c : compras) if (c.estado=="pendiente") { found=true; break; }
    if (!found) { warn("Sin pedidos pendientes."); return; }
    compra_listar();
    Compra* c = bid(compras, pint("ID pedido: ",1));
    if (!c) { err("No encontrado"); return; }
    if (c->estado != "pendiente") { warn("Pedido ya no esta pendiente."); return; }
    c->estado = "recibido"; c->fecha_recepcion = hoy();
    for (auto& it : c->items) {
        Ingrediente* ing = bid(ingredientes, it.prod_id);
        if (ing) {
            ing->stock += it.cant;
            cout << GRN << "  " << ing->nombre << ": +" << fixed<<setprecision(2)<<it.cant
                 << " " << ing->unidad << " -> " << ing->stock << RST << "\n";
        }
    }
    ok("Pedido #" + to_string(c->id) + " recibido y stock actualizado");
}

void compra_anular() {
    bool found = false;
    for (auto& c : compras) if (c.estado=="pendiente") { found=true; break; }
    if (!found) { warn("No hay pedidos pendientes para anular."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(28)<<"Proveedor"
         << setw(12)<<"Fecha" << right << setw(14)<<"Total" << "\n"
         << string(62,'-') << "\n";
    for (auto& c : compras)
        if (c.estado=="pendiente")
            cout << left << setw(5)<<c.id << setw(28)<<c.proveedor
                 << setw(12)<<c.fecha << right << setw(14)<<pesos(c.total) << "\n";
    Compra* c = bid(compras, pint("\nID pedido a anular: ",1));
    if (!c) { err("No encontrado"); return; }
    if (c->estado != "pendiente") {
        warn("El pedido ya esta en estado '"+c->estado+"' y no puede anularse."); return;
    }
    cout << RED << "Anular pedido #" << c->id << " a " << c->proveedor << "? (s/n): " << RST;
    string resp; getline(cin, resp);
    if (resp=="s" || resp=="S") {
        c->estado = "cancelado";
        ok("Pedido #"+to_string(c->id)+" anulado correctamente.");
    }
}

void menu_proveedores() {
    while (true) {
        titulo("Proveedores y Pedidos");
        string o = mmenu({{"1","Nuevo proveedor"},{"2","Listar proveedores"},
                          {"3","Modificar proveedor"},{"4","Desactivar / Activar proveedor"},
                          {"5","Nuevo pedido a proveedor"},{"6","Listar pedidos"},
                          {"7","Recibir pedido (actualiza stock)"},{"8","Anular pedido pendiente"},
                          {"0","Volver"}});
        if (o=="1") prov_crear();
        else if (o=="2") { prov_listar(false); pausar(); }
        else if (o=="3") {
            prov_listar(false);
            if (!proveedores.empty()) {
                Proveedor* p = bid(proveedores, pint("ID: ",1));
                if (p) {
                    string oo = mmenu({{"1","Nombre"},{"2","Contacto"},{"3","Telefono"},{"4","Email"},{"0","Cancelar"}});
                    if      (oo=="1") p->nombre   = readline("Nuevo: ");
                    else if (oo=="2") p->contacto = readline("Nuevo: ");
                    else if (oo=="3") p->telefono = readline("Nuevo: ");
                    else if (oo=="4") p->email    = readline("Nuevo: ");
                    if (oo!="0") ok("Proveedor actualizado");
                } else err("No encontrado");
            }
        }
        else if (o=="4") {
            prov_listar(false);
            if (!proveedores.empty()) {
                Proveedor* p = bid(proveedores, pint("ID: ",1));
                if (p) { p->activo = !p->activo; ok(p->activo?"Activado":"Desactivado"); }
                else err("No encontrado");
            }
        }
        else if (o=="5") compra_crear();
        else if (o=="6") { compra_listar(); pausar(); }
        else if (o=="7") compra_recibir();
        else if (o=="8") compra_anular();
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 3 — MESAS
// ──────────────────────────────────────────────────────
const vector<string> EST_MESA = {"disponible","ocupada","reservada","mantenimiento"};

void mesa_listar(bool activas = true) {
    vector<Mesa*> rows;
    for (auto& m : mesas) if (!activas || m.activo) rows.push_back(&m);
    if (rows.empty()) { warn("Sin mesas."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(5)<<"N"
         << setw(6)<<"Cap" << setw(18)<<"Ubicacion" << setw(16)<<"Estado" << "Activo\n";
    cout << string(58,'-') << "\n";
    for (auto* m : rows) {
        map<string,string> colores = {{"disponible",GRN},{"ocupada",RED},{"reservada",YEL},{"mantenimiento",GRY}};
        string cc = colores.count(m->estado) ? colores[m->estado] : WHT;
        cout << left << setw(5)<<m->id << setw(5)<<m->numero << setw(6)<<m->capacidad
             << setw(18)<<m->ubicacion.substr(0,17) << cc << left << setw(16)<<m->estado << RST
             << (m->activo?"Si":"No") << "\n";
    }
}

void mesa_crear() {
    titulo("Nueva Mesa");
    Mesa m; m.id = ++id_mesa; m.activo = true; m.estado = "disponible";
    m.numero    = pint("Numero de mesa: ", 1);
    m.capacidad = pint("Capacidad (personas): ", 1);
    m.ubicacion = readline("Ubicacion (Salon/Terraza/etc): ");
    mesas.push_back(m);
    ok("Mesa #" + to_string(m.numero) + " ID#" + to_string(m.id));
}

void menu_mesas() {
    while (true) {
        titulo("Gestion de Mesas");
        string o = mmenu({{"1","Nueva mesa"},{"2","Listar mesas"},
                          {"3","Cambiar estado"},{"4","Modificar mesa"},
                          {"5","Desactivar / Activar"},{"6","Eliminar mesa"},
                          {"0","Volver"}});
        if (o=="1") mesa_crear();
        else if (o=="2") { mesa_listar(false); pausar(); }
        else if (o=="3") {
            mesa_listar();
            if (!mesas.empty()) {
                Mesa* m = bid(mesas, pint("ID: ",1));
                if (m) {
                    for (size_t i=0; i<EST_MESA.size(); i++)
                        cout << "  " << CYN << i+1 << "." << RST << " " << EST_MESA[i] << "\n";
                    cout << "\n";
                    m->estado = EST_MESA[pint("Estado #: ",1,(int)EST_MESA.size())-1];
                    ok("Mesa #" + to_string(m->numero) + " -> " + m->estado);
                } else err("No encontrado");
            }
        }
        else if (o=="4") {
            mesa_listar(false);
            if (!mesas.empty()) {
                Mesa* m = bid(mesas, pint("ID: ",1));
                if (m) {
                    string oo = mmenu({{"1","Numero"},{"2","Capacidad"},{"3","Ubicacion"},{"0","Cancelar"}});
                    if      (oo=="1") m->numero    = pint("Nuevo #: ",1);
                    else if (oo=="2") m->capacidad = pint("Nueva cap: ",1);
                    else if (oo=="3") m->ubicacion = readline("Nueva ubicacion: ");
                    if (oo!="0") ok("Mesa actualizada");
                } else err("No encontrado");
            }
        }
        else if (o=="5") {
            mesa_listar(false);
            if (!mesas.empty()) {
                Mesa* m = bid(mesas, pint("ID: ",1));
                if (m) { m->activo = !m->activo; ok(m->activo?"Activada":"Desactivada"); }
                else err("No encontrado");
            }
        }
        else if (o=="6") {
            mesa_listar(false);
            if (!mesas.empty()) {
                int id = pint("ID: ",1);
                auto it = find_if(mesas.begin(), mesas.end(), [id](auto& x){ return x.id==id; });
                if (it != mesas.end()) {
                    cout << RED << "Eliminar mesa #" << it->numero << "? Escriba 'si': " << RST;
                    if (readline()=="si") { mesas.erase(it); ok("Mesa eliminada"); }
                    else warn("Cancelado");
                } else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 4 — CLIENTES
// ──────────────────────────────────────────────────────
void cli_nivel(Cliente& c) {
    c.nivel = (c.gastado>=5000000)?"VIP":(c.gastado>=2000000)?"Oro":(c.gastado>=500000)?"Plata":"Bronce";
}

void cli_listar(bool activos = true) {
    vector<Cliente*> rows;
    for (auto& c : clientes) if (!activos || c.activo) rows.push_back(&c);
    if (rows.empty()) { warn("Sin clientes."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(26)<<"Nombre" << setw(14)<<"Telefono"
         << right << setw(8)<<"Visitas" << "   " << setw(14)<<"Total Gastado"
         << "   " << left << setw(10)<<"Nivel" << "Estado\n";
    cout << string(88,'-') << "\n";
    for (auto* c : rows) {
        map<string,string> nc = {{"VIP",MAG},{"Oro",YEL},{"Plata",CYN},{"Bronce",WHT}};
        string cc = nc.count(c->nivel) ? nc[c->nivel] : WHT;
        string st = c->activo ? GRN+"Activo"+RST : RED+"Inactivo"+RST;
        cout << left << setw(5)<<c->id << setw(26)<<c->nombre.substr(0,25)
             << setw(14)<<c->telefono << right << setw(8)<<c->visitas
             << "   " << setw(14)<<pesos(c->gastado) << "   "
             << cc << left << setw(10)<<c->nivel << RST << st << "\n";
    }
}

void cli_crear() {
    titulo("Nuevo Cliente");
    Cliente c; c.id=++id_cli; c.visitas=0; c.gastado=0.0; c.puntos=0;
    c.nivel="Bronce"; c.activo=true; c.creado=hoy();
    c.nombre    = readline("Nombre: ");
    c.telefono  = readline("Telefono: ");
    c.email     = readline("Email: ");
    c.direccion = readline("Direccion: ");
    clientes.push_back(c);
    ok("Cliente '" + c.nombre + "' ID#" + to_string(c.id));
}

void menu_clientes() {
    while (true) {
        titulo("Gestion de Clientes");
        string o = mmenu({{"1","Nuevo cliente"},{"2","Listar clientes"},
                          {"3","Buscar por telefono"},{"4","Modificar cliente"},
                          {"5","Desactivar / Activar cliente"},{"0","Volver"}});
        if (o=="1") cli_crear();
        else if (o=="2") { cli_listar(false); pausar(); }
        else if (o=="3") {
            string tel = readline("Telefono: ");
            bool f = false;
            for (auto& c : clientes) if (c.telefono==tel) {
                cout << "\n  " << CYN << "#" << c.id << RST << " " << c.nombre
                     << " | " << c.nivel << " | visitas:" << c.visitas
                     << " | gastado:" << pesos(c.gastado) << "\n";
                f = true;
            }
            if (!f) warn("No encontrado");
            pausar();
        }
        else if (o=="4") {
            cli_listar(false);
            if (!clientes.empty()) {
                Cliente* c = bid(clientes, pint("ID: ",1));
                if (c) {
                    string oo = mmenu({{"1","Nombre"},{"2","Telefono"},{"3","Email"},{"4","Direccion"},{"0","Cancelar"}});
                    if      (oo=="1") c->nombre    = readline("Nuevo: ");
                    else if (oo=="2") c->telefono  = readline("Nuevo: ");
                    else if (oo=="3") c->email     = readline("Nuevo: ");
                    else if (oo=="4") c->direccion = readline("Nuevo: ");
                    if (oo!="0") ok("Cliente actualizado");
                } else err("No encontrado");
            }
        }
        else if (o=="5") {
            cli_listar(false);
            if (!clientes.empty()) {
                Cliente* c = bid(clientes, pint("ID: ",1));
                if (c) { c->activo = !c->activo; ok(c->activo?"Activado":"Desactivado"); }
                else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 5 — RESERVAS
// ──────────────────────────────────────────────────────
const vector<string> EST_RES = {"pendiente","confirmada","cancelada","en_mesa"};

void res_listar() {
    if (reservas.empty()) { warn("Sin reservas."); return; }
    map<string,string> col = {{"pendiente",YEL},{"confirmada",GRN},{"cancelada",RED},{"en_mesa",CYN}};
    cout << "\n" << left << setw(5)<<"ID" << setw(22)<<"Cliente"
         << right << setw(5)<<"Mesa" << "  " << left << setw(13)<<"Fecha"
         << setw(8)<<"Hora" << right << setw(9)<<"Personas" << "  Estado\n";
    cout << string(76,'-') << "\n";
    for (auto& r : reservas) {
        string cc = col.count(r.estado) ? col[r.estado] : WHT;
        cout << left << setw(5)<<r.id << setw(22)<<r.cliente.substr(0,21)
             << right << setw(5)<<r.mesa_num << "  "
             << left << setw(13)<<r.fecha << setw(8)<<r.hora
             << right << setw(9)<<r.personas << "  " << cc << r.estado << RST << "\n";
    }
}

void res_crear() {
    titulo("Nueva Reserva");
    vector<Cliente*> clis_activos;
    for (auto& c : clientes) if (c.activo) clis_activos.push_back(&c);
    if (clis_activos.empty()) {
        warn("No hay clientes activos.");
        if (readline("Registrar cliente ahora? (s/n): ")=="s") cli_crear();
        else return;
        clis_activos.clear();
        for (auto& c : clientes) if (c.activo) clis_activos.push_back(&c);
        if (clis_activos.empty()) return;
    }
    cli_listar();
    Cliente* cl = bid(clientes, pint("ID cliente: ",1));
    if (!cl) { err("No encontrado"); return; }
    mesa_listar();
    if (mesas.empty()) return;
    Mesa* m = bid(mesas, pint("ID mesa: ",1));
    if (!m) { err("No encontrado"); return; }
    Reserva r; r.id=++id_res; r.cliente_id=cl->id; r.cliente=cl->nombre;
    r.mesa_id=m->id; r.mesa_num=m->numero; r.estado="pendiente"; r.creado=ahora();
    r.fecha     = pedir_fecha_reserva("Fecha (DD/MM/AAAA): ");
    r.hora      = readline("Hora (HH:MM): ");
    while (true) {
        int pers = pint("Personas (max " + to_string(m->capacidad) + "): ", 1);
        if (pers > m->capacidad)
            err("La mesa #" + to_string(m->numero) + " tiene capacidad maxima de " + to_string(m->capacidad) + " personas.");
        else { r.personas = pers; break; }
    }
    r.especiales= readline("Solicitudes especiales: ");
    reservas.push_back(r);
    m->estado = "reservada";
    ok("Reserva #" + to_string(r.id) + " — " + cl->nombre + " — " + r.fecha + " " + r.hora);
}

void menu_reservas() {
    while (true) {
        titulo("Gestion de Reservas");
        string o = mmenu({{"1","Nueva reserva"},{"2","Listar reservas"},
                          {"3","Cambiar estado"},{"4","Editar reserva"},
                          {"5","Cancelar reserva"},{"0","Volver"}});
        if (o=="1") res_crear();
        else if (o=="2") { res_listar(); pausar(); }
        else if (o=="3") {
            res_listar();
            if (!reservas.empty()) {
                Reserva* r = bid(reservas, pint("ID: ",1));
                if (r) {
                    sub("Nuevo estado");
                    for (size_t i=0; i<EST_RES.size(); i++)
                        cout << "  " << CYN << i+1 << "." << RST << " " << EST_RES[i] << "\n";
                    cout << "\n";
                    string nuevo = EST_RES[pint("Estado #: ",1,(int)EST_RES.size())-1];
                    r->estado = nuevo;
                    Mesa* m = bid(mesas, r->mesa_id);
                    if (m) {
                        if      (nuevo=="cancelada")  m->estado = "disponible";
                        else if (nuevo=="en_mesa")    m->estado = "ocupada";
                        else if (nuevo=="confirmada") m->estado = "reservada";
                    }
                    ok("Reserva -> " + nuevo);
                } else err("No encontrado");
            }
        }
        else if (o=="4") {
            res_listar();
            if (!reservas.empty()) {
                Reserva* r = bid(reservas, pint("ID: ",1));
                if (r) {
                    sub("Editando Reserva #" + to_string(r->id));
                    string oo = mmenu({{"1","Cliente"},{"2","Mesa"},{"3","Fecha"},
                                       {"4","Hora"},{"5","Personas"},{"6","Solicitudes especiales"},
                                       {"0","Cancelar"}});
                    if (oo=="1") {
                        cli_listar();
                        Cliente* c = bid(clientes, pint("Nuevo ID cliente: ",1));
                        if (c) { r->cliente_id=c->id; r->cliente=c->nombre; ok("Cliente actualizado"); }
                    } else if (oo=="2") {
                        mesa_listar();
                        Mesa* ant = bid(mesas, r->mesa_id);
                        Mesa* nue = bid(mesas, pint("Nuevo ID mesa: ",1));
                        if (nue) {
                            if (ant) ant->estado = "disponible";
                            r->mesa_id=nue->id; r->mesa_num=nue->numero;
                            nue->estado="reservada"; ok("Mesa actualizada");
                        }
                    } else if (oo=="3") { r->fecha = pedir_fecha_reserva("Nueva fecha (DD/MM/AAAA): "); ok("Fecha actualizada"); }
                    else if (oo=="4") { r->hora = readline("Nueva hora (HH:MM): "); ok("Hora actualizada"); }
                    else if (oo=="5") {
                        Mesa* mm = bid(mesas, r->mesa_id);
                        int cap = mm ? mm->capacidad : 999;
                        while (true) {
                            int pers = pint("Nuevo numero de personas (max " + to_string(cap) + "): ", 1);
                            if (pers > cap) err("La mesa #" + to_string(r->mesa_num) + " tiene capacidad maxima de " + to_string(cap) + " personas.");
                            else { r->personas = pers; ok("Personas actualizado"); break; }
                        }
                    }
                    else if (oo=="6") { r->especiales = readline("Nuevas solicitudes: "); ok("Actualizado"); }
                } else err("No encontrado");
            }
        }
        else if (o=="5") {
            res_listar();
            if (!reservas.empty()) {
                Reserva* r = bid(reservas, pint("ID: ",1));
                if (r) {
                    cout << RED << "Cancelar reserva de " << r->cliente << "? (s/n): " << RST;
                    if (readline()=="s") {
                        r->estado = "cancelada";
                        Mesa* m = bid(mesas, r->mesa_id);
                        if (m) m->estado = "disponible";
                        ok("Reserva cancelada");
                    } else warn("Cancelado");
                } else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 6 — PEDIDOS (local y domicilio)
// ──────────────────────────────────────────────────────
const vector<string> EST_PED = {"abierto","en_preparacion","listo","entregado","cancelado"};

void ped_listar() {
    if (pedidos.empty()) { warn("Sin pedidos."); return; }
    map<string,string> col = {{"abierto",CYN},{"en_preparacion",YEL},
                              {"listo",GRN},{"entregado",GRY},{"cancelado",RED}};
    cout << "\n" << left << setw(5)<<"ID" << setw(10)<<"Tipo"
         << setw(22)<<"Cliente" << setw(26)<<"Referencia"
         << right << setw(12)<<"Total" << "  " << left << setw(16)<<"Estado" << "Fecha\n";
    cout << string(106,'-') << "\n";
    for (auto& p : pedidos) {
        string cc  = col.count(p.estado) ? col[p.estado] : WHT;
        string ref = (p.tipo=="local") ? "Mesa " + to_string(p.mesa_num) : p.direccion.substr(0,24);
        cout << left << setw(5)<<p.id << setw(10)<<p.tipo << setw(22)<<p.cliente.substr(0,21)
             << setw(26)<<ref << right << setw(12)<<pesos(p.total) << "  "
             << cc << left << setw(16)<<p.estado << RST << p.fecha << "\n";
    }
}

void ped_detalle(Pedido& p) {
    cout << "\n" << BLD << string(54,'-') << RST << "\n";
    cout << "  " << CYN << "Pedido #" << p.id << RST << " | " << p.tipo << " | " << p.estado << "\n";
    cout << "  Cliente : " << p.cliente << "\n";
    cout << "  Fecha   : " << p.fecha << "\n";
    if (p.tipo=="local") cout << "  Mesa    : " << p.mesa_num << "\n";
    else                 cout << "  Direcc  : " << p.direccion << "\n";
    if (!p.notas.empty()) cout << "  Notas   : " << p.notas << "\n";
    cout << "\n  " << left << setw(26)<<"Plato"
         << right << setw(5)<<"Cant" << setw(12)<<"Precio" << setw(12)<<"Subtotal\n";
    cout << "  " << string(56,'-') << "\n";
    for (auto& it : p.items) {
        cout << "  " << left << setw(26)<<it.nombre.substr(0,25)
             << right << setw(5)<<it.cant << setw(12)<<pesos(it.pu) << setw(12)<<pesos(it.sub) << "\n";
        if (!it.nota.empty()) cout << "    " << GRY << "-> " << it.nota << RST << "\n";
    }
    cout << "  " << string(56,'-') << "\n";
    cout << "  " << BLD << right << setw(43)<<"TOTAL" << setw(12) << pesos(p.total) << RST << "\n";
    cout << BLD << string(54,'-') << RST << "\n";
}

void ped_crear() {
    titulo("Nuevo Pedido");
    cout << "  " << CYN << "1." << RST << " Local (mesa)\n";
    cout << "  " << CYN << "2." << RST << " Domicilio\n\n";
    string tipo = (popc({"1","2"})=="1") ? "local" : "domicilio";
    Pedido p; p.id=++id_ped; p.tipo=tipo; p.fecha=ahora();
    p.total=0.0; p.estado="abierto"; p.cliente_id=0; p.mesa_id=0; p.mesa_num=0;
    p.cliente="Anonimo";
    p.notas = readline("Notas: ");

    // Asociar cliente
    if (readline("Asociar cliente? (s/n): ")=="s") {
        vector<Cliente*> cact;
        for (auto& c : clientes) if (c.activo) cact.push_back(&c);
        if (cact.empty()) { warn("No hay clientes activos registrados."); }
        else {
            cli_listar();
            Cliente* cl = bid(clientes, pint("ID cliente: ",1));
            if (cl && cl->activo) { p.cliente_id=cl->id; p.cliente=cl->nombre; }
            else warn("Cliente no encontrado o inactivo");
        }
    }

    if (tipo=="local") {
        mesa_listar();
        Mesa* m = bid(mesas, pint("ID mesa: ",1));
        if (m) {
            if (m->estado == "reservada") {
                warn("La mesa #" + to_string(m->numero) + " esta reservada. No se puede asignar a un pedido directo.");
                return;
            }
            p.mesa_id=m->id; p.mesa_num=m->numero; m->estado="ocupada";
        }
    } else {
        p.direccion = readline("Direccion domicilio: ");
    }

    vector<Plato*> platos_act;
    for (auto& d : platos) if (d.activo) platos_act.push_back(&d);
    if (platos_act.empty()) { warn("Sin platos activos."); return; }

    while (true) {
        plato_listar();
        if (readline("\nAgregar plato al pedido? (s/n): ")!="s") break;
        Plato* d = bid(platos, pint("ID plato: ",1));
        if (!d || !d->activo) { err("No disponible"); continue; }
        if (d->stock <= 0) { warn("Sin stock de " + d->nombre); continue; }
        int cant = pint("Cantidad (max " + to_string(d->stock) + "): ", 1, d->stock);
        string nota = readline("Nota del item: ");
        double sub_t = cant * d->precio_venta;
        p.items.push_back({d->id, cant, d->nombre, nota, d->precio_venta, sub_t});
        p.total += sub_t; d->stock -= cant;
        ok("  " + to_string(cant) + "x " + d->nombre + " = " + pesos(sub_t));
    }

    if (p.items.empty()) { warn("Pedido vacio."); return; }
    pedidos.push_back(p);
    if (p.cliente_id > 0) {
        Cliente* cl = bid(clientes, p.cliente_id);
        if (cl) {
            cl->visitas++; cl->gastado+=p.total; cl->puntos+=(int)(p.total/1000);
            cli_nivel(*cl);
        }
    }
    cout << "\n" << BLD << GRN << "Pedido #" << p.id << " — Total: " << pesos(p.total) << RST << "\n";
}

void menu_pedidos() {
    while (true) {
        titulo("Gestion de Pedidos");
        string o = mmenu({{"1","Nuevo pedido"},{"2","Listar pedidos"},
                          {"3","Ver detalle"},{"4","Cambiar estado"},
                          {"5","Cancelar pedido"},{"0","Volver"}});
        if (o=="1") ped_crear();
        else if (o=="2") { ped_listar(); pausar(); }
        else if (o=="3") {
            ped_listar();
            if (!pedidos.empty()) {
                Pedido* p = bid(pedidos, pint("ID: ",1));
                if (p) { ped_detalle(*p); pausar(); } else err("No encontrado");
            }
        }
        else if (o=="4") {
            ped_listar();
            if (!pedidos.empty()) {
                Pedido* p = bid(pedidos, pint("ID: ",1));
                if (p) {
                    sub("Nuevo estado");
                    for (size_t i=0; i<EST_PED.size(); i++)
                        cout << "  " << CYN << i+1 << "." << RST << " " << EST_PED[i] << "\n";
                    cout << "\n";
                    string nuevo = EST_PED[pint("Estado #: ",1,(int)EST_PED.size())-1];
                    p->estado = nuevo;
                    if (nuevo=="entregado" && p->mesa_id>0) {
                        Mesa* m = bid(mesas, p->mesa_id);
                        if (m) m->estado = "disponible";
                    }
                    ok("Pedido #" + to_string(p->id) + " -> " + nuevo);
                } else err("No encontrado");
            }
        }
        else if (o=="5") {
            ped_listar();
            if (!pedidos.empty()) {
                Pedido* p = bid(pedidos, pint("ID: ",1));
                if (p) {
                    cout << RED << "Cancelar pedido? (s/n): " << RST;
                    if (readline()=="s") {
                        p->estado = "cancelado";
                        for (auto& it : p->items) {
                            Plato* pl = bid(platos, it.prod_id);
                            if (pl) pl->stock += it.cant;
                        }
                        if (p->mesa_id>0) {
                            Mesa* m = bid(mesas, p->mesa_id);
                            if (m) m->estado = "disponible";
                        }
                        ok("Pedido cancelado — stock devuelto");
                    } else warn("Cancelado");
                } else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 7 — EMPLEADOS
// ──────────────────────────────────────────────────────
const vector<string> ROLES = {"Mesero","Cocinero","Cajero","Administrador","Domiciliario","Auxiliar","Otro"};

void emp_listar(bool activos = true) {
    vector<Empleado*> rows;
    for (auto& e : empleados) if (!activos || e.activo) rows.push_back(&e);
    if (rows.empty()) { warn("Sin empleados."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(26)<<"Nombre" << setw(16)<<"Rol"
         << right << setw(14)<<"Salario Base" << "   " << left << setw(24)<<"Tipo Contrato" << "Estado\n";
    cout << string(94,'-') << "\n";
    for (auto* e : rows) {
        string st = e->activo ? GRN+"Activo"+RST : RED+"Inactivo"+RST;
        cout << left << setw(5)<<e->id << setw(26)<<e->nombre.substr(0,25)
             << setw(16)<<e->rol << right << setw(14)<<pesos(e->salario)
             << "   " << left << setw(24)<<e->contrato << st << "\n";
    }
}

void emp_crear() {
    titulo("Nuevo Empleado");
    Empleado e; e.id=++id_emp; e.activo=true;
    e.nombre    = readline("Nombre: ");
    e.documento = readline("Documento: ");
    e.telefono  = readline("Telefono: ");
    e.email     = readline("Email: ");
    e.ingreso = pedir_fecha_col("Fecha ingreso (DD/MM/AAAA): ");
    cout << "Roles:\n";
    for (size_t i=0; i<ROLES.size(); i++) cout << "  " << CYN << i+1 << "." << RST << " " << ROLES[i] << "\n";
    cout << "\n";
    e.rol     = ROLES[pint("Rol #: ",1,(int)ROLES.size())-1];
    e.salario = pfloat("Salario base (min SMLV=" + pesos(SMLV) + "): ", SMLV);
    cout << "Tipo contrato:\n";
    cout << "  " << CYN << "1." << RST << " Termino fijo\n";
    cout << "  " << CYN << "2." << RST << " Indefinido\n";
    cout << "  " << CYN << "3." << RST << " Prestacion de servicios\n\n";
    string tc = popc({"1","2","3"});
    map<string,string> maptc = {{"1","Termino fijo"},{"2","Indefinido"},{"3","Prest. servicios"}};
    e.contrato = maptc[tc];
    empleados.push_back(e);
    ok("Empleado '" + e.nombre + "' ID#" + to_string(e.id));
}

void menu_empleados() {
    while (true) {
        titulo("Gestion de Empleados");
        string o = mmenu({{"1","Nuevo empleado"},{"2","Listar empleados"},
                          {"3","Consultar empleado"},{"4","Modificar empleado"},
                          {"5","Desactivar / Activar empleado"},
                          {"0","Volver"}});
        if (o=="1") emp_crear();
        else if (o=="2") { emp_listar(false); pausar(); }
        else if (o=="3") {
            emp_listar();
            if (!empleados.empty()) {
                Empleado* e = bid(empleados, pint("ID: ",1));
                if (e) {
                    cout << "\n" << BLD << string(44,'-') << RST << "\n";
                    cout << "  " << CYN << left << setw(16) << "ID"       << RST << e->id       << "\n";
                    cout << "  " << CYN << left << setw(16) << "Nombre"   << RST << e->nombre   << "\n";
                    cout << "  " << CYN << left << setw(16) << "Documento"<< RST << e->documento<< "\n";
                    cout << "  " << CYN << left << setw(16) << "Telefono" << RST << e->telefono << "\n";
                    cout << "  " << CYN << left << setw(16) << "Email"    << RST << e->email    << "\n";
                    cout << "  " << CYN << left << setw(16) << "Rol"      << RST << e->rol      << "\n";
                    cout << "  " << CYN << left << setw(16) << "Salario"  << RST << pesos(e->salario) << "\n";
                    cout << "  " << CYN << left << setw(16) << "Contrato" << RST << e->contrato << "\n";
                    cout << "  " << CYN << left << setw(16) << "Ingreso"  << RST << e->ingreso  << "\n";
                    cout << "  " << CYN << left << setw(16) << "Estado"   << RST << (e->activo?"Activo":"Inactivo") << "\n";
                    cout << BLD << string(44,'-') << RST << "\n";
                    pausar();
                } else err("No encontrado");
            }
        }
        else if (o=="4") {
            emp_listar(false);
            if (!empleados.empty()) {
                Empleado* e = bid(empleados, pint("ID: ",1));
                if (e) {
                    string oo = mmenu({{"1","Nombre"},{"2","Telefono"},{"3","Rol"},
                                       {"4","Salario"},{"5","Tipo contrato"},{"0","Cancelar"}});
                    if (oo=="1") e->nombre   = readline("Nuevo nombre: ");
                    else if (oo=="2") e->telefono = readline("Nuevo telefono: ");
                    else if (oo=="3") {
                        for (size_t i=0; i<ROLES.size(); i++)
                            cout << "  " << CYN << i+1 << "." << RST << " " << ROLES[i] << "\n";
                        cout << "\n";
                        e->rol = ROLES[pint("Rol #: ",1,(int)ROLES.size())-1];
                    }
                    else if (oo=="4") e->salario = pfloat("Nuevo salario: ", SMLV);
                    else if (oo=="5") {
                        cout << "  " << CYN << "1." << RST << " Termino fijo\n";
                        cout << "  " << CYN << "2." << RST << " Indefinido\n";
                        cout << "  " << CYN << "3." << RST << " Prest. servicios\n\n";
                        string tc = popc({"1","2","3"});
                        map<string,string> mt={{"1","Termino fijo"},{"2","Indefinido"},{"3","Prest. servicios"}};
                        e->contrato = mt[tc];
                    }
                    if (oo!="0") ok("Empleado actualizado");
                } else err("No encontrado");
            }
        }
        else if (o=="5") {
            emp_listar(false);
            if (!empleados.empty()) {
                Empleado* e = bid(empleados, pint("ID: ",1));
                if (e) { e->activo = !e->activo; ok(e->activo?"Activado":"Desactivado"); }
                else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 8 — NOMINA (Normatividad colombiana vigente)
//  Decretos 1469 y 1470 del 29 de diciembre de 2025
//  Ref: mintrabajo.gov.co/mi-calculadora
// ──────────────────────────────────────────────────────
Nomina calc_nomina(const Empleado& e, const string& periodo) {
    Nomina n; n.id=++id_nom; n.emp_id=e.id; n.empleado=e.nombre;
    n.rol=e.rol; n.periodo=periodo; n.generado=hoy();
    double sal = e.salario;
    n.salario_base    = sal;
    n.aux_transporte  = (sal <= 2*SMLV) ? AUX_TRANSP : 0.0;
    n.ded_salud       = sal * 0.04;
    n.ded_pension     = (sal >= SMLV) ? sal * 0.04 : 0.0;
    n.total_ded       = n.ded_salud + n.ded_pension;
    n.salario_neto    = sal - n.total_ded + n.aux_transporte;
    n.apt_salud       = sal * 0.085;
    n.apt_pension     = sal * 0.12;
    n.arl             = sal * 0.00522;
    n.caja            = sal * 0.04;
    n.icbf            = sal * 0.03;
    n.sena            = sal * 0.02;
    n.total_apt       = n.apt_salud + n.apt_pension + n.arl + n.caja + n.icbf + n.sena;
    double base_ces   = sal + n.aux_transporte;
    n.cesantias       = base_ces * 0.0833;
    n.int_ces         = n.cesantias * 0.12;
    n.prima           = base_ces * 0.0833;
    n.vacaciones      = sal * 0.0417;
    n.total_pres      = n.cesantias + n.int_ces + n.prima + n.vacaciones;
    n.costo_total     = sal + n.aux_transporte + n.total_apt + n.total_pres;
    return n;
}

void nom_imprimir(const Nomina& n) {
    int W = 58;
    cout << "\n" << BLD << CYN << string(W,'=') << RST << "\n";
    cout << BLD << CYN << "  COMPROBANTE DE NOMINA — Dec.1469-1470/2025\n" << RST;
    cout << BLD << CYN << "  " << n.empleado << " — " << n.rol << " — " << n.periodo << "\n" << RST;
    cout << BLD << CYN << string(W,'=') << RST << "\n";
    auto fila = [](const string& label, double val, bool neg=false) {
        cout << "  " << left << setw(38) << label << right << setw(14)
             << (neg?"-":"") + pesos(val) << "\n";
    };
    cout << "\n"; fila("Salario Base (SMLV 2026)", n.salario_base);
    if (n.aux_transporte>0) fila("+ Auxilio de transporte", n.aux_transporte);
    cout << "\n" << BLD << "  DEDUCCIONES DEL EMPLEADO\n" << RST;
    fila("  Salud (4%)",    n.ded_salud,   true);
    fila("  Pension (4%)",  n.ded_pension, true);
    fila("  Total deducciones", n.total_ded, true);
    cout << "\n  " << BLD << GRN << left << setw(38)
         << "NETO RECIBIDO (inc. aux. transp.)" << right << setw(14) << pesos(n.salario_neto) << RST << "\n";
    cout << "\n" << BLD << "  APORTES DEL EMPLEADOR (sobre salario base)\n" << RST;
    fila("  Salud (8.5%)",          n.apt_salud);
    fila("  Pension (12%)",         n.apt_pension);
    fila("  ARL (0.522%)",          n.arl);
    fila("  Caja Compensacion (4%)",n.caja);
    fila("  ICBF (3%)",             n.icbf);
    fila("  SENA (2%)",             n.sena);
    cout << "\n" << BLD << "  PRESTACIONES SOCIALES (base: sal + aux)\n" << RST;
    fila("  Cesantias (8.33%)",              n.cesantias);
    fila("  Intereses cesantias (1%/mes)",   n.int_ces);
    fila("  Prima de servicios (8.33%)",     n.prima);
    fila("  Vacaciones (4.17% s/salario)",   n.vacaciones);
    cout << "\n  " << BLD << RED << left << setw(38)
         << "COSTO TOTAL EMPLEADOR" << right << setw(14) << pesos(n.costo_total) << RST << "\n";
    cout << BLD << CYN << string(W,'=') << RST << "\n";
}

void nom_generar() {
    titulo("Generar Nomina");
    emp_listar();
    if (empleados.empty()) return;
    Empleado* e = bid(empleados, pint("ID empleado: ",1));
    if (!e) { err("No encontrado"); return; }
    string per = pedir_periodo_col("Periodo ");
    // Check existing
    auto it = find_if(nominas.begin(), nominas.end(),
        [&](auto& n){ return n.emp_id==e->id && n.periodo==per; });
    if (it != nominas.end()) {
        warn("Ya existe nomina de " + e->nombre + " para " + per);
        if (readline("Recalcular? (s/n): ")!="s") return;
        nominas.erase(it);
    }
    Nomina n = calc_nomina(*e, per);
    nominas.push_back(n);
    nom_imprimir(n);
    ok("Nomina guardada ID#" + to_string(n.id));
}

void nom_listar() {
    if (nominas.empty()) { warn("Sin nominas."); return; }
    cout << "\n" << left << setw(5)<<"ID" << setw(26)<<"Empleado" << setw(9)<<"Periodo"
         << right << setw(14)<<"Sal.Base" << setw(14)<<"Neto" << setw(14)<<"Costo Total\n";
    cout << string(82,'-') << "\n";
    for (auto& n : nominas)
        cout << left << setw(5)<<n.id << setw(26)<<n.empleado.substr(0,25) << setw(9)<<n.periodo
             << right << setw(14)<<pesos(n.salario_base) << setw(14)<<pesos(n.salario_neto)
             << setw(14)<<pesos(n.costo_total) << "\n";
}

void nom_periodo() {
    string per = pedir_periodo_col("Periodo ");
    vector<Nomina*> ns;
    for (auto& n : nominas) if (n.periodo==per) ns.push_back(&n);
    if (ns.empty()) { warn("Sin nominas para " + per); return; }
    cout << "\n" << BLD << "Nomina periodo " << CYN << per << RST << "\n\n";
    double tot_neto=0, tot_costo=0;
    for (auto* n : ns) {
        cout << "  " << left << setw(26)<<n->empleado << setw(16)<<n->rol
             << "Neto: " << pesos(n->salario_neto) << "\n";
        tot_neto += n->salario_neto; tot_costo += n->costo_total;
    }
    cout << "\n  " << BLD << "Total neto    : " << GRN << pesos(tot_neto) << RST << "\n";
    cout << "  " << BLD << "Costo empleador: " << RED << pesos(tot_costo) << RST << "\n";
}

void menu_nomina() {
    while (true) {
        titulo("Gestion de Nomina");
        string o = mmenu({{"1","Generar nomina de empleado"},{"2","Consultar nomina por periodo"},
                          {"3","Historial de nominas"},{"4","Eliminar nomina"},
                          {"0","Volver"}});
        if (o=="1") nom_generar();
        else if (o=="2") { nom_periodo(); pausar(); }
        else if (o=="3") { nom_listar(); pausar(); }
        else if (o=="4") {
            nom_listar();
            if (!nominas.empty()) {
                int id = pint("ID: ",1);
                auto it = find_if(nominas.begin(), nominas.end(), [id](auto& x){ return x.id==id; });
                if (it != nominas.end()) {
                    cout << RED << "Eliminar nomina? Escriba 'si': " << RST;
                    if (readline()=="si") { nominas.erase(it); ok("Eliminado"); }
                    else warn("Cancelado");
                } else err("No encontrado");
            }
        }
        else break;
        guardar();
    }
}

// ──────────────────────────────────────────────────────
//  MODULO 9 — DASHBOARD / ESTADISTICAS
// ──────────────────────────────────────────────────────
struct Stats { int n; double suma,prom,mx,mn,med,std_dev; };

Stats calcStats(vector<double>& vals) {
    Stats s{}; s.n=(int)vals.size();
    if (vals.empty()) return s;
    sort(vals.begin(), vals.end());
    s.suma = 0; for (auto v:vals) s.suma+=v;
    s.prom = s.suma/s.n; s.mx=vals.back(); s.mn=vals.front();
    s.med  = (s.n%2==1) ? vals[s.n/2] : (vals[s.n/2-1]+vals[s.n/2])/2.0;
    double var=0; for (auto v:vals) var+=(v-s.prom)*(v-s.prom);
    s.std_dev = sqrt(var/s.n);
    return s;
}

void dashboard() {
    titulo("Dashboard — Analisis Estadistico");
    cout << "Periodo:";
    string o = mmenu({{"1","Hoy"},{"2","Ultima semana"},{"3","Mes actual"},
                      {"4","Ano actual"},{"5","Historico completo"}});
    // Get current date info
    time_t now = time(nullptr); tm* t = localtime(&now);
    char buf_hoy[11]; strftime(buf_hoy,sizeof(buf_hoy),"%Y-%m-%d",t);
    string shoy = string(buf_hoy);
    string desde;
    string label;
    if (o=="1") { desde=shoy; label="Hoy ("+shoy+")"; }
    else if (o=="2") {
        time_t sem=now-7*86400; tm* ts=localtime(&sem);
        char b[11]; strftime(b,sizeof(b),"%Y-%m-%d",ts);
        desde=string(b); label="Ultima semana";
    }
    else if (o=="3") {
        char b[8]; strftime(b,sizeof(b),"%Y-%m",t);
        desde=string(b)+"-01"; label="Mes actual";
    }
    else if (o=="4") {
        char b[5]; strftime(b,sizeof(b),"%Y",t);
        desde=string(b)+"-01-01"; label="Ano "+string(b);
    }
    else { desde="2000-01-01"; label="Historico completo"; }

    auto en_per = [&](const string& fecha) {
        if (o=="5") return true;
        return fecha.substr(0,10) >= desde;
    };

    vector<Pedido*> peds;
    for (auto& p : pedidos) if (en_per(p.fecha)) peds.push_back(&p);

    vector<double> totales;
    for (auto* p : peds) if (p->estado!="cancelado") totales.push_back(p->total);
    Stats st = calcStats(totales);

    cout << "\n" << BLD << CYN << "  PERIODO: " << label << RST << "\n\n";

    sub("Resumen de Pedidos");
    auto row=[](const string& label, const string& val) {
        cout << "  " << left << setw(34) << label << val << "\n";
    };
    row("Total pedidos:",         to_string(st.n));
    row("Ingresos totales:",      GRN+pesos(st.suma)+RST);
    row("Ticket promedio:",       pesos(st.prom));
    row("Desv. estandar:",        pesos(st.std_dev));
    row("Ticket maximo:",         pesos(st.mx));
    row("Ticket minimo:",         pesos(st.mn));
    row("Mediana:",               pesos(st.med));

    int loc=0, dom=0, canc=0, abiertos=0;
    for (auto* p : peds) {
        if (p->estado=="cancelado") canc++;
        else if (p->estado=="abierto") { abiertos++; if(p->tipo=="local") loc++; else dom++; }
        else if (p->tipo=="local")  loc++;
        else dom++;
    }
    row("Pedidos locales:", to_string(loc));
    row("Pedidos domicilio:", to_string(dom));
    row("Cancelados:", RED+to_string(canc)+RST);
    row("Abiertos (en curso):", CYN+to_string(abiertos)+RST);

    // Top 5 platos
    sub("Top 5 Platos mas Vendidos");
    map<string,pair<int,double>> cnt;
    for (auto* p : peds) if (p->estado!="cancelado")
        for (auto& it : p->items) { cnt[it.nombre].first+=it.cant; cnt[it.nombre].second+=it.sub; }
    if (!cnt.empty()) {
        vector<pair<string,pair<int,double>>> sorted_cnt(cnt.begin(), cnt.end());
        sort(sorted_cnt.begin(), sorted_cnt.end(), [](auto& a, auto& b){ return a.second.first>b.second.first; });
        double mx_c = sorted_cnt[0].second.first;
        int top = min((int)sorted_cnt.size(), 5);
        for (int i=0; i<top; i++) {
            auto& [nm, data] = sorted_cnt[i];
            int bars = (int)((data.first/mx_c)*20);
            cout << "  " << left << setw(28)<<nm.substr(0,27) << " "
                 << CYN << string(bars,'#') << GRY << string(20-bars,'-') << RST
                 << right << setw(5)<<data.first << " uds — " << pesos(data.second) << "\n";
        }
    } else cout << "  Sin datos\n";

    // Ingredientes y Platos
    sub("Inventario");
    int iact=0; double ilow=0;
    for (auto& i : ingredientes) if (i.activo) { iact++; if (i.stock < 5.0) ilow++; }
    int dact=0, dlow=0;
    for (auto& d : platos) if (d.activo) { dact++; if (d.stock <= 0) dlow++; }
    row("Ingredientes activos:", to_string(iact));
    row("Ingredientes con stock bajo (<5):", RED+to_string((int)ilow)+RST);
    row("Platos activos:", to_string(dact));
    row("Platos sin stock:", RED+to_string(dlow)+RST);

    // Nomina del Periodo
    sub("Nomina del Periodo");
    {
        vector<Nomina*> ns;
        if (o=="1" || o=="2" || o=="3") {
            // Hoy / Semana / Mes: filtrar por mes actual
            time_t t = time(nullptr); tm* lt = localtime(&t);
            char pfx[6]; strftime(pfx,sizeof(pfx),"%m/%y",lt);
            for (auto& n : nominas) if (n.periodo==string(pfx)) ns.push_back(&n);
        } else if (o=="4") {
            // Anio: mismo anio
            time_t t = time(nullptr); tm* lt = localtime(&t);
            char sfx[4]; strftime(sfx,sizeof(sfx),"/%y",lt);
            for (auto& n : nominas)
                if (n.periodo.size()>=3 && n.periodo.substr(2)==string(sfx)) ns.push_back(&n);
        } else {
            for (auto& n : nominas) ns.push_back(&n);
        }
        if (!ns.empty()) {
            double tot_neto=0, tot_costo=0;
            for (auto* n : ns) { tot_neto+=n->salario_neto; tot_costo+=n->costo_total; }
            row("Empleados con nomina:", to_string((int)ns.size()));
            row("Total neto:", GRN+pesos(tot_neto)+RST);
            row("Costo empleador:", RED+pesos(tot_costo)+RST);
        } else {
            cout << "  " << GRY << "Sin nominas registradas en este periodo." << RST << "\n";
        }
    }

    // Top 5 clientes
    sub("Top 5 Clientes por Gasto");
    vector<Cliente*> top_c;
    for (auto& c : clientes) if (c.activo) top_c.push_back(&c);
    sort(top_c.begin(), top_c.end(), [](auto* a, auto* b){ return a->gastado>b->gastado; });
    map<string,string> nc = {{"VIP",MAG},{"Oro",YEL},{"Plata",CYN},{"Bronce",WHT}};
    if (!top_c.empty()) {
        double mx_g = top_c[0]->gastado;
        int top = min((int)top_c.size(), 5);
        for (int i=0; i<top; i++) {
            auto* c = top_c[i];
            int bars = mx_g>0 ? (int)((c->gastado/mx_g)*15) : 0;
            string cc = nc.count(c->nivel)?nc[c->nivel]:WHT;
            cout << "  " << left << setw(26)<<c->nombre.substr(0,25) << " "
                 << CYN << string(bars,'#') << GRY << string(15-bars,'-') << RST
                 << " " << right << setw(14)<<pesos(c->gastado) << " " << cc << c->nivel << RST << "\n";
        }
    } else cout << "  Sin datos\n";
}

// ──────────────────────────────────────────────────────
//  MODULO 10 — SIMULACION
// ──────────────────────────────────────────────────────
void simular() {
    titulo("Simulacion del Restaurante");
    int dias = pint("Cuantos dias simular? (1-30): ", 1, 30);
    cout << "\n" << CYN << "Generando datos base..." << RST << "\n";

    mt19937 rng((unsigned int)time(nullptr));
    auto ri = [&](int a, int b){ return uniform_int_distribution<int>(a,b)(rng); };
    auto rd = [&](double a, double b){ return uniform_real_distribution<double>(a,b)(rng); };

    // Ingredientes — agrega los que no existan por nombre
    vector<tuple<string,string,string,double>> ings_sim = {
        {"Arroz","granos","kg",3500},
        {"Pollo","carnes","kg",12000},
        {"Papa","verduras","kg",2800},
        {"Frijol","granos","kg",8000},
        {"Leche","lacteos","litro",3200},
        {"Tomate","verduras","kg",3000},
        {"Cebolla","verduras","kg",2500},
        {"Aceite","aceites","litro",8500},
        {"Limon","frutas","unidad",500},
        {"Sal","condimentos","gramos",800},
        {"Azucar","granos","kg",3000},
        {"Platano","frutas","unidad",600},
        {"Carne de Res","carnes","kg",22000},
        {"Queso","lacteos","kg",18000},
        {"Harina","granos","kg",2500}
    };
    { set<string> nombres_ing;
      for (auto& i : ingredientes) nombres_ing.insert(i.nombre);
      int nuevos_ing = 0;
      for (auto& [nm,cat,und,pr] : ings_sim) {
          if (!nombres_ing.count(nm)) {
              Ingrediente i; i.id=++id_ing; i.nombre=nm; i.categoria=cat;
              i.unidad=und; i.precio=pr; i.stock=rd(50,200);
              i.activo=true; i.creado=hoy();
              ingredientes.push_back(i); nuevos_ing++;
          }
      }
      if (nuevos_ing) ok("  " + to_string(nuevos_ing) + " ingredientes agregados");
    }
    // Platos — agrega los que no existan por nombre
    auto ing_by_name = [&](const string& nm) -> Ingrediente* {
        for (auto& i : ingredientes) if (i.nombre == nm) return &i;
        return nullptr;
    };
    vector<tuple<string,double,vector<string>>> platos_sim = {
        {"Bandeja Paisa",    32000, {"Arroz","Pollo","Frijol","Sal"}},
        {"Ajiaco Bogotano",  28000, {"Arroz","Pollo","Papa","Cebolla","Sal"}},
        {"Sancocho de Gallina",25000,{"Pollo","Papa","Cebolla","Aceite","Sal"}},
        {"Empanadas x3",      8000, {"Harina","Papa","Carne de Res","Sal"}},
        {"Patacones con Hogao",12000,{"Platano","Tomate","Cebolla","Aceite"}},
        {"Arroz con Pollo",  22000, {"Arroz","Pollo","Cebolla","Aceite","Sal"}},
        {"Churrasco",        38000, {"Carne de Res","Aceite","Sal"}},
        {"Ensalada Cesar",   15000, {"Tomate","Cebolla","Limon","Sal"}},
        {"Flan de Caramelo",  9000, {"Leche","Azucar","Huevo"}},
        {"Tres Leches",      10000, {"Leche","Azucar","Harina"}},
        {"Limonada Natural",  7000, {"Limon","Azucar"}},
        {"Jugo Natural",      8000, {"Limon","Azucar"}},
        {"Cafe Americano",    5000, {"Azucar"}},
        {"Agua Mineral",      3000, {}},
        {"Arepa con Queso",  12000, {"Harina","Queso","Sal"}}
    };
    { set<string> nombres_plato;
      for (auto& d : platos) nombres_plato.insert(d.nombre);
      int nuevos_platos = 0;
      for (auto& [nm, pventa, ing_names] : platos_sim) {
          if (!nombres_plato.count(nm)) {
              Plato d; d.id=++id_plato; d.nombre=nm; d.precio_venta=pventa;
              d.stock=ri(20,60); d.activo=true; d.creado=hoy();
              for (auto& inm : ing_names) {
                  Ingrediente* ing = ing_by_name(inm);
                  if (ing) d.ingredientes.push_back({ing->id, ing->nombre, rd(0.1,0.5)});
              }
              platos.push_back(d); nuevos_platos++;
          }
      }
      if (nuevos_platos) ok("  " + to_string(nuevos_platos) + " platos agregados");
    }
    // Proveedores
    if (proveedores.empty()) {
        vector<tuple<string,string,string>> provs = {
            {"Alimentos del Valle","Roberto Mora","3101234567"},
            {"Carnes Premium S.A.S","Claudia Rios","3209876543"},
            {"Frutas La Cosecha","Mario Nieto","3154567890"}
        };
        for (auto& [nm,ct,tel] : provs) {
            Proveedor p; p.id=++id_prov; p.nombre=nm; p.contacto=ct;
            p.telefono=tel; p.email="info@prov.com"; p.productos_sum="Varios";
            p.activo=true; p.creado=hoy();
            proveedores.push_back(p);
        }
        ok("  " + to_string(provs.size()) + " proveedores");
    }
    // Mesas — asegura al menos 10 activas
    { int mesas_act = 0;
      for (auto& m : mesas) if (m.activo) mesas_act++;
      if (mesas_act < 10) {
          set<int> nums;
          for (auto& m : mesas) nums.insert(m.numero);
          int num = nums.empty() ? 0 : *nums.rbegin();
          int faltan = 10 - mesas_act;
          vector<string> ubics = {"Salon","Terraza","VIP"};
          for (int i=0; i<faltan; i++) {
              num++;
              while (nums.count(num)) num++;
              nums.insert(num);
              Mesa m; m.id=++id_mesa; m.numero=num;
              m.capacidad=ri(2,6); m.ubicacion=ubics[ri(0,2)];
              m.estado="disponible"; m.activo=true;
              mesas.push_back(m);
          }
          ok("  " + to_string(faltan) + " mesas agregadas (total activas: 10)");
      }
    }
    // Empleados
    if (empleados.empty()) {
        vector<tuple<string,string>> emps = {
            {"Jorge Pena","Mesero"},{"Rosa Quintero","Mesero"},
            {"Edwin Caro","Cocinero"},{"Fernanda Diaz","Cajero"},{"Luis Ortega","Domiciliario"}
        };
        for (auto& [nm,rol] : emps) {
            Empleado e; e.id=++id_emp; e.nombre=nm;
            e.documento=to_string(ri(10000000,99999999));
            e.telefono="31"+to_string(ri(10000000,99999999));
            e.email=nm.substr(0,3)+"@dondesiempre.com";
            e.rol=rol; e.salario=round(SMLV*(1.0+rd(0,0.8)));
            { static const vector<string> cts={"Indefinido","Termino fijo","Prest. servicios"};
              e.contrato=cts[ri(0,2)]; }
            e.ingreso=hoy(); e.activo=true;
            empleados.push_back(e);
        }
        ok("  " + to_string(emps.size()) + " empleados");
    }
    // Clientes
    vector<string> nombres = {
        "Carlos Lopez","Maria Garcia","Juan Perez","Ana Martinez","Pedro Rodriguez",
        "Laura Sanchez","Diego Ramirez","Sofia Torres","Andres Herrera","Valentina Castro",
        "Camilo Vargas","Isabella Moreno","Sebastian Rojas","Daniela Mendoza","Felipe Restrepo",
        "Natalia Cardenas","Julian Ospina","Paola Quintero","Rodrigo Mendoza","Luisa Fernandez"
    };
    int nuevos = 0;
    for (auto& nm : nombres) {
        bool exists = false;
        for (auto& c : clientes) if (c.nombre==nm) { exists=true; break; }
        if (!exists) {
            Cliente c; c.id=++id_cli; c.nombre=nm;
            c.telefono="31"+to_string(ri(10000000,99999999));
            c.email=nm.substr(0,4)+to_string(ri(1,99))+"@mail.com";
            c.direccion="Calle "+to_string(ri(1,100))+" #"+to_string(ri(1,50))+"-"+to_string(ri(1,99));
            c.visitas=0; c.gastado=0.0; c.puntos=0;
            c.nivel="Bronce"; c.activo=true; c.creado=hoy();
            clientes.push_back(c); nuevos++;
        }
    }
    if (nuevos>0) ok("  " + to_string(nuevos) + " clientes");

    // Simular dias
    cout << "\n" << CYN << "Simulando " << dias << " dias..." << RST << "\n";
    int total_ped=0; double total_ing=0.0;

    time_t now_t = time(nullptr);
    for (int d=0; d<dias; d++) {
        time_t dia_t = now_t - (dias-d-1)*86400;
        tm* dt = localtime(&dia_t);
        char dfmt[11]; strftime(dfmt,sizeof(dfmt),"%Y-%m-%d",dt);
        string fecha_d = string(dfmt);
        int n_ped = ri(8, 25);
        for (int i=0; i<n_ped; i++) {
            string tipo = (ri(0,99)<65) ? "local" : "domicilio";
            Cliente& cli = clientes[ri(0,(int)clientes.size()-1)];
            Pedido p; p.id=++id_ped; p.tipo=tipo;
            p.fecha=fecha_d+" "+to_string(ri(10,22))+":"+to_string(ri(0,59));
            p.total=0.0; p.cliente_id=cli.id; p.cliente=cli.nombre;
            p.mesa_id=0; p.mesa_num=0;

            int n_items = ri(1,4);
            for (int j=0; j<n_items && j<(int)platos.size(); j++) {
                Plato& d = platos[ri(0,(int)platos.size()-1)];
                int cant = ri(1,3);
                double sub_t = cant * d.precio_venta;
                p.items.push_back({d.id, cant, d.nombre, "", d.precio_venta, sub_t});
                p.total += sub_t;
            }

            // Estado aleatorio ponderado
            int r_est = ri(0,99);
            p.estado = (r_est<82)?"entregado":(r_est<92)?"cancelado":"abierto";

            if (tipo=="local" && !mesas.empty()) {
                Mesa& m = mesas[ri(0,(int)mesas.size()-1)];
                p.mesa_id=m.id; p.mesa_num=m.numero;
            } else if (tipo=="domicilio") {
                p.direccion = cli.direccion;
            }
            pedidos.push_back(p);
            if (p.estado=="entregado") {
                cli.visitas++; cli.gastado+=p.total;
                cli.puntos+=(int)(p.total/1000);
                cli_nivel(cli);
                total_ing+=p.total; total_ped++;
            }
        }
        cout << "  Dia " << d+1 << "/" << dias << " — " << fecha_d << " — " << n_ped << " pedidos\n";
    }
    // Generar reservas futuras (entre 3 y 8)
    vector<string> horas_res = {"12:00","13:00","14:00","19:00","20:00","20:30","21:00"};
    vector<string> esp_opts  = {"","","","Cumpleanos","Mesa romantica","Sin gluten","Silla para nino","Alergico a lacteos"};
    vector<Mesa*> mesas_disp;
    for (auto& m : mesas) if (m.activo && m.estado=="disponible") mesas_disp.push_back(&m);
    int n_res = ri(3,8); int nuevas_res=0;
    for (int i=0; i<n_res; i++) {
        if (mesas_disp.empty() || clientes.empty()) break;
        Cliente& cli = clientes[ri(0,(int)clientes.size()-1)];
        Mesa* m = mesas_disp[ri(0,(int)mesas_disp.size()-1)];
        int dias_adel = ri(1,15);
        time_t fut_t = now_t + dias_adel*86400;
        tm* ft = localtime(&fut_t);
        char ffmt[11]; strftime(ffmt,sizeof(ffmt),"%d/%m/%Y",ft);
        int pers = ri(1, m->capacidad);
        Reserva r; r.id=++id_res;
        r.cliente_id=cli.id; r.cliente=cli.nombre;
        r.mesa_id=m->id; r.mesa_num=m->numero;
        r.fecha=string(ffmt);
        r.hora=horas_res[ri(0,(int)horas_res.size()-1)];
        r.personas=pers;
        r.especiales=esp_opts[ri(0,(int)esp_opts.size()-1)];
        r.estado=(ri(0,1)==0)?"pendiente":"confirmada";
        r.creado=ahora();
        reservas.push_back(r);
        nuevas_res++;
    }
    if (nuevas_res>0) ok("  "+to_string(nuevas_res)+" reservas generadas");

    // Generar pedidos a proveedores (entre 2 y 5)
    int n_comp = ri(2,5); int nuevas_comp=0;
    for (int i=0; i<n_comp; i++) {
        if (proveedores.empty() || ingredientes.empty()) break;
        Proveedor& pv = proveedores[ri(0,(int)proveedores.size()-1)];
        Compra c; c.id=++id_comp; c.proveedor_id=pv.id; c.proveedor=pv.nombre;
        // Fecha aleatoria en los ultimos 7 dias
        int dias_atras = ri(0,6);
        time_t fc_t = now_t - dias_atras*86400;
        tm* fc = localtime(&fc_t);
        char cfmt[11]; strftime(cfmt,sizeof(cfmt),"%Y-%m-%d",fc);
        c.fecha=string(cfmt); c.notas=""; c.total=0.0;
        // Entre 2 y 5 ingredientes distintos por pedido
        set<int> usados;
        int n_items = ri(2,5);
        for (int j=0; j<n_items; j++) {
            Ingrediente& ing = ingredientes[ri(0,(int)ingredientes.size()-1)];
            if (usados.count(ing.id)) continue;
            usados.insert(ing.id);
            ItemCompra it; it.prod_id=ing.id; it.nombre=ing.nombre;
            it.cant = round(rd(5.0,50.0)*10)/10.0;
            it.pu   = ing.precio;
            it.sub  = it.cant * it.pu;
            c.items.push_back(it); c.total += it.sub;
        }
        if (c.items.empty()) continue;
        // Estado: 70% recibido (y actualiza stock), 30% pendiente
        if (ri(0,9) < 7) {
            c.estado="recibido";
            c.fecha_recepcion=c.fecha;
            for (auto& it : c.items) {
                Ingrediente* ing = bid(ingredientes, it.prod_id);
                if (ing) ing->stock += it.cant;
            }
        } else {
            c.estado="pendiente"; c.fecha_recepcion="";
        }
        compras.push_back(c); nuevas_comp++;
    }
    if (nuevas_comp>0) ok("  "+to_string(nuevas_comp)+" pedidos a proveedores generados");

    cout << "\n" << BLD << GRN << "Simulacion completada:\n" << RST;
    cout << "  Pedidos entregados: " << total_ped << "\n";
    cout << "  Ingresos simulados: " << pesos(total_ing) << "\n";
    cout << "  Reservas generadas: " << nuevas_res << "\n";
    cout << "  Pedidos a proveedores: " << nuevas_comp << "\n";
    guardar();
}

// ──────────────────────────────────────────────────────
//  MODULO 11 — PLANES DE PROMOCION
// ──────────────────────────────────────────────────────
void promociones() {
    titulo("Planes de Promocion — Clientes Frecuentes");
    vector<Cliente*> activos;
    for (auto& c : clientes) if (c.activo && c.visitas>0) activos.push_back(&c);
    if (activos.empty()) { warn("Sin clientes con historial."); return; }

    int vip=0,oro=0,plata=0,bron=0;
    for (auto* c : activos) {
        if (c->nivel=="VIP")    vip++;
        else if (c->nivel=="Oro")   oro++;
        else if (c->nivel=="Plata") plata++;
        else bron++;
    }

    cout << "\n" << BLD << MAG << string(58,'-') << RST << "\n";
    cout << BLD << MAG << "  SEGMENTACION DE CLIENTES\n" << RST;
    cout << BLD << MAG << string(58,'-') << RST << "\n";
    cout << "  " << MAG << "VIP   " << RST << "(" << setw(3)<<vip   << ")  Gasto > $5.000.000\n";
    cout << "  " << YEL << "Oro   " << RST << "(" << setw(3)<<oro   << ")  Gasto $2.000.000 – $5.000.000\n";
    cout << "  " << CYN << "Plata " << RST << "(" << setw(3)<<plata << ")  Gasto $500.000 – $2.000.000\n";
    cout << "  " << WHT << "Bronce" << RST << "(" << setw(3)<<bron  << ")  Gasto < $500.000\n";

    cout << "\n" << BLD << YEL << "  PLANES SUGERIDOS\n" << RST;
    cout << string(58,'-') << "\n";

    cout << "\n  " << MAG << "* VIP EXCLUSIVO\n" << RST;
    cout << "  +- 15% descuento permanente en todos los pedidos\n";
    cout << "  +- Reserva prioritaria — confirmacion en 1 hora\n";
    cout << "  +- Menu degustacion gratis en cumpleanos\n";
    cout << "  +- Invitacion a eventos y maridajes especiales\n";
    if (vip>0) {
        cout << "\n  Clientes VIP (" << vip << "):\n";
        for (auto* c : activos) if (c->nivel=="VIP")
            cout << "    * " << left << setw(24)<<c->nombre << pesos(c->gastado) << "  " << c->visitas << " visitas\n";
    } else { cout << "  " << GRY << "  (sin clientes en este nivel aun)" << RST << "\n"; }

    cout << "\n  " << YEL << "* PROGRAMA ORO\n" << RST;
    cout << "  +- 10% descuento martes y miercoles\n";
    cout << "  +- Postre gratis en pedidos > $80.000\n";
    cout << "  +- Puntos dobles los fines de semana\n";
    if (oro>0) {
        cout << "\n  Clientes Oro (" << oro << "):\n";
        for (auto* c : activos) if (c->nivel=="Oro")
            cout << "    * " << left << setw(24)<<c->nombre << pesos(c->gastado) << "  " << c->visitas << " visitas\n";
    } else { cout << "  " << GRY << "  (sin clientes en este nivel aun)" << RST << "\n"; }

    cout << "\n  " << CYN << "* PROGRAMA PLATA\n" << RST;
    cout << "  +- 5% descuento comprando 3 veces por semana\n";
    cout << "  +- Bebida gratis en pedidos > $60.000\n";
    if (plata>0) {
        cout << "\n  Clientes Plata (" << plata << "):\n";
        for (auto* c : activos) if (c->nivel=="Plata")
            cout << "    * " << left << setw(24)<<c->nombre << pesos(c->gastado) << "  " << c->visitas << " visitas\n";
    } else { cout << "  " << GRY << "  (sin clientes en este nivel aun)" << RST << "\n"; }

    cout << "\n  " << WHT << "* BIENVENIDA (Bronce)\n" << RST;
    cout << "  +- 10% descuento en la segunda visita\n";
    cout << "  +- Entrada gratis en el primer pedido del mes\n";
    if (bron>0) {
        cout << "\n  Clientes Bronce (" << bron << "):\n";
        for (auto* c : activos) if (c->nivel=="Bronce")
            cout << "    * " << left << setw(24)<<c->nombre << pesos(c->gastado) << "  " << c->visitas << " visitas\n";
    } else { cout << "  " << GRY << "  (sin clientes en este nivel aun)" << RST << "\n"; }
    cout << "\n  OFERTA REACTIVACION:\n";
    cout << "  +- 'Te extranamos! 20% OFF en tu proxima visita'\n";

    // Top 5
    sort(activos.begin(), activos.end(), [](auto* a, auto* b){ return a->gastado>b->gastado; });
    cout << "\n" << BLD << "  TOP 5 — MAYOR GASTO:\n" << RST;
    map<string,string> nc = {{"VIP",MAG},{"Oro",YEL},{"Plata",CYN},{"Bronce",WHT}};
    int top = min((int)activos.size(), 5);
    for (int i=0; i<top; i++) {
        auto* c = activos[i];
        string cc = nc.count(c->nivel)?nc[c->nivel]:WHT;
        cout << "  " << i+1 << ". " << left << setw(24)<<c->nombre
             << cc << left << setw(6)<<c->nivel << RST
             << right << setw(14)<<pesos(c->gastado) << "  " << c->visitas << " visitas\n";
    }
    sort(activos.begin(), activos.end(), [](auto* a, auto* b){ return a->visitas>b->visitas; });
    cout << "\n" << BLD << "  TOP 5 — MAS FRECUENTES:\n" << RST;
    top = min((int)activos.size(), 5);
    for (int i=0; i<top; i++) {
        auto* c = activos[i];
        cout << "  " << i+1 << ". " << left << setw(24)<<c->nombre
             << right << setw(4)<<c->visitas << " visitas  " << pesos(c->gastado) << "\n";
    }
}

// ──────────────────────────────────────────────────────
//  BANNER Y MENU PRINCIPAL
// ──────────────────────────────────────────────────────
void banner() {
    limpiar();
    cout << BLD << CYN;
    cout << "  +======================================================+\n";
    cout << "  |             RESTAURANTE  \"Donde Siempre\"             |\n";
    cout << "  |            Sistema de Gestion — PGSR v1.0            |\n";
    cout << "  +======================================================+\n";
    cout << "  |             Universidad Popular del Cesar            |\n";
    cout << "  |     Ingenieria de Sistemas — Estructura de Datos     |\n";
    cout << "  |            Parcial 1 — Grupo 06 — 2026               |\n";
    cout << "  +======================================================+\n";
    cout << RST;
}

int main() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif

    banner();
    cout << "\n  " << YEL << "Cargar datos guardados? (s/n): " << RST;
    if (readline() == "s") cargar();
    else cout << "  Iniciando con datos vacios.\n";
    pausar();

    while (true) {
        banner();
        cout << "\n  " << BLD << "MENU PRINCIPAL\n\n" << RST;
        string o = mmenu({
            {"1", "Ingredientes"},
            {"2", "Platos"},
            {"3", "Proveedores y Pedidos a Proveedor"},
            {"4", "Mesas"},
            {"5", "Clientes"},
            {"6", "Reservas y Solicitudes Especiales"},
            {"7", "Pedidos (Local y Domicilio)"},
            {"8", "Empleados"},
            {"9", "Nomina"},
            {"10","Dashboard y Estadisticas"},
            {"11","Simulacion del Restaurante"},
            {"12","Planes de Promocion"},
            {"13","Guardar datos"},
            {"0", "Salir"}
        });
        if      (o=="1")  menu_ingredientes();
        else if (o=="2")  menu_platos();
        else if (o=="3")  menu_proveedores();
        else if (o=="4")  menu_mesas();
        else if (o=="5")  menu_clientes();
        else if (o=="6")  menu_reservas();
        else if (o=="7")  menu_pedidos();
        else if (o=="8")  menu_empleados();
        else if (o=="9")  menu_nomina();
        else if (o=="10") { dashboard(); pausar(); }
        else if (o=="11") { simular(); pausar(); }
        else if (o=="12") { promociones(); pausar(); }
        else if (o=="13") { guardar(); pausar(); }
        else {
            cout << "\n" << CYN << "Guardando datos...\n" << RST;
            guardar();
            cout << "\n" << BLD << GRN << "Hasta luego! Restaurante \"Donde Siempre\"\n\n" << RST;
            break;
        }
    }
    return 0;
}
