#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
╔══════════════════════════════════════════════════════╗
║             RESTAURANTE «DONDE SIEMPRE»              ║
║              Sistema de Gestión — PGSR               ║
╠══════════════════════════════════════════════════════╣
║            Universidad Popular del Cesar             ║
║       Facultad de Ingenierías y Tecnológicas         ║
║     Ingeniería de Sistemas — Estructura de Datos     ║
║             Parcial 1 — Grupo 06 — 2026              ║
╚══════════════════════════════════════════════════════╝
"""

import os, sys, io, random
from datetime import datetime, date, timedelta
from collections import defaultdict

# Siempre trabajar en la carpeta del script para que los .dat se guarden aqui
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# Forzar UTF-8 en la consola de Windows
if sys.platform == "win32":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
    sys.stdin  = io.TextIOWrapper(sys.stdin.buffer,  encoding="utf-8", errors="replace")

# ──────────────────────────────────────────────────────────
#  COLORES ANSI
# ──────────────────────────────────────────────────────────
if os.name == "nt":
    os.system("color")

R  = "\033[0m"
B  = "\033[1m"
RE = "\033[91m"
GR = "\033[92m"
YE = "\033[93m"
BL = "\033[94m"
MA = "\033[95m"
CY = "\033[96m"
WH = "\033[97m"
GY = "\033[90m"

# ──────────────────────────────────────────────────────────
#  BASE DE DATOS EN MEMORIA
# ──────────────────────────────────────────────────────────
DB = {
    "ingredients":  [],   # MÓDULO 1 — ingredientes (materia prima)
    "dishes":       [],   # MÓDULO 2 — platos del menú
    "suppliers":    [],
    "purchases":    [],
    "customers":    [],
    "tables":       [],
    "reservations": [],
    "orders":       [],
    "employees":    [],
    "payrolls":     [],
    "_ids": {
        "ingredients":0,"dishes":0,"suppliers":0,"purchases":0,"customers":0,
        "tables":0,"reservations":0,"orders":0,"employees":0,"payrolls":0
    }
}

# Decreto 1469 y 1470 del 29 de diciembre de 2025
SMLV             = 1_750_905   # Salario Mínimo Legal Vigente 2026 (COP) — aumento 23%
AUXILIO_TRANSPORTE = 249_095   # Auxilio de transporte 2026 (COP) — aumento 24.5%

# ──────────────────────────────────────────────────────────
#  PERSISTENCIA — archivos .dat separados por | (igual que C++)
# ──────────────────────────────────────────────────────────
def esc(s):
    return str(s).replace("|", "\x01").replace("~", "\x02")

def unesc(s):
    return s.replace("\x01", "|").replace("\x02", "~")

def enc_items_compra(items):
    return "~".join(
        f"{it['prod_id']};{esc(it['nombre'])};{it['cant']};{it['pu']};{it['sub']}"
        for it in items)

def dec_items_compra(s):
    if not s: return []
    result = []
    for part in s.split("~"):
        f = part.split(";")
        if len(f) < 5: continue
        result.append({"prod_id":int(f[0]),"nombre":unesc(f[1]),"cant":float(f[2]),"pu":float(f[3]),"sub":float(f[4])})
    return result

def enc_items_pedido(items):
    return "~".join(
        f"{it['prod_id']};{esc(it['nombre'])};{it['cant']};{it['pu']};{it['sub']};{esc(it.get('nota',''))}"
        for it in items)

def dec_items_pedido(s):
    if not s: return []
    result = []
    for part in s.split("~"):
        f = part.split(";")
        if len(f) < 6: continue
        result.append({"prod_id":int(f[0]),"nombre":unesc(f[1]),"cant":int(f[2]),"pu":float(f[3]),"sub":float(f[4]),"nota":unesc(f[5])})
    return result

def enc_ings_plato(ings):
    """Codifica la lista de ingredientes de un plato."""
    return "~".join(f"{i['ing_id']};{esc(i['nombre'])};{i['cantidad']}" for i in ings)

def dec_ings_plato(s):
    """Decodifica la lista de ingredientes de un plato."""
    if not s: return []
    result = []
    for part in s.split("~"):
        f = part.split(";")
        if len(f) < 3: continue
        result.append({"ing_id":int(f[0]),"nombre":unesc(f[1]),"cantidad":float(f[2])})
    return result

def guardar():
    ids = DB["_ids"]
    with open("ids.dat","w",encoding="utf-8") as f:
        f.write(f"ingredientes:{ids['ingredients']}|platos:{ids['dishes']}|proveedores:{ids['suppliers']}|compras:{ids['purchases']}|mesas:{ids['tables']}|clientes:{ids['customers']}|reservas:{ids['reservations']}|pedidos:{ids['orders']}|empleados:{ids['employees']}|nominas:{ids['payrolls']}")
    with open("ingredientes.dat","w",encoding="utf-8") as f:
        for i in DB["ingredients"]:
            f.write(f"{i['id']}|{esc(i['nombre'])}|{esc(i['categoria'])}|{esc(i['unidad'])}|{i['precio']}|{i['stock']}|{1 if i['activo'] else 0}|{esc(i['creado'])}\n")
    with open("platos.dat","w",encoding="utf-8") as f:
        for d in DB["dishes"]:
            f.write(f"{d['id']}|{esc(d['nombre'])}|{d['precio_venta']}|{d['stock']}|{1 if d['activo'] else 0}|{esc(d['creado'])}|{enc_ings_plato(d['ingredientes'])}\n")
    with open("prov.dat","w",encoding="utf-8") as f:
        for p in DB["suppliers"]:
            f.write(f"{p['id']}|{esc(p['nombre'])}|{esc(p['contacto'])}|{esc(p['telefono'])}|{esc(p['email'])}|{esc(p['productos'])}|{1 if p['activo'] else 0}|{esc(p['creado'])}\n")
    with open("compras.dat","w",encoding="utf-8") as f:
        for c in DB["purchases"]:
            f.write(f"{c['id']}|{c['proveedor_id']}|{esc(c['proveedor'])}|{c['fecha']}|{c['total']}|{esc(c['estado'])}|{esc(c['notas'])}|{esc(c.get('fecha_recepcion',''))}|{enc_items_compra(c['items'])}\n")
    with open("mesas.dat","w",encoding="utf-8") as f:
        for m in DB["tables"]:
            f.write(f"{m['id']}|{m['numero']}|{m['capacidad']}|{esc(m['ubicacion'])}|{esc(m['estado'])}|{1 if m['activo'] else 0}\n")
    with open("clientes.dat","w",encoding="utf-8") as f:
        for c in DB["customers"]:
            f.write(f"{c['id']}|{esc(c['nombre'])}|{esc(c['telefono'])}|{esc(c['email'])}|{esc(c['direccion'])}|{c['visitas']}|{c['gastado']}|{c['puntos']}|{esc(c['nivel'])}|{1 if c['activo'] else 0}|{esc(c['creado'])}\n")
    with open("reservas.dat","w",encoding="utf-8") as f:
        for r in DB["reservations"]:
            f.write(f"{r['id']}|{r['cliente_id']}|{esc(r['cliente'])}|{r['mesa_id']}|{r['mesa_num']}|{esc(r['fecha'])}|{esc(r['hora'])}|{r['personas']}|{esc(r['especiales'])}|{esc(r['estado'])}|{esc(r['creado'])}\n")
    with open("pedidos.dat","w",encoding="utf-8") as f:
        for p in DB["orders"]:
            f.write(f"{p['id']}|{p['cliente_id'] or 0}|{esc(p['cliente'])}|{esc(p['tipo'])}|{esc(p['fecha'])}|{p['total']}|{esc(p['estado'])}|{p['mesa_id'] or 0}|{p['mesa_num'] or 0}|{esc(p['direccion'])}|{esc(p['notas'])}|{enc_items_pedido(p['items'])}\n")
    with open("empleados.dat","w",encoding="utf-8") as f:
        for e in DB["employees"]:
            f.write(f"{e['id']}|{esc(e['nombre'])}|{esc(e['documento'])}|{esc(e['telefono'])}|{esc(e['email'])}|{esc(e['rol'])}|{e['salario']}|{esc(e['contrato'])}|{esc(e['ingreso'])}|{1 if e['activo'] else 0}\n")
    with open("nominas.dat","w",encoding="utf-8") as f:
        for n in DB["payrolls"]:
            f.write(f"{n['id']}|{n['emp_id']}|{esc(n['empleado'])}|{esc(n['rol'])}|{esc(n['periodo'])}|{esc(n['generado'])}|{n['salario_base']}|{n['auxilio_transporte']}|{n['ded_salud_emp']}|{n['ded_pension_emp']}|{n['total_deducciones']}|{n['salario_neto']}|{n['apt_salud_emp']}|{n['apt_pension_emp']}|{n['arl']}|{n['caja_compensacion']}|{n['icbf']}|{n['sena']}|{n['total_aportes_empleador']}|{n['cesantias']}|{n['int_cesantias']}|{n['prima_servicios']}|{n['vacaciones']}|{n['total_prestaciones']}|{n['costo_total_empleador']}\n")
    ok("Datos guardados correctamente")

def cargar():
    if not os.path.exists("ids.dat"):
        warn("Archivos no encontrados. Iniciando con datos vacios.")
        return False
    try:
        with open("ids.dat","r",encoding="utf-8") as f:
            key_map = {"ingredientes":"ingredients","platos":"dishes",
                       "proveedores":"suppliers","compras":"purchases",
                       "mesas":"tables","clientes":"customers","reservas":"reservations",
                       "pedidos":"orders","empleados":"employees","nominas":"payrolls"}
            for part in f.read().strip().split("|"):
                if ":" in part:
                    k, v = part.split(":", 1)
                    if k in key_map: DB["_ids"][key_map[k]] = int(v)
        if os.path.exists("ingredientes.dat"):
            with open("ingredientes.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 8: continue
                    DB["ingredients"].append({"id":int(t[0]),"nombre":unesc(t[1]),"categoria":unesc(t[2]),"unidad":unesc(t[3]),"precio":float(t[4]),"stock":float(t[5]),"activo":t[6]=="1","creado":unesc(t[7])})
        if os.path.exists("platos.dat"):
            with open("platos.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 7: continue
                    DB["dishes"].append({"id":int(t[0]),"nombre":unesc(t[1]),"precio_venta":float(t[2]),"stock":int(t[3]),"activo":t[4]=="1","creado":unesc(t[5]),"ingredientes":dec_ings_plato(t[6])})
        if os.path.exists("prov.dat"):
            with open("prov.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 8: continue
                    DB["suppliers"].append({"id":int(t[0]),"nombre":unesc(t[1]),"contacto":unesc(t[2]),"telefono":unesc(t[3]),"email":unesc(t[4]),"productos":unesc(t[5]),"activo":t[6]=="1","creado":unesc(t[7])})
        if os.path.exists("compras.dat"):
            with open("compras.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 9: continue
                    DB["purchases"].append({"id":int(t[0]),"proveedor_id":int(t[1]),"proveedor":unesc(t[2]),"fecha":t[3],"total":float(t[4]),"estado":unesc(t[5]),"notas":unesc(t[6]),"fecha_recepcion":unesc(t[7]),"items":dec_items_compra(t[8])})
        if os.path.exists("mesas.dat"):
            with open("mesas.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 6: continue
                    DB["tables"].append({"id":int(t[0]),"numero":int(t[1]),"capacidad":int(t[2]),"ubicacion":unesc(t[3]),"estado":unesc(t[4]),"activo":t[5]=="1"})
        if os.path.exists("clientes.dat"):
            with open("clientes.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 11: continue
                    DB["customers"].append({"id":int(t[0]),"nombre":unesc(t[1]),"telefono":unesc(t[2]),"email":unesc(t[3]),"direccion":unesc(t[4]),"visitas":int(t[5]),"gastado":float(t[6]),"puntos":int(t[7]),"nivel":unesc(t[8]),"activo":t[9]=="1","creado":unesc(t[10])})
        if os.path.exists("reservas.dat"):
            with open("reservas.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 11: continue
                    DB["reservations"].append({"id":int(t[0]),"cliente_id":int(t[1]),"cliente":unesc(t[2]),"mesa_id":int(t[3]),"mesa_num":int(t[4]),"fecha":unesc(t[5]),"hora":unesc(t[6]),"personas":int(t[7]),"especiales":unesc(t[8]),"estado":unesc(t[9]),"creado":unesc(t[10])})
        if os.path.exists("pedidos.dat"):
            with open("pedidos.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 12: continue
                    mid = int(t[7]); mnum = int(t[8])
                    DB["orders"].append({"id":int(t[0]),"cliente_id":int(t[1]) or None,"cliente":unesc(t[2]),"tipo":unesc(t[3]),"fecha":unesc(t[4]),"total":float(t[5]),"estado":unesc(t[6]),"mesa_id":mid or None,"mesa_num":mnum or None,"direccion":unesc(t[9]),"notas":unesc(t[10]),"items":dec_items_pedido(t[11])})
        if os.path.exists("empleados.dat"):
            with open("empleados.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 10: continue
                    DB["employees"].append({"id":int(t[0]),"nombre":unesc(t[1]),"documento":unesc(t[2]),"telefono":unesc(t[3]),"email":unesc(t[4]),"rol":unesc(t[5]),"salario":float(t[6]),"contrato":unesc(t[7]),"ingreso":unesc(t[8]),"activo":t[9]=="1"})
        if os.path.exists("nominas.dat"):
            with open("nominas.dat","r",encoding="utf-8") as f:
                for line in f:
                    t = line.rstrip("\n").split("|")
                    if len(t) < 25: continue
                    DB["payrolls"].append({"id":int(t[0]),"emp_id":int(t[1]),"empleado":unesc(t[2]),"rol":unesc(t[3]),"periodo":unesc(t[4]),"generado":unesc(t[5]),"salario_base":float(t[6]),"auxilio_transporte":float(t[7]),"ded_salud_emp":float(t[8]),"ded_pension_emp":float(t[9]),"total_deducciones":float(t[10]),"salario_neto":float(t[11]),"apt_salud_emp":float(t[12]),"apt_pension_emp":float(t[13]),"arl":float(t[14]),"caja_compensacion":float(t[15]),"icbf":float(t[16]),"sena":float(t[17]),"total_aportes_empleador":float(t[18]),"cesantias":float(t[19]),"int_cesantias":float(t[20]),"prima_servicios":float(t[21]),"vacaciones":float(t[22]),"total_prestaciones":float(t[23]),"costo_total_empleador":float(t[24])})
        ok("Datos cargados correctamente")
        return True
    except Exception as e:
        err(f"Error al cargar: {e}")
        return False

# ──────────────────────────────────────────────────────────
#  UTILIDADES
# ──────────────────────────────────────────────────────────
def nid(col):
    DB["_ids"][col] += 1
    return DB["_ids"][col]

def limpiar():
    os.system("cls" if os.name == "nt" else "clear")

def pausa():
    input(f"\n{GY}Presione ENTER para continuar...{R}")

def titulo(t):
    w = 58
    print(f"\n{B}{BL}{'═'*w}{R}")
    print(f"{B}{CY}  {t.upper()}{R}")
    print(f"{B}{BL}{'═'*w}{R}\n")

def sub(t):
    print(f"\n{B}{YE}── {t} ──{R}\n")

def ok(m):   print(f"{GR}✓ {m}{R}")
def err(m):  print(f"{RE}✗ {m}{R}")
def warn(m): print(f"{YE}⚠ {m}{R}")

def pint(prompt, mn=None, mx=None):
    while True:
        try:
            v = int(input(prompt))
            if mn is not None and v < mn: err(f"Mínimo {mn}"); continue
            if mx is not None and v > mx: err(f"Máximo {mx}"); continue
            return v
        except ValueError: err("Ingrese un entero")

def pfloat(prompt, mn=0.0):
    while True:
        try:
            v = float(input(prompt))
            if v < mn: err(f"Mínimo {mn}"); continue
            return v
        except ValueError: err("Ingrese un número")

def popc(opts):
    while True:
        o = input("Opción: ").strip()
        if o in opts: return o
        err(f"Opciones válidas: {', '.join(opts)}")

def pesos(v): return f"${v:>12,.0f}".replace(",", ".")

def bid(col, i): return next((x for x in DB[col] if x["id"] == i), None)

def hoy(): return date.today().isoformat()
def ahora(): return datetime.now().strftime("%Y-%m-%d %H:%M")

def barra(v, mx, w=25):
    if mx == 0: return ""
    f = int((v/mx)*w)
    return f"{CY}{'█'*f}{GY}{'░'*(w-f)}{R}"

def unidad_sim(cat):
    """Retorna la unidad correcta según la categoría del producto."""
    if cat in ["Plato Principal", "Entrada", "Postre"]: return "porcion"
    if cat == "Bebida": return random.choice(["unidad", "litro", "ml"])
    if cat == "Insumo": return random.choice(["kg", "g", "litro", "ml"])
    return "unidad"

def pedir_periodo_col(prompt):
    """Pide un periodo en formato MM/AA con validacion."""
    default = datetime.now().strftime("%m/%y")
    while True:
        val = input(f"{prompt}(Enter={default}): ").strip() or default
        if len(val) == 5 and val[2] == "/" and val[:2].isdigit() and val[3:].isdigit():
            mm = int(val[:2])
            if 1 <= mm <= 12:
                return val
        err("Formato invalido. Use MM/AA (ej: 03/26)")

def listar_ops(titulo, items):
    """Muestra una lista numerada de opciones, una por línea."""
    print(f"\n{GY}  {titulo}:{R}")
    for i, item in enumerate(items):
        print(f"  {CY}{i+1:>2}.{R} {item}")
    print()

def mmenu(opciones):
    """Muestra un menú con opciones en líneas individuales y retorna la selección.
    opciones: lista de tuplas (clave, etiqueta)
    """
    print()
    for k, label in opciones:
        if k == "0":
            print(f"  {RE} 0.{R} {label}")
        else:
            print(f"  {CY}{k:>2}.{R} {label}")
    print()
    return popc([k for k, _ in opciones])

# ──────────────────────────────────────────────────────────
#  MÓDULO 1 — INGREDIENTES
# ──────────────────────────────────────────────────────────
CATS_ING = ["verduras","carnes","aceites","granos","lacteos","frutas","condimentos"]
UNDS_ING = ["kg","litro","unidad","gramos"]

def ing_crear():
    titulo("Nuevo Ingrediente")
    i = {
        "id":       nid("ingredients"),
        "nombre":   input("Nombre del ingrediente: ").strip(),
        "categoria":"",
        "unidad":   "",
        "precio":   0.0,
        "stock":    0.0,
        "activo":   True,
        "creado":   hoy()
    }
    listar_ops("Categoria", CATS_ING)
    i["categoria"] = CATS_ING[pint("Cat #: ",1,len(CATS_ING))-1]
    listar_ops("Unidad de medida", UNDS_ING)
    i["unidad"]  = UNDS_ING[pint("Unidad #: ",1,len(UNDS_ING))-1]
    i["precio"]  = pfloat("Precio por unidad ($): ", 0.01)
    i["stock"]   = pfloat("Cantidad en stock: ", 0)
    DB["ingredients"].append(i)
    ok(f"Ingrediente '{i['nombre']}' creado ID#{i['id']}")
    return i

def ing_listar(activos=True):
    rows = [i for i in DB["ingredients"] if not activos or i["activo"]]
    if not rows: warn("Sin ingredientes."); return
    print(f"\n{'ID':<5}{'Nombre':<24}{'Categoria':<14}{'Unidad':<9}{'Precio/u':>12}{'Stock':>10}  {'Estado'}")
    print("─"*82)
    for i in rows:
        st = f"{GR}Activo{R}" if i["activo"] else f"{RE}Inactivo{R}"
        print(f"{i['id']:<5}{i['nombre']:<24}{i['categoria']:<14}{i['unidad']:<9}{pesos(i['precio']):>12}{i['stock']:>10.2f}  {st}")

def ing_modificar():
    ing_listar(False)
    if not DB["ingredients"]: return
    i = bid("ingredients", pint("ID: ",1))
    if not i: err("No encontrado"); return
    sub(f"Modificando: {i['nombre']}")
    print(f"  Categoria : {i['categoria']}")
    print(f"  Unidad    : {i['unidad']}")
    print(f"  Precio    : {pesos(i['precio'])}")
    print(f"  Stock     : {i['stock']}")
    o = mmenu([("1","Nombre"),("2","Categoria"),("3","Unidad"),
               ("4","Precio por unidad"),("5","Ajuste stock (+/-)"),("0","Cancelar")])
    if o=="1":   i["nombre"]   = input("Nuevo nombre: ").strip()
    elif o=="2":
        listar_ops("Categoria", CATS_ING)
        i["categoria"] = CATS_ING[pint("Cat #: ",1,len(CATS_ING))-1]
    elif o=="3":
        listar_ops("Unidad", UNDS_ING)
        i["unidad"] = UNDS_ING[pint("Unidad #: ",1,len(UNDS_ING))-1]
    elif o=="4": i["precio"]  = pfloat("Nuevo precio ($): ", 0.01)
    elif o=="5":
        d = pfloat("Ajuste (+/-): ")
        i["stock"] = max(0.0, i["stock"] + d)
        ok(f"Stock actualizado: {i['stock']}")
    if o != "0": ok("Ingrediente actualizado")

def menu_ingredientes():
    while True:
        titulo("Modulo 1 — Ingredientes")
        o = mmenu([("1","Nuevo ingrediente"),("2","Listar todos"),
                   ("3","Modificar ingrediente"),("4","Activar / Desactivar"),
                   ("5","Eliminar ingrediente"),("0","Volver")])
        if o=="1": ing_crear()
        elif o=="2": ing_listar(False); pausa()
        elif o=="3": ing_modificar()
        elif o=="4":
            ing_listar(False)
            if DB["ingredients"]:
                i = bid("ingredients", pint("ID: ",1))
                if i:
                    i["activo"] = not i["activo"]
                    ok(f"{'Activado' if i['activo'] else 'Desactivado'}")
        elif o=="5":
            ing_listar(False)
            if DB["ingredients"]:
                i = bid("ingredients", pint("ID: ",1))
                if i and input(f"{RE}¿Eliminar? (s/n): {R}").lower()=="s":
                    DB["ingredients"].remove(i); ok("Eliminado")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 2 — PLATOS
# ──────────────────────────────────────────────────────────
def plato_crear():
    titulo("Nuevo Plato")
    if not DB["ingredients"]:
        warn("No hay ingredientes. Registra ingredientes primero."); return
    d = {
        "id":          nid("dishes"),
        "nombre":      input("Nombre del plato: ").strip(),
        "precio_venta":0.0,
        "stock":       0,
        "activo":      True,
        "creado":      hoy(),
        "ingredientes":[]
    }
    while True:
        ing_listar()
        if input("\n¿Añadir ingrediente al plato? (s/n): ").lower()!="s": break
        ing = bid("ingredients", pint("ID ingrediente: ",1))
        if not ing or not ing["activo"]:
            err("Ingrediente no encontrado o inactivo"); continue
        if any(x["ing_id"]==ing["id"] for x in d["ingredientes"]):
            warn(f"'{ing['nombre']}' ya esta en el plato"); continue
        cant = pfloat(f"Cantidad de {ing['nombre']} ({ing['unidad']}): ", 0.01)
        d["ingredientes"].append({"ing_id":ing["id"],"nombre":ing["nombre"],"cantidad":cant})
        ok(f"  +{cant} {ing['unidad']} de {ing['nombre']}")
    if not d["ingredientes"]:
        warn("El plato debe tener al menos un ingrediente, cancelado.")
        DB["_ids"]["dishes"] -= 1; return
    d["precio_venta"] = pfloat("Precio de venta ($): ", 0.01)
    d["stock"]        = pint("Stock disponible (unidades): ", 0)
    DB["dishes"].append(d)
    ok(f"Plato '{d['nombre']}' creado ID#{d['id']}")
    return d

def plato_listar(activos=True):
    rows = [d for d in DB["dishes"] if not activos or d["activo"]]
    if not rows: warn("Sin platos."); return
    print(f"\n{'ID':<5}{'Nombre':<28}{'Precio venta':>14}{'Stock':>8}  {'Estado'}")
    print("─"*64)
    for d in rows:
        st = f"{GR}Activo{R}" if d["activo"] else f"{RE}Inactivo{R}"
        print(f"{d['id']:<5}{d['nombre']:<28}{pesos(d['precio_venta']):>14}{d['stock']:>8}  {st}")
        for ing in d["ingredientes"]:
            print(f"     {GY}  ↳ {ing['nombre']:<22} {ing['cantidad']}{R}")

def plato_modificar():
    plato_listar(False)
    if not DB["dishes"]: return
    d = bid("dishes", pint("ID: ",1))
    if not d: err("No encontrado"); return
    sub(f"Modificando: {d['nombre']}")
    print(f"  Precio venta : {pesos(d['precio_venta'])}")
    print(f"  Stock        : {d['stock']}")
    o = mmenu([("1","Nombre"),("2","Precio de venta"),("3","Stock"),
               ("4","Añadir ingrediente"),("5","Quitar ingrediente"),("0","Cancelar")])
    if o=="1":   d["nombre"]       = input("Nuevo nombre: ").strip()
    elif o=="2": d["precio_venta"] = pfloat("Nuevo precio ($): ", 0.01)
    elif o=="3": d["stock"]        = pint("Nuevo stock: ", 0)
    elif o=="4":
        ing_listar()
        ing = bid("ingredients", pint("ID ingrediente: ",1))
        if ing and ing["activo"]:
            cant = pfloat(f"Cantidad ({ing['unidad']}): ", 0.01)
            d["ingredientes"].append({"ing_id":ing["id"],"nombre":ing["nombre"],"cantidad":cant})
            ok("Ingrediente añadido")
        else: err("Ingrediente no encontrado o inactivo")
    elif o=="5":
        if not d["ingredientes"]: warn("Sin ingredientes en este plato"); return
        for idx,ing in enumerate(d["ingredientes"]):
            print(f"  {CY}{idx+1}.{R} {ing['nombre']} (x{ing['cantidad']})")
        idx = pint("# a eliminar: ",1,len(d["ingredientes"]))-1
        ok(f"'{d['ingredientes'].pop(idx)['nombre']}' eliminado")
    if o != "0": ok("Plato actualizado")

def menu_platos():
    while True:
        titulo("Modulo 2 — Platos")
        o = mmenu([("1","Nuevo plato"),("2","Listar todos"),
                   ("3","Modificar plato"),("4","Activar / Desactivar"),
                   ("5","Eliminar plato"),("0","Volver")])
        if o=="1": plato_crear()
        elif o=="2": plato_listar(False); pausa()
        elif o=="3": plato_modificar()
        elif o=="4":
            plato_listar(False)
            if DB["dishes"]:
                d = bid("dishes", pint("ID: ",1))
                if d:
                    d["activo"] = not d["activo"]
                    ok(f"{'Activado' if d['activo'] else 'Desactivado'}")
        elif o=="5":
            plato_listar(False)
            if DB["dishes"]:
                d = bid("dishes", pint("ID: ",1))
                if d and input(f"{RE}¿Eliminar plato? (s/n): {R}").lower()=="s":
                    DB["dishes"].remove(d); ok("Eliminado")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 2 — PROVEEDORES Y PEDIDOS A PROVEEDOR
# ──────────────────────────────────────────────────────────
def prov_crear():
    titulo("Nuevo Proveedor")
    p = {
        "id":       nid("suppliers"),
        "nombre":   input("Empresa: ").strip(),
        "contacto": input("Contacto: ").strip(),
        "telefono": input("Teléfono: ").strip(),
        "email":    input("Email: ").strip(),
        "productos":input("Productos que suministra: ").strip(),
        "activo":   True,
        "creado":   hoy()
    }
    DB["suppliers"].append(p)
    ok(f"Proveedor '{p['nombre']}' ID#{p['id']}")

def prov_listar(activos=True):
    rows = [p for p in DB["suppliers"] if not activos or p["activo"]]
    if not rows: warn("Sin proveedores."); return
    print(f"\n{'ID':<5}{'Nombre':<28}{'Contacto':<20}{'Teléfono':<15}{'Estado'}")
    print("─"*76)
    for p in rows:
        st = f"{GR}Activo{R}" if p["activo"] else f"{RE}Inactivo{R}"
        print(f"{p['id']:<5}{p['nombre']:<28}{p['contacto']:<20}{p['telefono']:<15}{st}")

def compra_crear():
    titulo("Nuevo Pedido a Proveedor")
    prov_listar()
    if not DB["suppliers"]: return
    sv = bid("suppliers", pint("ID proveedor: ",1))
    if not sv: err("No encontrado"); return
    if not DB["ingredients"]:
        warn("No hay ingredientes registrados. Crea ingredientes primero."); return
    c = {
        "id":           nid("purchases"),
        "proveedor_id": sv["id"],
        "proveedor":    sv["nombre"],
        "fecha":        hoy(),
        "items":        [],
        "total":        0.0,
        "estado":       "pendiente",
        "notas":        input("Notas: ").strip()
    }
    while True:
        ing_listar()
        if input("\n¿Agregar ingrediente? (s/n): ").lower()!="s": break
        ing = bid("ingredients", pint("ID ingrediente: ",1))
        if not ing: err("No encontrado"); continue
        cant  = pfloat(f"Cantidad de {ing['nombre']} ({ing['unidad']}): ", 0.01)
        sub_t = cant * ing["precio"]
        c["items"].append({"prod_id":ing["id"],"nombre":ing["nombre"],"cant":cant,"pu":ing["precio"],"sub":sub_t})
        c["total"] += sub_t
        print(f"  {GR}Subtotal {ing['nombre']}: {pesos(sub_t)}{R}")
        ok(f"  Total acumulado del pedido: {pesos(c['total'])}")
        if input("¿Añadir otro ingrediente? (s/n): ").lower()!="s": break
    if not c["items"]: warn("Pedido vacío, cancelado."); return
    print(f"\n{B}{CY}  Total general del pedido: {pesos(c['total'])}{R}")
    DB["purchases"].append(c)
    ok(f"Pedido #{c['id']} a {sv['nombre']} creado — {pesos(c['total'])}")

def compra_recibir():
    rows = [c for c in DB["purchases"] if c["estado"]=="pendiente"]
    if not rows: warn("Sin pedidos pendientes."); return
    print(f"\n{'ID':<5}{'Proveedor':<28}{'Fecha':<12}{'Total':>14}")
    print("─"*62)
    for c in rows:
        print(f"{c['id']:<5}{c['proveedor']:<28}{c['fecha']:<12}{pesos(c['total']):>14}")
    c = bid("purchases", pint("\nID pedido: ",1))
    if not c: err("No encontrado"); return
    c["estado"] = "recibido"
    c["fecha_recepcion"] = hoy()
    for item in c["items"]:
        ing = bid("ingredients", item["prod_id"])
        if ing:
            ing["stock"] += item["cant"]
            ok(f"  {ing['nombre']}: +{item['cant']} {ing['unidad']} → {ing['stock']}")
    ok(f"Pedido #{c['id']} recibido — stock de ingredientes actualizado")

def compra_listar():
    if not DB["purchases"]: warn("Sin pedidos."); return
    col = {"pendiente":YE,"recibido":GR,"cancelado":RE}
    for c in DB["purchases"]:
        cc = col.get(c["estado"],WH)
        print(f"\n{B}Pedido #{c['id']}{R}  Proveedor ID:{c['proveedor_id']} — {c['proveedor']}  {cc}{c['estado']}{R}  {c['fecha']}")
        print(f"  {'Ingrediente':<26}{'Cantidad':>10}  {'Precio/u':>12}  {'Subtotal':>12}")
        print(f"  {'─'*64}")
        for it in c["items"]:
            print(f"  {it['nombre']:<26}{it['cant']:>10.2f}  {pesos(it['pu']):>12}  {pesos(it['sub']):>12}")
        print(f"  {'─'*64}")
        print(f"  {B}{'TOTAL':>51} {pesos(c['total']):>12}{R}")

def compra_anular():
    pendientes = [c for c in DB["purchases"] if c["estado"]=="pendiente"]
    if not pendientes:
        warn("No hay pedidos pendientes para anular."); return
    print(f"\n{'ID':<5}{'Proveedor':<28}{'Fecha':<12}{'Total':>14}")
    print("─"*62)
    for c in pendientes:
        print(f"{c['id']:<5}{c['proveedor']:<28}{c['fecha']:<12}{pesos(c['total']):>14}")
    c = bid("purchases", pint("\nID pedido a anular: ",1))
    if not c: err("No encontrado"); return
    if c["estado"] != "pendiente":
        warn(f"El pedido #{c['id']} ya esta en estado '{c['estado']}' y no puede anularse."); return
    if input(f"{RE}¿Anular pedido #{c['id']} a {c['proveedor']}? (s/n): {R}").lower()=="s":
        c["estado"] = "cancelado"
        ok(f"Pedido #{c['id']} anulado correctamente.")

def menu_proveedores():
    while True:
        titulo("Proveedores y Pedidos")
        o = mmenu([("1","Nuevo proveedor"),("2","Listar proveedores"),
                   ("3","Modificar proveedor"),("4","Desactivar / Activar proveedor"),
                   ("5","Nuevo pedido a proveedor"),("6","Listar pedidos"),
                   ("7","Recibir pedido (actualiza stock)"),("8","Anular pedido pendiente"),
                   ("0","Volver")])
        if o=="1": prov_crear()
        elif o=="2": prov_listar(False); pausa()
        elif o=="3":
            prov_listar(False)
            if DB["suppliers"]:
                p = bid("suppliers", pint("ID: ",1))
                if p:
                    oo = mmenu([("1","Nombre"),("2","Contacto"),("3","Telefono"),("4","Email"),("0","Cancelar")])
                    if oo=="1":   p["nombre"]   = input("Nuevo: ").strip()
                    elif oo=="2": p["contacto"] = input("Nuevo: ").strip()
                    elif oo=="3": p["telefono"] = input("Nuevo: ").strip()
                    elif oo=="4": p["email"]    = input("Nuevo: ").strip()
                    if oo != "0": ok("Proveedor actualizado")
        elif o=="4":
            prov_listar(False)
            if DB["suppliers"]:
                p = bid("suppliers", pint("ID: ",1))
                if p:
                    p["activo"] = not p["activo"]
                    ok(f"{'Activado' if p['activo'] else 'Desactivado'}")
        elif o=="5": compra_crear()
        elif o=="6": compra_listar(); pausa()
        elif o=="7": compra_recibir()
        elif o=="8": compra_anular()
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 3 — MESAS
# ──────────────────────────────────────────────────────────
EST_MESA = ["disponible","ocupada","reservada","mantenimiento"]

def mesa_crear():
    titulo("Nueva Mesa")
    m = {
        "id":        nid("tables"),
        "numero":    pint("Número de mesa: ",1),
        "capacidad": pint("Capacidad (personas): ",1),
        "ubicacion": input("Ubicación (Salón/Terraza/etc): ").strip(),
        "estado":    "disponible",
        "activo":    True
    }
    DB["tables"].append(m)
    ok(f"Mesa #{m['numero']} ID#{m['id']}")

def mesa_listar(activas=True):
    rows = [m for m in DB["tables"] if not activas or m["activo"]]
    if not rows: warn("Sin mesas."); return
    col = {"disponible":GR,"ocupada":RE,"reservada":YE,"mantenimiento":GY}
    print(f"\n{'ID':<5}{'Nº':<5}{'Cap':<6}{'Ubicación':<18}{'Estado':<16}{'Activo'}")
    print("─"*58)
    for m in rows:
        cc = col.get(m["estado"],WH)
        print(f"{m['id']:<5}{m['numero']:<5}{m['capacidad']:<6}{m['ubicacion']:<18}{cc}{m['estado']:<16}{R}{'Sí' if m['activo'] else 'No'}")

def menu_mesas():
    while True:
        titulo("Gestión de Mesas")
        o = mmenu([("1","Nueva mesa"),("2","Listar mesas"),
                   ("3","Cambiar estado de mesa"),("4","Modificar mesa"),
                   ("5","Desactivar / Activar mesa"),("6","Eliminar mesa"),
                   ("0","Volver")])
        if o=="1": mesa_crear()
        elif o=="2": mesa_listar(False); pausa()
        elif o=="3":
            mesa_listar()
            if DB["tables"]:
                m = bid("tables", pint("ID: ",1))
                if m:
                    listar_ops("Estado", EST_MESA)
                    m["estado"] = EST_MESA[pint("Estado #: ",1,len(EST_MESA))-1]
                    ok(f"Mesa #{m['numero']} → {m['estado']}")
        elif o=="4":
            mesa_listar(False)
            if DB["tables"]:
                m = bid("tables", pint("ID: ",1))
                if m:
                    oo = mmenu([("1","Numero de mesa"),("2","Capacidad"),("3","Ubicacion"),("0","Cancelar")])
                    if oo=="1":   m["numero"]    = pint("Nuevo #: ",1)
                    elif oo=="2": m["capacidad"] = pint("Nueva capacidad: ",1)
                    elif oo=="3": m["ubicacion"] = input("Nueva ubicacion: ").strip()
                    if oo != "0": ok("Mesa actualizada")
        elif o=="5":
            mesa_listar(False)
            if DB["tables"]:
                m = bid("tables", pint("ID: ",1))
                if m:
                    m["activo"] = not m["activo"]
                    ok(f"{'Activada' if m['activo'] else 'Desactivada'}")
        elif o=="6":
            mesa_listar(False)
            if DB["tables"]:
                m = bid("tables", pint("ID: ",1))
                if m and input(f"{RE}¿Eliminar mesa #{m['numero']}? (s/n): {R}").lower()=="s":
                    DB["tables"].remove(m); ok("Mesa eliminada")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 4 — CLIENTES
# ──────────────────────────────────────────────────────────
def cli_nivel(c):
    g = c["gastado"]
    c["nivel"] = "VIP" if g>=5_000_000 else "Oro" if g>=2_000_000 else "Plata" if g>=500_000 else "Bronce"

def cli_crear():
    titulo("Nuevo Cliente")
    c = {
        "id":        nid("customers"),
        "nombre":    input("Nombre: ").strip(),
        "telefono":  input("Teléfono: ").strip(),
        "email":     input("Email: ").strip(),
        "direccion": input("Dirección: ").strip(),
        "visitas":   0,
        "gastado":   0.0,
        "puntos":    0,
        "nivel":     "Bronce",
        "activo":    True,
        "creado":    hoy()
    }
    DB["customers"].append(c)
    ok(f"Cliente '{c['nombre']}' ID#{c['id']}")
    return c

def cli_listar(activos=True):
    rows = [c for c in DB["customers"] if not activos or c["activo"]]
    if not rows: warn("Sin clientes."); return
    nc = {"VIP":MA,"Oro":YE,"Plata":CY,"Bronce":WH}
    print(f"\n{'ID':<5}{'Nombre':<26}{'Teléfono':<14}{'Visitas':>8}   {'Total Gastado':>14}   {'Nivel':<10}{'Estado'}")
    print("─"*86)
    for c in rows:
        cc = nc.get(c["nivel"],WH)
        st = f"{GR}Activo{R}" if c["activo"] else f"{RE}Inactivo{R}"
        print(f"{c['id']:<5}{c['nombre']:<26}{c['telefono']:<14}{c['visitas']:>8}   {pesos(c['gastado']):>14}   {cc}{c['nivel']:<10}{R}{st}")

def menu_clientes():
    while True:
        titulo("Gestión de Clientes")
        o = mmenu([("1","Nuevo cliente"),("2","Listar clientes"),
                   ("3","Buscar cliente por teléfono"),("4","Modificar cliente"),
                   ("5","Desactivar / Activar cliente"),("0","Volver")])
        if o=="1": cli_crear()
        elif o=="2": cli_listar(False); pausa()
        elif o=="3":
            tel = input("Teléfono: ").strip()
            rs  = [c for c in DB["customers"] if c["telefono"]==tel]
            if rs:
                for c in rs:
                    print(f"\n  {CY}#{c['id']}{R} {c['nombre']} | {c['nivel']} | visitas:{c['visitas']} | gastado:{pesos(c['gastado'])}")
            else: warn("No encontrado")
            pausa()
        elif o=="4":
            cli_listar(False)
            if DB["customers"]:
                c = bid("customers", pint("ID: ",1))
                if c:
                    oo = mmenu([("1","Nombre"),("2","Telefono"),("3","Email"),("4","Direccion"),("0","Cancelar")])
                    if oo=="1":   c["nombre"]    = input("Nuevo: ").strip()
                    elif oo=="2": c["telefono"]  = input("Nuevo: ").strip()
                    elif oo=="3": c["email"]     = input("Nuevo: ").strip()
                    elif oo=="4": c["direccion"] = input("Nuevo: ").strip()
                    if oo != "0": ok("Cliente actualizado")
        elif o=="5":
            cli_listar(False)
            if DB["customers"]:
                c = bid("customers", pint("ID: ",1))
                if c:
                    c["activo"] = not c["activo"]
                    ok(f"{'Activado' if c['activo'] else 'Desactivado'}")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 5 — RESERVAS
# ──────────────────────────────────────────────────────────
EST_RES = ["pendiente","confirmada","cancelada","en_mesa"]

def hoy_col():
    """Fecha de hoy en formato colombiano DD/MM/AAAA"""
    return date.today().strftime("%d/%m/%Y")

def pedir_fecha_col(prompt):
    """Pide una fecha en formato DD/MM/AAAA con validación."""
    while True:
        val = input(prompt).strip()
        if not val: return hoy_col()
        try:
            datetime.strptime(val, "%d/%m/%Y")
            return val
        except ValueError:
            err("Formato invalido. Use DD/MM/AAAA (ej: 24/03/2026)")

def pedir_fecha_reserva(prompt):
    """Pide una fecha de reserva: hoy hasta 31 dias adelante (DD/MM/AAAA)."""
    hoy_ = date.today()
    max_ = hoy_ + timedelta(days=31)
    while True:
        val = input(prompt).strip()
        if not val: val = hoy_col()
        try:
            d = datetime.strptime(val, "%d/%m/%Y").date()
            if d < hoy_:
                err("La fecha no puede ser en el pasado.")
            elif d > max_:
                err(f"Maximo 31 dias adelante ({max_.strftime('%d/%m/%Y')}).")
            else:
                return val
        except ValueError:
            err("Formato invalido. Use DD/MM/AAAA (ej: 24/03/2026)")

def res_crear():
    titulo("Nueva Reserva")
    clis_activos = [c for c in DB["customers"] if c["activo"]]
    if not clis_activos:
        warn("No hay clientes activos registrados.")
        if input("¿Registrar cliente ahora? (s/n): ").lower()=="s": cli_crear()
        else: return
        clis_activos = [c for c in DB["customers"] if c["activo"]]
        if not clis_activos: return
    cli_listar()
    c = bid("customers", pint("ID cliente: ",1))
    if not c: err("No encontrado"); return
    mesa_listar()
    if not DB["tables"]: return
    m = bid("tables", pint("ID mesa: ",1))
    if not m: err("No encontrado"); return
    while True:
        pers = pint(f"Personas (max {m['capacidad']}): ", 1)
        if pers > m["capacidad"]:
            err(f"La mesa #{m['numero']} tiene capacidad maxima de {m['capacidad']} personas.")
        else:
            break
    r = {
        "id":         nid("reservations"),
        "cliente_id": c["id"],
        "cliente":    c["nombre"],
        "mesa_id":    m["id"],
        "mesa_num":   m["numero"],
        "fecha":      pedir_fecha_reserva("Fecha (DD/MM/AAAA): "),
        "hora":       input("Hora (HH:MM): ").strip(),
        "personas":   pers,
        "especiales": input("Solicitudes especiales: ").strip(),
        "estado":     "pendiente",
        "creado":     ahora()
    }
    DB["reservations"].append(r)
    m["estado"] = "reservada"
    ok(f"Reserva #{r['id']} — {c['nombre']} — {r['fecha']} {r['hora']}")

def res_listar():
    if not DB["reservations"]: warn("Sin reservas."); return
    col = {"pendiente":YE,"confirmada":GR,"cancelada":RE,"en_mesa":CY}
    print(f"\n{'ID':<5}{'Cliente':<22}{'Mesa':>5}  {'Fecha':<13}{'Hora':<8}{'Personas':>9}  {'Estado'}")
    print("─"*76)
    for r in DB["reservations"]:
        cc = col.get(r["estado"],WH)
        print(f"{r['id']:<5}{r['cliente']:<22}{r['mesa_num']:>5}  {r['fecha']:<13}{r['hora']:<8}{r['personas']:>9}  {cc}{r['estado']}{R}")

def menu_reservas():
    while True:
        titulo("Gestión de Reservas")
        o = mmenu([("1","Nueva reserva"),("2","Listar reservas"),
                   ("3","Cambiar estado de reserva"),("4","Editar reserva"),
                   ("5","Cancelar reserva"),("0","Volver")])
        if o=="1": res_crear()
        elif o=="2": res_listar(); pausa()
        elif o=="3":
            res_listar()
            if DB["reservations"]:
                r = bid("reservations", pint("ID: ",1))
                if r:
                    sub("Nuevo estado")
                    for i,e in enumerate(EST_RES):
                        print(f"  {CY}{i+1}.{R} {e}")
                    print()
                    nuevo = EST_RES[pint("Estado #: ",1,len(EST_RES))-1]
                    r["estado"] = nuevo
                    m = bid("tables", r["mesa_id"])
                    if m:
                        if nuevo=="cancelada":    m["estado"] = "disponible"
                        elif nuevo=="en_mesa":    m["estado"] = "ocupada"
                        elif nuevo=="confirmada": m["estado"] = "reservada"
                    ok(f"Reserva → {nuevo}")
        elif o=="4":
            res_listar()
            if DB["reservations"]:
                r = bid("reservations", pint("ID: ",1))
                if r:
                    sub(f"Editando Reserva #{r['id']}")
                    oo = mmenu([("1","Cliente"),("2","Mesa"),("3","Fecha"),
                                ("4","Hora"),("5","Personas"),("6","Solicitudes especiales"),
                                ("0","Cancelar")])
                    if oo=="1":
                        cli_listar()
                        if DB["customers"]:
                            c = bid("customers", pint("Nuevo ID cliente: ",1))
                            if c:
                                r["cliente_id"] = c["id"]
                                r["cliente"]    = c["nombre"]
                                ok("Cliente actualizado")
                    elif oo=="2":
                        mesa_listar()
                        m_ant = bid("tables", r["mesa_id"])
                        m_nue = bid("tables", pint("Nuevo ID mesa: ",1))
                        if m_nue:
                            if m_ant: m_ant["estado"] = "disponible"
                            r["mesa_id"]  = m_nue["id"]
                            r["mesa_num"] = m_nue["numero"]
                            m_nue["estado"] = "reservada"
                            ok("Mesa actualizada")
                    elif oo=="3":
                        r["fecha"] = pedir_fecha_reserva("Nueva fecha (DD/MM/AAAA): ")
                        ok("Fecha actualizada")
                    elif oo=="4":
                        r["hora"] = input("Nueva hora (HH:MM): ").strip()
                        ok("Hora actualizada")
                    elif oo=="5":
                        m_r = bid("tables", r["mesa_id"])
                        cap = m_r["capacidad"] if m_r else 999
                        while True:
                            pers = pint(f"Nuevo numero de personas (max {cap}): ", 1)
                            if pers > cap:
                                err(f"La mesa #{r['mesa_num']} tiene capacidad maxima de {cap} personas.")
                            else:
                                r["personas"] = pers
                                ok("Personas actualizado")
                                break
                    elif oo=="6":
                        r["especiales"] = input("Nuevas solicitudes especiales: ").strip()
                        ok("Solicitudes actualizadas")
        elif o=="5":
            res_listar()
            if DB["reservations"]:
                r = bid("reservations", pint("ID: ",1))
                if r and input(f"{RE}¿Cancelar reserva? (s/n): {R}").lower()=="s":
                    r["estado"] = "cancelada"
                    m = bid("tables", r["mesa_id"])
                    if m: m["estado"] = "disponible"
                    ok("Reserva cancelada")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 6 — PEDIDOS (local y domicilio)
# ──────────────────────────────────────────────────────────
EST_PED = ["abierto","en_preparacion","listo","entregado","cancelado"]

def ped_crear():
    titulo("Nuevo Pedido")
    listar_ops("Tipo de pedido", ["Local (mesa)", "Domicilio"])
    tipo = "local" if popc(["1","2"])=="1" else "domicilio"
    p = {
        "id":         nid("orders"),
        "tipo":       tipo,
        "fecha":      ahora(),
        "items":      [],
        "total":      0.0,
        "estado":     "abierto",
        "cliente_id": None,
        "cliente":    "Anónimo",
        "mesa_id":    None,
        "mesa_num":   None,
        "direccion":  "",
        "notas":      input("Notas: ").strip()
    }
    if input("¿Asociar cliente? (s/n): ").lower()=="s":
        clis_activos = [c for c in DB["customers"] if c["activo"]]
        if not clis_activos:
            warn("No hay clientes activos registrados.")
        else:
            cli_listar()
            cid = pint("ID cliente: ",1)
            c = bid("customers", cid)
            if c and c["activo"]:
                p["cliente_id"] = cid
                p["cliente"]    = c["nombre"]
            else:
                warn("Cliente no encontrado o inactivo")
    if tipo=="local":
        mesa_listar()
        m = bid("tables", pint("ID mesa: ",1))
        if m:
            if m["estado"] == "reservada":
                warn(f"La mesa #{m['numero']} esta reservada. No se puede asignar a un pedido directo.")
                return
            p["mesa_id"]  = m["id"]
            p["mesa_num"] = m["numero"]
            m["estado"]   = "ocupada"
    else:
        p["direccion"] = input("Dirección domicilio: ").strip()

    prods = [x for x in DB["dishes"] if x["activo"]]
    if not prods: warn("Sin platos activos. Crea platos en el menu de Platos."); return
    while True:
        plato_listar()
        if input("\n¿Agregar plato al pedido? (s/n): ").lower()!="s": break
        pr = bid("dishes", pint("ID plato: ",1))
        if not pr or not pr["activo"]: err("No disponible"); continue
        if pr["stock"]<=0: warn(f"Sin stock de {pr['nombre']}"); continue
        cant = pint(f"Cantidad (max {pr['stock']}): ",1, pr["stock"])

        # Verificar stock de ingredientes antes de agregar
        hay_stock_ing = True
        for ip in pr["ingredientes"]:
            ing = bid("ingredients", ip["ing_id"])
            if not ing:
                err(f"Ingrediente '{ip['nombre']}' no encontrado en inventario")
                hay_stock_ing = False; break
            necesario = ip["cantidad"] * cant
            if ing["stock"] < necesario:
                err(f"Stock insuficiente de '{ing['nombre']}': necesita {necesario} {ing['unidad']}, disponible {ing['stock']} {ing['unidad']}")
                hay_stock_ing = False; break
        if not hay_stock_ing: continue

        # Descontar ingredientes del stock
        for ip in pr["ingredientes"]:
            ing = bid("ingredients", ip["ing_id"])
            if ing:
                usado = ip["cantidad"] * cant
                ing["stock"] -= usado
                ok(f"    -> {ing['nombre']}: -{usado} {ing['unidad']} (quedan {ing['stock']})")

        nota = input("Nota del item: ").strip()
        sub_t = cant*pr["precio_venta"]
        p["items"].append({"prod_id":pr["id"],"nombre":pr["nombre"],"cant":cant,
                           "pu":pr["precio_venta"],"sub":sub_t,"nota":nota})
        p["total"] += sub_t
        pr["stock"] -= cant
        ok(f"  {cant}x {pr['nombre']} = {pesos(sub_t)}")

    if not p["items"]: warn("Pedido vacío."); return
    DB["orders"].append(p)
    if p["cliente_id"]:
        c = bid("customers", p["cliente_id"])
        if c:
            c["visitas"] += 1
            c["gastado"] += p["total"]
            c["puntos"]  += int(p["total"]/1000)
            cli_nivel(c)
    print(f"\n{B}{GR}Pedido #{p['id']} — Total: {pesos(p['total'])}{R}")
    return p

def ped_listar():
    if not DB["orders"]: warn("Sin pedidos."); return
    col = {"abierto":CY,"en_preparacion":YE,"listo":GR,"entregado":GY,"cancelado":RE}
    print(f"\n{'ID':<5}{'Tipo':<10}{'Cliente':<22}{'Referencia':<26}{'Total':>12}{'Estado':<16}{'Fecha'}")
    print("─"*104)
    for o in DB["orders"]:
        cc  = col.get(o["estado"],WH)
        ref = f"Mesa {o['mesa_num']}" if o["tipo"]=="local" else o["direccion"][:24]
        print(f"{o['id']:<5}{o['tipo']:<10}{o['cliente']:<22}{ref:<26}{pesos(o['total']):>12}  {cc}{o['estado']:<16}{R}{o['fecha']}")

def ped_detalle():
    o = bid("orders", pint("ID pedido: ",1))
    if not o: err("No encontrado"); return
    print(f"\n{B}{'─'*52}{R}")
    print(f"  {CY}Pedido #{o['id']}{R} | {o['tipo'].upper()} | {o['estado'].upper()}")
    print(f"  Cliente : {o['cliente']}")
    print(f"  Fecha   : {o['fecha']}")
    ref = f"Mesa {o['mesa_num']}" if o["tipo"]=="local" else o["direccion"]
    print(f"  Ref     : {ref}")
    if o["notas"]: print(f"  Notas   : {o['notas']}")
    print(f"\n  {'Producto':<26}{'Cant':>5}{'Precio':>12}{'Subtotal':>12}")
    print(f"  {'─'*56}")
    for it in o["items"]:
        print(f"  {it['nombre']:<26}{it['cant']:>5}{pesos(it['pu']):>12}{pesos(it['sub']):>12}")
        if it.get("nota"): print(f"    {GY}↳ {it['nota']}{R}")
    print(f"  {'─'*56}")
    print(f"  {B}{'TOTAL':>43} {pesos(o['total']):>12}{R}")
    print(f"{B}{'─'*52}{R}")

def menu_pedidos():
    while True:
        titulo("Gestión de Pedidos")
        o = mmenu([("1","Nuevo pedido"),("2","Listar pedidos"),
                   ("3","Ver detalle de pedido"),("4","Cambiar estado de pedido"),
                   ("5","Cancelar pedido"),("0","Volver")])
        if o=="1": ped_crear()
        elif o=="2": ped_listar(); pausa()
        elif o=="3": ped_detalle(); pausa()
        elif o=="4":
            ped_listar()
            if DB["orders"]:
                p = bid("orders", pint("ID: ",1))
                if p:
                    sub("Nuevo estado")
                    for i,e in enumerate(EST_PED):
                        print(f"  {CY}{i+1}.{R} {e}")
                    print()
                    nuevo = EST_PED[pint("Estado #: ",1,len(EST_PED))-1]
                    p["estado"] = nuevo
                    if nuevo=="entregado" and p["mesa_id"]:
                        m = bid("tables", p["mesa_id"])
                        if m: m["estado"] = "disponible"
                    ok(f"Pedido #{p['id']} → {nuevo}")
        elif o=="5":
            ped_listar()
            if DB["orders"]:
                p = bid("orders", pint("ID: ",1))
                if p and input(f"{RE}¿Cancelar? (s/n): {R}").lower()=="s":
                    p["estado"] = "cancelado"
                    for it in p["items"]:
                        pr = bid("dishes", it["prod_id"])
                        if pr:
                            pr["stock"] += it["cant"]
                            # Devolver ingredientes al stock
                            for ip in pr["ingredientes"]:
                                ing = bid("ingredients", ip["ing_id"])
                                if ing:
                                    devuelto = ip["cantidad"] * it["cant"]
                                    ing["stock"] += devuelto
                                    ok(f"    -> {ing['nombre']}: +{devuelto} {ing['unidad']} (quedan {ing['stock']})")
                    if p["mesa_id"]:
                        m = bid("tables", p["mesa_id"])
                        if m: m["estado"] = "disponible"
                    ok("Pedido cancelado — stock devuelto")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 7 — EMPLEADOS
# ──────────────────────────────────────────────────────────
ROLES = ["Mesero","Cocinero","Cajero","Administrador","Domiciliario","Auxiliar","Otro"]

def emp_crear():
    titulo("Nuevo Empleado")
    e = {
        "id":        nid("employees"),
        "nombre":    input("Nombre: ").strip(),
        "documento": input("Documento: ").strip(),
        "telefono":  input("Teléfono: ").strip(),
        "email":     input("Email: ").strip(),
        "rol":       "",
        "salario":   0.0,
        "contrato":  "",
        "ingreso":   pedir_fecha_col("Fecha ingreso (DD/MM/AAAA): "),
        "activo":    True
    }
    listar_ops("Rol", ROLES)
    e["rol"]     = ROLES[pint("Rol #: ",1,len(ROLES))-1]
    e["salario"] = pfloat(f"Salario base (min. SMLV={pesos(SMLV)}): ", SMLV)
    listar_ops("Tipo de contrato", ["Termino fijo","Indefinido","Prestacion de servicios"])
    e["contrato"] = {"1":"Termino fijo","2":"Indefinido","3":"Prest. servicios"}[popc(["1","2","3"])]
    DB["employees"].append(e)
    ok(f"Empleado '{e['nombre']}' ID#{e['id']}")
    return e

def emp_listar(activos=True):
    rows = [e for e in DB["employees"] if not activos or e["activo"]]
    if not rows: warn("Sin empleados."); return
    print(f"\n{'ID':<5}{'Nombre':<26}{'Rol':<16}{'Salario Base':>14}   {'Tipo Contrato':<24}{'Estado'}")
    print("─"*94)
    for e in rows:
        st = f"{GR}Activo{R}" if e["activo"] else f"{RE}Inactivo{R}"
        print(f"{e['id']:<5}{e['nombre']:<26}{e['rol']:<16}{pesos(e['salario']):>14}   {e['contrato']:<24}{st}")

def menu_empleados():
    while True:
        titulo("Gestión de Empleados")
        o = mmenu([("1","Nuevo empleado"),("2","Listar empleados"),
                   ("3","Consultar empleado"),("4","Modificar empleado"),
                   ("5","Desactivar / Activar empleado"),
                   ("0","Volver")])
        if o=="1": emp_crear()
        elif o=="2": emp_listar(False); pausa()
        elif o=="3":
            emp_listar()
            if DB["employees"]:
                e = bid("employees", pint("ID: ",1))
                if e:
                    print(f"\n{B}{'─'*42}{R}")
                    for k,v in e.items(): print(f"  {CY}{k:<18}{R} {v}")
                    print(f"{B}{'─'*42}{R}")
                    pausa()
        elif o=="4":
            emp_listar(False)
            if DB["employees"]:
                e = bid("employees", pint("ID: ",1))
                if e:
                    oo = mmenu([("1","Nombre"),("2","Teléfono"),("3","Rol"),
                                ("4","Salario"),("5","Tipo contrato"),("0","Cancelar")])
                    if oo=="1": e["nombre"]    = input("Nuevo nombre: ").strip()
                    elif oo=="2": e["telefono"] = input("Nuevo teléfono: ").strip()
                    elif oo=="3":
                        listar_ops("Rol", ROLES)
                        e["rol"] = ROLES[pint("Rol #: ",1,len(ROLES))-1]
                    elif oo=="4": e["salario"]  = pfloat("Nuevo salario: ", SMLV)
                    elif oo=="5":
                        tc = mmenu([("1","Término fijo"),("2","Indefinido"),
                                    ("3","Prest. servicios"),("0","Cancelar")])
                        if tc!="0":
                            e["contrato"] = {"1":"Término fijo","2":"Indefinido","3":"Prest. servicios"}[tc]
                    if oo!="0": ok("Empleado actualizado")
        elif o=="5":
            emp_listar(False)
            if DB["employees"]:
                e = bid("employees", pint("ID: ",1))
                if e:
                    e["activo"] = not e["activo"]
                    ok(f"Empleado {'activado' if e['activo'] else 'desactivado'}")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 8 — NÓMINA (Normatividad colombiana vigente)
# ──────────────────────────────────────────────────────────
"""
Decreto 1469 y 1470 del 29 de diciembre de 2025:
  SMLV 2026        : $1.750.905  (+23%)
  Aux. transporte  : $  249.095  (+24.5%)  — aplica si salario <= 2 SMLV

DEDUCCIONES empleado:   Salud 4% | Pensión 4%
APORTES empleador:      Salud 8.5% | Pensión 12% | ARL 0.522%
                        Caja 4% | ICBF 3% | SENA 2%
PRESTACIONES:           Cesantías 8.33% | Int.Ces. 1%(mes) | Prima 8.33% | Vacaciones 4.17%
Nota: el auxilio de transporte NO es base para aportes ni prestaciones (Art. 7 Ley 1ª de 1963)
Ref: https://www.mintrabajo.gov.co/atencion-alciudadano/tramites-y-servicios/mi-calculadora
"""

def calc_nomina(sal):
    # Auxilio de transporte: aplica si salario <= 2 SMLV (Decreto 1469/2025)
    aux_transp = AUXILIO_TRANSPORTE if sal <= 2 * SMLV else 0.0

    # Deducciones del empleado (sobre salario base, sin auxilio)
    hs  = sal * 0.04
    hp  = sal * 0.04 if sal >= SMLV else 0.0
    ded = hs + hp
    # El auxilio de transporte se paga completo (no se le descuenta)
    neto = sal - ded + aux_transp

    # Aportes del empleador (base = salario, el auxilio NO es base)
    es  = sal * 0.085
    ep  = sal * 0.12
    arl = sal * 0.00522
    caj = sal * 0.04
    icb = sal * 0.03
    sen = sal * 0.02
    tot_emp = es + ep + arl + caj + icb + sen

    # Prestaciones sociales (base = salario + auxilio transporte para cesantías/prima)
    base_ces = sal + aux_transp   # Art. 249 CST: el aux. sí cuenta para cesantías y prima
    ces      = base_ces * 0.0833
    int_ces  = ces * 0.12         # 12% anual sobre cesantías ≈ 1% mensual
    pri      = base_ces * 0.0833
    vac      = sal * 0.0417       # Vacaciones solo sobre salario (no incluye aux.)
    tot_pres = ces + int_ces + pri + vac

    return {
        "salario_base":            sal,
        "auxilio_transporte":      aux_transp,
        # Empleado
        "ded_salud_emp":           hs,
        "ded_pension_emp":         hp,
        "total_deducciones":       ded,
        "salario_neto":            neto,      # lo que recibe en mano
        # Empleador
        "apt_salud_emp":           es,
        "apt_pension_emp":         ep,
        "arl":                     arl,
        "caja_compensacion":       caj,
        "icbf":                    icb,
        "sena":                    sen,
        "total_aportes_empleador": tot_emp,
        # Prestaciones
        "cesantias":               ces,
        "int_cesantias":           int_ces,
        "prima_servicios":         pri,
        "vacaciones":              vac,
        "total_prestaciones":      tot_pres,
        "costo_total_empleador":   sal + aux_transp + tot_emp + tot_pres
    }

def nom_generar():
    titulo("Generar Nómina")
    emp_listar()
    if not DB["employees"]: return
    e = bid("employees", pint("ID empleado: ",1))
    if not e: err("No encontrado"); return
    per = pedir_periodo_col("Periodo ")
    ex  = next((n for n in DB["payrolls"] if n["emp_id"]==e["id"] and n["periodo"]==per), None)
    if ex:
        warn(f"Ya existe nómina de {e['nombre']} para {per}")
        if input("¿Recalcular? (s/n): ").lower()!="s": return
        DB["payrolls"].remove(ex)
    c = calc_nomina(e["salario"])
    n = {"id":nid("payrolls"),"emp_id":e["id"],"empleado":e["nombre"],"rol":e["rol"],"periodo":per,"generado":hoy(),**c}
    DB["payrolls"].append(n)
    # Imprimir comprobante
    W = 56
    print(f"\n{B}{CY}{'═'*W}{R}")
    print(f"{B}{CY}  COMPROBANTE DE NÓMINA — Dec.1469-1470/2025{R}")
    print(f"{B}{CY}  {e['nombre'].upper()} — {e['rol']} — {per}{R}")
    print(f"{B}{CY}{'═'*W}{R}")
    print(f"\n  {'Salario Base (SMLV 2026)':<36} {pesos(c['salario_base']):>14}")
    if c['auxilio_transporte'] > 0:
        print(f"  {'+ Auxilio de transporte':<36} {pesos(c['auxilio_transporte']):>14}")
    print(f"\n  {B}DEDUCCIONES DEL EMPLEADO{R}")
    print(f"  {'  Salud (4%)':<36} -{pesos(c['ded_salud_emp'])[1:]:>13}")
    print(f"  {'  Pensión (4%)':<36} -{pesos(c['ded_pension_emp'])[1:]:>13}")
    print(f"  {'  Total deducciones':<36} -{pesos(c['total_deducciones'])[1:]:>13}")
    print(f"\n  {B}{GR}{'NETO RECIBIDO (incluye aux.transp.)':<36} {pesos(c['salario_neto']):>14}{R}")
    print(f"\n  {B}APORTES DEL EMPLEADOR (sobre salario base){R}")
    print(f"  {'  Salud (8.5%)':<36} {pesos(c['apt_salud_emp']):>14}")
    print(f"  {'  Pensión (12%)':<36} {pesos(c['apt_pension_emp']):>14}")
    print(f"  {'  ARL (0.522%)':<36} {pesos(c['arl']):>14}")
    print(f"  {'  Caja Compensación (4%)':<36} {pesos(c['caja_compensacion']):>14}")
    print(f"  {'  ICBF (3%)':<36} {pesos(c['icbf']):>14}")
    print(f"  {'  SENA (2%)':<36} {pesos(c['sena']):>14}")
    print(f"\n  {B}PRESTACIONES SOCIALES (base: sal + aux){R}")
    print(f"  {'  Cesantías (8.33%)':<36} {pesos(c['cesantias']):>14}")
    print(f"  {'  Intereses cesantías (1%/mes)':<36} {pesos(c['int_cesantias']):>14}")
    print(f"  {'  Prima de servicios (8.33%)':<36} {pesos(c['prima_servicios']):>14}")
    print(f"  {'  Vacaciones (4.17% s/salario)':<36} {pesos(c['vacaciones']):>14}")
    print(f"\n  {B}{RE}{'COSTO TOTAL EMPLEADOR':<36} {pesos(c['costo_total_empleador']):>14}{R}")
    print(f"{B}{CY}{'═'*W}{R}")
    ok(f"Nómina guardada ID#{n['id']}")

def nom_periodo():
    per = pedir_periodo_col("Periodo ")
    ns  = [n for n in DB["payrolls"] if n["periodo"]==per]
    if not ns: warn(f"Sin nóminas para {per}"); return
    print(f"\n{B}Nómina período {CY}{per}{R}")
    for n in ns:
        print(f"  {n['empleado']:<26}{n['rol']:<16}Neto: {pesos(n['salario_neto'])}")
    print(f"\n  {B}Total neto    : {GR}{pesos(sum(n['salario_neto'] for n in ns))}{R}")
    print(f"  {B}Costo empleador: {RE}{pesos(sum(n['costo_total_empleador'] for n in ns))}{R}")

def nom_listar():
    if not DB["payrolls"]: warn("Sin nóminas."); return
    print(f"\n{'ID':<5}{'Empleado':<26}{'Período':<9}{'Sal.Base':>14}{'Neto':>14}{'Costo Total':>14}")
    print("─"*82)
    for n in DB["payrolls"]:
        print(f"{n['id']:<5}{n['empleado']:<26}{n['periodo']:<9}{pesos(n['salario_base']):>14}{pesos(n['salario_neto']):>14}{pesos(n['costo_total_empleador']):>14}")

def menu_nomina():
    while True:
        titulo("Gestión de Nómina")
        o = mmenu([("1","Generar nómina de empleado"),("2","Consultar nómina por período"),
                   ("3","Historial de nóminas"),("4","Eliminar nómina"),
                   ("0","Volver")])
        if o=="1": nom_generar()
        elif o=="2": nom_periodo(); pausa()
        elif o=="3": nom_listar(); pausa()
        elif o=="4":
            nom_listar()
            if DB["payrolls"]:
                n = bid("payrolls", pint("ID: ",1))
                if n and input(f"{RE}¿Eliminar? (s/n): {R}").lower()=="s":
                    DB["payrolls"].remove(n); ok("Eliminado")
        elif o=="0": break
        guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 9 — DASHBOARD / ESTADÍSTICAS
# ──────────────────────────────────────────────────────────
def stats(pedidos):
    tots = [p["total"] for p in pedidos if p["estado"]!="cancelado"]
    if not tots: return {"n":0,"suma":0,"prom":0,"mx":0,"mn":0,"med":0,"std":0}
    tots.sort(); n=len(tots); s=sum(tots)
    prom=s/n; mn=tots[0]; mx=tots[-1]
    med = tots[n//2] if n%2 else (tots[n//2-1]+tots[n//2])/2
    std = (sum((x-prom)**2 for x in tots)/n)**0.5
    return {"n":n,"suma":s,"prom":prom,"mx":mx,"mn":mn,"med":med,"std":std}

def dashboard():
    titulo("Dashboard — Análisis Estadístico")
    print("Período: 1.Hoy  2.Semana  3.Mes  4.Año  5.Todo")
    o = popc(["1","2","3","4","5"])
    hoy_ = date.today()
    labels = {
        "1": f"Hoy ({hoy_})",
        "2": "Última semana",
        "3": f"Mes actual ({hoy_.strftime('%B %Y')})",
        "4": f"Año {hoy_.year}",
        "5": "Histórico completo"
    }
    desde = {
        "1": hoy_,
        "2": hoy_ - timedelta(days=7),
        "3": hoy_.replace(day=1),
        "4": hoy_.replace(month=1, day=1),
        "5": date(2000, 1, 1)
    }[o]

    def en_per(f):
        try: return date.fromisoformat(str(f)[:10]) >= desde
        except: return o=="5"

    peds = [p for p in DB["orders"] if en_per(p["fecha"])]
    st   = stats(peds)

    print(f"\n{B}{CY}  PERÍODO: {labels[o]}{R}\n")

    sub("Resumen de Pedidos")
    print(f"  {'Total pedidos:':<32} {B}{st['n']}{R}")
    print(f"  {'Ingresos:':<32} {B}{GR}{pesos(st['suma'])}{R}")
    print(f"  {'Ticket promedio:':<32} {pesos(st['prom'])}")
    print(f"  {'Desv. estándar:':<32} {pesos(st['std'])}")
    print(f"  {'Ticket máximo:':<32} {pesos(st['mx'])}")
    print(f"  {'Ticket mínimo:':<32} {pesos(st['mn'])}")
    print(f"  {'Mediana:':<32} {pesos(st['med'])}")

    loc    = [p for p in peds if p["tipo"]=="local"    and p["estado"]!="cancelado"]
    dom    = [p for p in peds if p["tipo"]=="domicilio" and p["estado"]!="cancelado"]
    canc   = [p for p in peds if p["estado"]=="cancelado"]
    abiertos = [p for p in peds if p["estado"]=="abierto"]
    print(f"  {'Pedidos locales:':<32} {len(loc)}")
    print(f"  {'Pedidos domicilio:':<32} {len(dom)}")
    print(f"  {'Cancelados:':<32} {RE}{len(canc)}{R}")
    print(f"  {'Abiertos (en curso):':<32} {CY}{len(abiertos)}{R}")

    sub("Top 5 Productos más Vendidos")
    cnt = defaultdict(lambda: {"u":0,"ing":0})
    for p in peds:
        if p["estado"]!="cancelado":
            for it in p["items"]:
                cnt[it["nombre"]]["u"]   += it["cant"]
                cnt[it["nombre"]]["ing"] += it["sub"]
    if cnt:
        top = sorted(cnt.items(), key=lambda x:x[1]["u"], reverse=True)[:5]
        mx_ = top[0][1]["u"] if top else 1
        for nm,(d) in top:
            print(f"  {nm:<28} {barra(d['u'],mx_,18)} {d['u']:>4} uds — {pesos(d['ing'])}")
    else: print("  Sin datos")

    sub("Distribución por Estado")
    ec = defaultdict(int)
    for p in peds: ec[p["estado"]] += 1
    tot_ = len(peds) or 1
    for est, cnt_ in sorted(ec.items()):
        print(f"  {est:<18} {barra(cnt_, max(ec.values(),default=1),18)} {cnt_:>3} ({cnt_/tot_*100:.1f}%)")

    sub("Inventario")
    pact = [p for p in DB["dishes"] if p["activo"]]
    iact = [i for i in DB["ingredients"] if i["activo"]]
    print(f"  {'Platos activos:':<32} {len(pact)}")
    print(f"  {'Ingredientes activos:':<32} {len(iact)}")

    sub("Nomina del Periodo")
    if o=="1":
        ns = [n for n in DB["payrolls"] if n["periodo"]==hoy_.strftime("%m/%y")]
    elif o=="2":
        semana = [(hoy_ - timedelta(days=i)).strftime("%m/%y") for i in range(7)]
        ns = [n for n in DB["payrolls"] if n["periodo"] in semana]
    elif o=="3":
        ns = [n for n in DB["payrolls"] if n["periodo"]==hoy_.strftime("%m/%y")]
    elif o=="4":
        pfx = hoy_.strftime("/%y")
        ns  = [n for n in DB["payrolls"] if n["periodo"].endswith(pfx)]
    else:
        ns = list(DB["payrolls"])
    if ns:
        print(f"  {'Empleados con nómina:':<32} {len(ns)}")
        print(f"  {'Total neto:':<32} {GR}{pesos(sum(n['salario_neto'] for n in ns))}{R}")
        print(f"  {'Costo empleador:':<32} {RE}{pesos(sum(n['costo_total_empleador'] for n in ns))}{R}")
    else:
        print(f"  {GY}Sin nominas registradas en este periodo.{R}")

    sub("Top 5 Clientes")
    top_c = sorted([c for c in DB["customers"] if c["activo"]], key=lambda x:x["gastado"], reverse=True)[:5]
    nc = {"VIP":MA,"Oro":YE,"Plata":CY,"Bronce":WH}
    if top_c:
        mx_c = top_c[0]["gastado"] if top_c else 1
        for c in top_c:
            cc = nc.get(c["nivel"],WH)
            print(f"  {c['nombre']:<26} {barra(c['gastado'],mx_c,14)} {pesos(c['gastado'])} {cc}{c['nivel']}{R}")
    else: print("  Sin datos")

# ──────────────────────────────────────────────────────────
#  MÓDULO 10 — SIMULACIÓN
# ──────────────────────────────────────────────────────────
_NOMBRES = ["Carlos López","María García","Juan Pérez","Ana Martínez","Pedro Rodríguez",
            "Laura Sánchez","Diego Ramírez","Sofía Torres","Andrés Herrera","Valentina Castro",
            "Camilo Vargas","Isabella Moreno","Sebastián Rojas","Daniela Mendoza","Felipe Restrepo",
            "Natalia Cárdenas","Julián Ospina","Paola Quintero","Rodrigo Mendoza","Luisa Fernández"]

# (nombre, categoria, unidad, precio)
_INGS_SIM = [
    ("Arroz","granos","kg",3500),("Pollo","carnes","kg",12000),("Carne de res","carnes","kg",18000),
    ("Papa","verduras","kg",2000),("Tomate","verduras","kg",3000),("Cebolla","verduras","kg",2500),
    ("Aceite vegetal","aceites","litro",8000),("Leche","lacteos","litro",3500),
    ("Crema de leche","lacteos","litro",6000),("Naranja","frutas","kg",4000),
    ("Limon","frutas","kg",3000),("Sal","condimentos","gramos",500),
    ("Pimienta","condimentos","gramos",8000),("Maiz","granos","kg",2800),
    ("Cerveza","granos","unidad",3500),
]
# (nombre, precio_venta, [indices de ingredientes 0-based])
_PLATOS_SIM = [
    ("Bandeja Paisa",32000,[0,1,3,6]),("Ajiaco Bogotano",28000,[0,1,3,4]),
    ("Sancocho de Gallina",25000,[1,3,4,5]),("Empanadas x3",8000,[0,2,5,11]),
    ("Patacones con Hogao",12000,[13,4,5,6]),("Arroz con Pollo",22000,[0,1,6,11]),
    ("Churrasco a la Parrilla",38000,[2,6,11,12]),("Ensalada Cesar",15000,[4,5,8]),
    ("Flan de Caramelo",9000,[7,8]),("Tres Leches",10000,[7,8]),
    ("Limonada Natural",7000,[10,11]),("Jugo Natural",8000,[9,11]),
    ("Cafe Americano",5000,[11]),("Agua Mineral",3000,[11]),
    ("Cerveza Nacional",6000,[14]),
]
_PROVS = [
    ("Alimentos del Valle","Roberto Mora","3101234567"),
    ("Carnes Premium S.A.S","Claudia Ríos","3209876543"),
    ("Frutas La Cosecha","Mario Nieto","3154567890"),
]
_EMPS  = [
    ("Jorge Peña","Mesero"),("Rosa Quintero","Mesero"),
    ("Edwin Caro","Cocinero"),("Fernanda Díaz","Cajero"),("Luis Ortega","Domiciliario"),
]

def simular():
    random.seed()
    titulo("Simulación del Restaurante")
    dias = pint("¿Cuántos días simular? (1-30): ",1,30)
    print(f"\n{CY}Generando datos base...{R}")

    # Ingredientes
    existentes_ing = {i["nombre"] for i in DB["ingredients"]}
    nuevos_ing = 0
    for nm,cat,und,precio in _INGS_SIM:
        if nm not in existentes_ing:
            DB["_ids"]["ingredients"] += 1
            DB["ingredients"].append({"id":DB["_ids"]["ingredients"],"nombre":nm,"categoria":cat,
                "unidad":und,"precio":precio,"stock":round(random.uniform(5,50),2),"activo":True,"creado":hoy()})
            nuevos_ing += 1
    if nuevos_ing: ok(f"  {nuevos_ing} ingredientes agregados")

    # Platos (referencian ingredientes por posicion en _INGS_SIM)
    existentes_plato = {d["nombre"] for d in DB["dishes"]}
    nuevos_plato = 0
    ings_sim_ids = [next((i["id"] for i in DB["ingredients"] if i["nombre"]==nm), None) for nm,_,_,_ in _INGS_SIM]
    for nm,precio_v,idxs in _PLATOS_SIM:
        if nm not in existentes_plato:
            DB["_ids"]["dishes"] += 1
            comp = []
            for idx in idxs:
                ing_id = ings_sim_ids[idx] if idx < len(ings_sim_ids) else None
                nm_ing = _INGS_SIM[idx][0] if idx < len(_INGS_SIM) else ""
                if ing_id: comp.append({"ing_id":ing_id,"nombre":nm_ing,"cantidad":round(random.uniform(0.1,0.5),2)})
            DB["dishes"].append({"id":DB["_ids"]["dishes"],"nombre":nm,"precio_venta":precio_v,
                "stock":random.randint(20,80),"activo":True,"creado":hoy(),"ingredientes":comp})
            nuevos_plato += 1
    if nuevos_plato: ok(f"  {nuevos_plato} platos agregados")

    if not DB["suppliers"]:
        for nm,ct,tel in _PROVS:
            DB["_ids"]["suppliers"] += 1
            DB["suppliers"].append({"id":DB["_ids"]["suppliers"],"nombre":nm,"contacto":ct,
                "telefono":tel,"email":"info@prov.com","productos":"Varios","activo":True,"creado":hoy()})
        ok(f"  {len(_PROVS)} proveedores")

    mesas_act = len([m for m in DB["tables"] if m["activo"]])
    if mesas_act < 10:
        nums = {m["numero"] for m in DB["tables"]}
        num = max(nums, default=0)
        faltan = 10 - mesas_act
        for _ in range(faltan):
            num += 1
            while num in nums: num += 1
            nums.add(num)
            DB["_ids"]["tables"] += 1
            DB["tables"].append({"id":DB["_ids"]["tables"],"numero":num,
                "capacidad":random.choice([2,4,4,6]),"ubicacion":random.choice(["Salon","Terraza","VIP"]),
                "estado":"disponible","activo":True})
        ok(f"  {faltan} mesas agregadas (total activas: 10)")

    if not DB["employees"]:
        for nm,rol in _EMPS:
            DB["_ids"]["employees"] += 1
            sal = SMLV * random.uniform(1.0, 1.8)
            DB["employees"].append({"id":DB["_ids"]["employees"],"nombre":nm,
                "documento":str(random.randint(10000000,99999999)),
                "telefono":f"31{random.randint(10000000,99999999)}",
                "email":f"{nm.split()[0].lower()}@dondesiempre.com","rol":rol,"salario":round(sal,-3),
                "contrato":random.choice(["Indefinido","Termino fijo","Prest. servicios"]),"ingreso":hoy(),"activo":True})
        ok(f"  {len(_EMPS)} empleados")

    existentes = {c["nombre"] for c in DB["customers"]}
    nuevos = 0
    for nm in _NOMBRES:
        if nm not in existentes:
            DB["_ids"]["customers"] += 1
            DB["customers"].append({"id":DB["_ids"]["customers"],"nombre":nm,
                "telefono":f"31{random.randint(10000000,99999999)}",
                "email":f"{nm.split()[0].lower()}{random.randint(1,99)}@mail.com",
                "direccion":f"Calle {random.randint(1,100)} #{random.randint(1,50)}-{random.randint(1,99)}",
                "visitas":0,"gastado":0.0,"puntos":0,"nivel":"Bronce","activo":True,"creado":hoy()})
            nuevos += 1
    if nuevos: ok(f"  {nuevos} clientes")

    print(f"\n{CY}Simulando {dias} días...{R}")
    prods = [p for p in DB["dishes"] if p["activo"]]
    clis  = DB["customers"]
    mesas = [m for m in DB["tables"] if m["activo"]]
    total_ped = 0; total_ing = 0.0

    for d in range(dias):
        fecha_d = (date.today() - timedelta(days=dias-d-1)).isoformat()
        n_ped   = random.randint(8, 25)
        for _ in range(n_ped):
            tipo  = random.choices(["local","domicilio"], weights=[65,35])[0]
            cli   = random.choice(clis)
            items = []
            total = 0.0
            for pr in random.sample(prods, min(random.randint(1,4), len(prods))):
                cant  = random.randint(1,3)
                sub_t = cant * pr["precio_venta"]
                items.append({"prod_id":pr["id"],"nombre":pr["nombre"],"cant":cant,"pu":pr["precio_venta"],"sub":sub_t,"nota":""})
                total += sub_t
            estado = random.choices(["entregado","cancelado","abierto"], weights=[82,10,8])[0]
            m = random.choice(mesas) if tipo=="local" else None
            DB["_ids"]["orders"] += 1
            DB["orders"].append({
                "id":DB["_ids"]["orders"],"tipo":tipo,
                "fecha":f"{fecha_d} {random.randint(10,22):02d}:{random.randint(0,59):02d}",
                "items":items,"total":total,"estado":estado,
                "cliente_id":cli["id"],"cliente":cli["nombre"],
                "mesa_id":m["id"] if m else None,"mesa_num":m["numero"] if m else None,
                "direccion":cli.get("direccion","") if tipo=="domicilio" else "","notas":""
            })
            if estado=="entregado":
                cli["visitas"] += 1; cli["gastado"] += total
                cli["puntos"]  += int(total/1000)
                cli_nivel(cli)
                total_ing += total; total_ped += 1
        print(f"  Día {d+1}/{dias} — {fecha_d} — {n_ped} pedidos")

    # Generar reservas futuras (entre 3 y 8 reservas distribuidas en los próximos dias)
    mesas_disp = [m for m in DB["tables"] if m["activo"] and m["estado"]=="disponible"]
    clis_act   = [c for c in DB["customers"] if c["activo"]]
    n_res = random.randint(3, 8)
    nuevas_res = 0
    horas_res  = ["12:00","13:00","14:00","19:00","20:00","20:30","21:00"]
    esp_opciones = ["","","","Cumpleanos","Mesa romantica","Sin gluten","Silla para nino","Alergico a lacteos"]
    for _ in range(n_res):
        if not mesas_disp or not clis_act: break
        cli = random.choice(clis_act)
        m   = random.choice(mesas_disp)
        dias_adelante = random.randint(1, 15)
        fecha_res = (date.today() + timedelta(days=dias_adelante)).strftime("%d/%m/%Y")
        pers = random.randint(1, m["capacidad"])
        DB["_ids"]["reservations"] += 1
        DB["reservations"].append({
            "id":         DB["_ids"]["reservations"],
            "cliente_id": cli["id"],
            "cliente":    cli["nombre"],
            "mesa_id":    m["id"],
            "mesa_num":   m["numero"],
            "fecha":      fecha_res,
            "hora":       random.choice(horas_res),
            "personas":   pers,
            "especiales": random.choice(esp_opciones),
            "estado":     random.choice(["pendiente","confirmada"]),
            "creado":     ahora()
        })
        nuevas_res += 1
    if nuevas_res: ok(f"  {nuevas_res} reservas generadas")

    # Generar pedidos a proveedores (entre 2 y 5)
    ings_act = [i for i in DB["ingredients"] if i["activo"]]
    nuevas_comp = 0
    if DB["suppliers"] and ings_act:
        n_comp = random.randint(2, 5)
        for _ in range(n_comp):
            pv = random.choice(DB["suppliers"])
            dias_atras = random.randint(0, 6)
            fecha_c = (date.today() - timedelta(days=dias_atras)).strftime("%Y-%m-%d")
            DB["_ids"]["purchases"] += 1
            comp = {
                "id":               DB["_ids"]["purchases"],
                "proveedor_id":     pv["id"],
                "proveedor":        pv["nombre"],
                "fecha":            fecha_c,
                "total":            0.0,
                "estado":           "",
                "notas":            "",
                "fecha_recepcion":  "",
                "items":            []
            }
            usados = set()
            n_items = random.randint(2, 5)
            for _ in range(n_items):
                ing = random.choice(ings_act)
                if ing["id"] in usados: continue
                usados.add(ing["id"])
                cant  = round(random.uniform(5.0, 50.0), 1)
                sub_t = cant * ing["precio"]
                comp["items"].append({"prod_id":ing["id"],"nombre":ing["nombre"],
                                      "cant":cant,"pu":ing["precio"],"sub":sub_t})
                comp["total"] += sub_t
            if not comp["items"]:
                DB["_ids"]["purchases"] -= 1
                continue
            # 70% recibido, 30% pendiente
            if random.random() < 0.7:
                comp["estado"] = "recibido"
                comp["fecha_recepcion"] = fecha_c
                for it in comp["items"]:
                    ing_obj = bid("ingredients", it["prod_id"])
                    if ing_obj: ing_obj["stock"] += it["cant"]
            else:
                comp["estado"] = "pendiente"
            DB["purchases"].append(comp)
            nuevas_comp += 1
    if nuevas_comp: ok(f"  {nuevas_comp} pedidos a proveedores generados")

    print(f"\n{B}{GR}Simulación completada:{R}")
    print(f"  Pedidos entregados: {total_ped}")
    print(f"  Ingresos simulados: {pesos(total_ing)}")
    print(f"  Reservas generadas: {nuevas_res}")
    print(f"  Pedidos a proveedores: {nuevas_comp}")
    guardar()

# ──────────────────────────────────────────────────────────
#  MÓDULO 11 — PLANES DE PROMOCIÓN
# ──────────────────────────────────────────────────────────
def promociones():
    titulo("Planes de Promoción — Clientes Frecuentes")
    activos = [c for c in DB["customers"] if c["activo"] and c["visitas"]>0]
    if not activos: warn("Sin clientes con historial."); return

    vip   = [c for c in activos if c["nivel"]=="VIP"]
    oro   = [c for c in activos if c["nivel"]=="Oro"]
    plata = [c for c in activos if c["nivel"]=="Plata"]
    bron  = [c for c in activos if c["nivel"]=="Bronce"]

    print(f"\n{B}{MA}{'─'*58}{R}")
    print(f"{B}{MA}  SEGMENTACIÓN DE CLIENTES{R}")
    print(f"{B}{MA}{'─'*58}{R}")
    print(f"  {MA}VIP   {R}({len(vip):>3})  Gasto > $5.000.000")
    print(f"  {YE}Oro   {R}({len(oro):>3})  Gasto $2.000.000 – $5.000.000")
    print(f"  {CY}Plata {R}({len(plata):>3})  Gasto $500.000 – $2.000.000")
    print(f"  {WH}Bronce{R}({len(bron):>3})  Gasto < $500.000")

    print(f"\n{B}{YE}  PLANES SUGERIDOS{R}")
    print(f"{'─'*58}")

    print(f"\n  {MA}★ VIP EXCLUSIVO{R}")
    print(f"  ├ 15% descuento permanente en todos los pedidos")
    print(f"  ├ Reserva prioritaria — confirmacion en 1 hora")
    print(f"  ├ Menu degustacion gratis en cumpleanos")
    print(f"  └ Invitacion a eventos y maridajes especiales")
    if vip:
        print(f"\n  Clientes VIP ({len(vip)}):")
        for c in vip: print(f"    • {c['nombre']:<24} {pesos(c['gastado'])}  {c['visitas']} visitas")
    else:
        print(f"  {GY}  (sin clientes en este nivel aun){R}")

    print(f"\n  {YE}★ PROGRAMA ORO{R}")
    print(f"  ├ 10% descuento martes y miercoles")
    print(f"  ├ Postre gratis en pedidos > $80.000")
    print(f"  └ Puntos dobles los fines de semana")
    if oro:
        print(f"\n  Clientes Oro ({len(oro)}):")
        for c in oro: print(f"    • {c['nombre']:<24} {pesos(c['gastado'])}  {c['visitas']} visitas")
    else:
        print(f"  {GY}  (sin clientes en este nivel aun){R}")

    print(f"\n  {CY}★ PROGRAMA PLATA{R}")
    print(f"  ├ 5% descuento comprando 3 veces por semana")
    print(f"  └ Bebida gratis en pedidos > $60.000")
    if plata:
        print(f"\n  Clientes Plata ({len(plata)}):")
        for c in plata: print(f"    • {c['nombre']:<24} {pesos(c['gastado'])}  {c['visitas']} visitas")
    else:
        print(f"  {GY}  (sin clientes en este nivel aun){R}")

    print(f"\n  {WH}★ BIENVENIDA (Bronce){R}")
    print(f"  ├ 10% descuento en la segunda visita")
    print(f"  └ Entrada gratis en el primer pedido del mes")
    if bron:
        print(f"\n  Clientes Bronce ({len(bron)}):")
        for c in bron: print(f"    • {c['nombre']:<24} {pesos(c['gastado'])}  {c['visitas']} visitas")
    else:
        print(f"  {GY}  (sin clientes en este nivel aun){R}")

    print(f"\n  {B}OFERTA REACTIVACIÓN{R}")
    print(f"  └ '¡Te extrañamos! 20% OFF en tu próxima visita'")

    top_g = sorted(activos, key=lambda x:x["gastado"], reverse=True)[:5]
    top_v = sorted(activos, key=lambda x:x["visitas"], reverse=True)[:5]
    nc = {"VIP":MA,"Oro":YE,"Plata":CY,"Bronce":WH}

    print(f"\n{B}  TOP 5 — MAYOR GASTO:{R}")
    for i,c in enumerate(top_g,1):
        cc = nc.get(c["nivel"],WH)
        print(f"  {i}. {c['nombre']:<24} {cc}{c['nivel']:<6}{R}  {pesos(c['gastado'])}  {c['visitas']} visitas")

    print(f"\n{B}  TOP 5 — MÁS FRECUENTES:{R}")
    for i,c in enumerate(top_v,1):
        print(f"  {i}. {c['nombre']:<24} {c['visitas']:>4} visitas  {pesos(c['gastado'])}")

# ──────────────────────────────────────────────────────────
#  MENÚ PRINCIPAL
# ──────────────────────────────────────────────────────────
def banner():
    limpiar()
    print(f"{B}{CY}")
    print("  ╔══════════════════════════════════════════════════════╗")
    print("  ║             RESTAURANTE «DONDE SIEMPRE»              ║")
    print("  ║              Sistema de Gestión — PGSR v1.0          ║")
    print("  ╠══════════════════════════════════════════════════════╣")
    print("  ║            Universidad Popular del Cesar             ║")
    print("  ║       Facultad de Ingenierías y Tecnológicas         ║")
    print("  ║     Ingeniería de Sistemas — Estructura de Datos     ║")
    print("  ║             Parcial 1 — Grupo 06 — 2026              ║")
    print("  ╚══════════════════════════════════════════════════════╝")
    print(f"{R}")

def main():
    banner()
    print(f"  {YE}¿Cargar datos guardados? (s/n): {R}", end="")
    if input().lower()=="s":
        cargar()
    else:
        print("  Iniciando con datos vacíos.")
    pausa()

    MENU = {
        "1":  ("Ingredientes",                      menu_ingredientes),
        "2":  ("Platos",                            menu_platos),
        "3":  ("Proveedores & Pedidos",             menu_proveedores),
        "4":  ("Mesas",                             menu_mesas),
        "5":  ("Clientes",                          menu_clientes),
        "6":  ("Reservas & Solicitudes Especiales", menu_reservas),
        "7":  ("Pedidos (Local & Domicilio)",       menu_pedidos),
        "8":  ("Empleados",                         menu_empleados),
        "9":  ("Nómina",                            menu_nomina),
        "10": ("Dashboard & Estadísticas",          lambda: (dashboard(), pausa())),
        "11": ("Simulación del Restaurante",        lambda: (simular(), pausa())),
        "12": ("Planes de Promoción",               lambda: (promociones(), pausa())),
        "13": ("Guardar datos",                     lambda: (guardar(), pausa())),
    }

    while True:
        banner()
        print(f"  {B}MENÚ PRINCIPAL{R}\n")
        for k,(label,_) in MENU.items():
            print(f"  {CY}{k:>2}.{R} {label}")
        print(f"\n  {RE} 0.{R} Salir\n")
        opc = input(f"  {B}Seleccione: {R}").strip()
        if opc == "0":
            print(f"\n{CY}Guardando...{R}")
            guardar()
            print(f"\n{B}{GR}¡Hasta luego! «Donde Siempre»{R}\n")
            break
        elif opc in MENU:
            MENU[opc][1]()
        else:
            err("Opción inválida"); pausa()

if __name__ == "__main__":
    main()
