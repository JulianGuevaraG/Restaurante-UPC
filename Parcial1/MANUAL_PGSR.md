# Manual de Usuario
# Sistema de Gestión de Restaurante «Donde Siempre» — PGSR v1.0

---

**Institución:** Universidad Popular del Cesar
**Facultad:** Ingeniería de Sistemas
**Asignatura:** Estructura de Datos
**Parcial:** 1 — Grupo 06 — 2026

---

## 1. Descripción General

El **PGSR (Programa de Gestión del Sistema de Restaurante)** es un sistema de consola para la administración integral del restaurante «Donde Siempre». Permite gestionar inventario, proveedores, mesas, clientes, reservas, pedidos, empleados, nómina y planes de fidelización.

El sistema existe en **dos versiones equivalentes**:

| Versión | Archivo | Lenguaje | Compilador / Intérprete |
|---|---|---|---|
| Python | `Parcial_Py/restaurante.py` | Python 3.x | Python 3.14+ |
| C++ | `Parcial_C++/restaurante.cpp` | C++17 | TDM-GCC 10.3.0 |

Ambas versiones comparten la misma lógica de negocio, los mismos archivos `.dat` de persistencia y el mismo comportamiento de interfaz.

El sistema está organizado en **3 módulos de gestión interna** (ingredientes, platos y proveedores) que alimentan al resto de las operaciones del restaurante.

---

## 2. Requisitos y Ejecución

### Versión Python

**Requisitos:**
- Python 3.10 o superior instalado en `C:\Python314\`
- Windows PowerShell o terminal compatible con colores ANSI

**Cómo ejecutar:**
```powershell
python "c:\Users\ASUS\Documents\UPC\4to Semestre\Estructura de Datos\Parcial1\Parcial_Py\restaurante.py"
```

> El script automáticamente ajusta su directorio de trabajo a la carpeta `Parcial_Py\`, por lo que los archivos `.dat` siempre se guardan en el lugar correcto sin importar desde dónde se ejecute.

### Versión C++

**Requisitos:**
- TDM-GCC 10.3.0 instalado en `C:\TDM-GCC-64\`
- Dev-C++ configurado con el compilador TDM-GCC 10.3.0 y flag `-std=c++17`

**Cómo compilar y ejecutar desde terminal:**
```bash
g++ -std=c++17 -o restaurante.exe restaurante.cpp
restaurante.exe
```

---

## 3. Persistencia de Datos

El sistema guarda y carga los datos en archivos `.dat` con formato de texto plano delimitado por `|`. Esto permite que ambas versiones compartan los mismos archivos de datos.

### Archivos generados

| Archivo | Contenido |
|---|---|
| `ids.dat` | Contadores de IDs por entidad (formato etiquetado) |
| `ingredientes.dat` | Ingredientes / materia prima |
| `platos.dat` | Platos del menú con composición de ingredientes |
| `prov.dat` | Proveedores registrados |
| `compras.dat` | Pedidos realizados a proveedores |
| `mesas.dat` | Mesas del restaurante |
| `clientes.dat` | Clientes registrados |
| `reservas.dat` | Reservas de mesas |
| `pedidos.dat` | Pedidos locales y a domicilio |
| `empleados.dat` | Personal del restaurante |
| `nominas.dat` | Registros de nómina generados |

### Formato del archivo `ids.dat`

```
ingredientes:15|platos:15|proveedores:3|compras:2|mesas:10|clientes:20|reservas:4|pedidos:61|empleados:5|nominas:2
```

Cada campo indica cuántos registros se han creado de esa entidad (usado para asignar IDs únicos consecutivos).

### Codificación especial

Los campos que contienen los caracteres `|` o `~` son escapados automáticamente para no romper el formato:
- `|` → `\x01`
- `~` → `\x02`

Los ítems de un pedido o compra se separan con `~` y sus campos internos con `;`.

---

## 4. Menú Principal

Al iniciar el sistema se pregunta si se desean cargar los datos guardados anteriormente:

```
¿Cargar datos guardados? (s/n):
```

Si se responde `s`, se cargan todos los archivos `.dat`. Si se responde `n`, el sistema inicia con la base de datos vacía.

El menú principal presenta las siguientes opciones:

```
  1.  Ingredientes
  2.  Platos
  3.  Proveedores & Pedidos a Proveedor
  4.  Mesas
  5.  Clientes
  6.  Reservas & Solicitudes Especiales
  7.  Pedidos (Local & Domicilio)
  8.  Empleados
  9.  Nómina
 10.  Dashboard & Estadísticas
 11.  Simulación del Restaurante
 12.  Planes de Promoción
 13.  Guardar datos
  0.  Salir
```

> Al salir (opción `0`) los datos se guardan automáticamente.

---

## 5. Módulo 1 — Ingredientes

Gestiona la materia prima del restaurante (ingredientes de cocina).

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nuevo ingrediente |
| 2 | Listar todos |
| 3 | Modificar ingrediente |
| 4 | Activar / Desactivar |
| 5 | Eliminar ingrediente |

### Campos de un ingrediente

| Campo | Descripción |
|---|---|
| ID | Identificador único automático |
| Nombre | Nombre del ingrediente |
| Categoría | verduras / carnes / aceites / granos / lacteos / frutas / condimentos |
| Unidad | kg / litro / unidad / gramos |
| Precio por unidad | Costo de compra por unidad de medida (mínimo $0,01) |
| Stock | Cantidad disponible en bodega |
| Estado | Activo / Inactivo |

### Reglas de validación

- La categoría debe ser una de las 7 opciones restringidas.
- La unidad debe ser una de las 4 opciones disponibles.
- El precio mínimo es **$0,01** por unidad.
- El stock acepta decimales (ej: 0,5 kg).

---

## 6. Módulo 2 — Platos

Gestiona el menú del restaurante. Cada plato se compone de ingredientes registrados en el Módulo 1.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nuevo plato |
| 2 | Listar todos |
| 3 | Modificar plato |
| 4 | Activar / Desactivar |
| 5 | Eliminar plato |

### Campos de un plato

| Campo | Descripción |
|---|---|
| ID | Identificador único automático |
| Nombre | Nombre del plato |
| Precio de venta | Precio al cliente (mínimo $0,01) |
| Stock | Unidades disponibles para venta |
| Ingredientes | Lista de ingredientes con cantidad por porción |
| Estado | Activo / Inactivo |

### Composición de ingredientes

Al crear un plato, el sistema solicita agregar ingredientes uno a uno:
1. Se muestra la lista de ingredientes activos.
2. Se selecciona el ingrediente por ID.
3. Se ingresa la cantidad por porción del plato.
4. Se repite hasta completar la composición.

Un plato debe tener **al menos un ingrediente** para ser guardado.

### Reglas de validación

- Solo se pueden agregar ingredientes que existan y estén activos.
- No se puede agregar el mismo ingrediente dos veces al mismo plato.
- Al modificar, se puede añadir o quitar ingredientes individualmente.

---

## 7. Módulo 3 — Proveedores y Pedidos a Proveedor

Gestiona los proveedores de ingredientes y registra los pedidos realizados.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nuevo proveedor |
| 2 | Listar proveedores |
| 3 | Modificar proveedor |
| 4 | Desactivar / Activar proveedor |
| 5 | Nuevo pedido a proveedor |
| 6 | Listar todos los pedidos |
| 7 | Recibir pedido pendiente (actualiza stock) |
| 8 | Anular pedido pendiente |

### Flujo de un pedido a proveedor

1. Se selecciona el proveedor.
2. Se muestran los ingredientes disponibles.
3. Se selecciona un ingrediente por ID; el precio unitario se toma automáticamente del registro.
4. Se ingresa la cantidad (acepta decimales, ej: 5,5 kg).
5. Se muestra el **subtotal** del ítem y el **total acumulado** del pedido.
6. Se pregunta si añadir otro ingrediente; si no, el pedido se cierra.
7. Se muestra el **total general** antes de confirmar.
8. El pedido queda en estado **pendiente**.
9. Al **recibir** el pedido, el stock de cada ingrediente se incrementa automáticamente.

### Anulación de pedidos

Solo se pueden anular pedidos en estado **pendiente**. Si el pedido ya fue recibido, el sistema bloquea la operación. Al anular, el pedido pasa a estado **cancelado** sin revertir stock.

### Estados de un pedido a proveedor

| Estado | Color | Descripción |
|---|---|---|
| Pendiente | Amarillo | Pedido creado, aún no recibido — se puede anular |
| Recibido | Verde | Stock de ingredientes actualizado — no se puede anular |
| Cancelado | Rojo | Pedido anulado |

---

## 8. Módulo 4 — Mesas

Administra las mesas físicas del restaurante.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nueva mesa |
| 2 | Listar mesas |
| 3 | Cambiar estado de mesa |
| 4 | Desactivar / Activar mesa |

### Campos de una mesa

| Campo | Descripción |
|---|---|
| Número | Número visible de la mesa |
| Capacidad | Personas máximas |
| Ubicación | Salon / Terraza / VIP |
| Estado | disponible / reservada / ocupada |

---

## 9. Módulo 5 — Clientes

Administra la base de clientes del restaurante.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nuevo cliente |
| 2 | Listar clientes |
| 3 | Buscar cliente |
| 4 | Modificar cliente |
| 5 | Desactivar / Activar cliente |

### Campos de un cliente

| Campo | Descripción |
|---|---|
| Nombre | Nombre completo |
| Teléfono | Número de contacto |
| Email | Correo electrónico |
| Dirección | Dirección para domicilios |
| Visitas | Número total de visitas |
| Gastado | Total acumulado en pesos |
| Puntos | Puntos de fidelización (1 punto por cada $1.000) |
| Nivel | Bronce / Plata / Oro / VIP |

### Niveles de cliente (asignación automática)

| Nivel | Condición |
|---|---|
| VIP | Gasto acumulado > $5.000.000 |
| Oro | Gasto acumulado $2.000.000 – $5.000.000 |
| Plata | Gasto acumulado $500.000 – $2.000.000 |
| Bronce | Gasto acumulado < $500.000 |

El nivel se recalcula automáticamente cada vez que un cliente realiza un pedido entregado.

---

## 10. Módulo 6 — Reservas y Solicitudes Especiales

Permite gestionar reservaciones de mesas.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nueva reserva |
| 2 | Listar reservas |
| 3 | Cambiar estado de reserva |
| 4 | Editar reserva |
| 5 | Cancelar reserva |

### Campos de una reserva

| Campo | Descripción |
|---|---|
| Cliente | Cliente que reserva (debe estar registrado) |
| Mesa | Mesa a reservar |
| Personas | Número de personas (no puede superar la capacidad de la mesa) |
| Fecha | Fecha de la reserva en formato DD/MM/AAAA |
| Hora | Hora en formato HH:MM |
| Solicitudes especiales | Notas o requerimientos del cliente |

### Validaciones de fecha de reserva

- La fecha debe ser **hoy o posterior**.
- La fecha no puede ser **más de 31 días** en el futuro.
- Formato obligatorio: `DD/MM/AAAA` (ej: `25/03/2026`).

### Estados de una reserva

| Estado | Descripción |
|---|---|
| Pendiente | Reserva creada, en espera de confirmación |
| Confirmada | Reserva confirmada por el restaurante |
| En mesa | El cliente ya llegó y está en la mesa |
| Cancelada | Reserva anulada, la mesa queda disponible |

> Cuando se crea una reserva, el estado de la mesa cambia automáticamente a **"reservada"**.

---

## 11. Módulo 7 — Pedidos (Local y Domicilio)

Registra y gestiona los pedidos realizados por los clientes.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nuevo pedido |
| 2 | Listar pedidos |
| 3 | Cambiar estado de pedido |
| 4 | Agregar ítem a pedido abierto |
| 5 | Ver detalle de pedido |

### Tipos de pedido

| Tipo | Descripción |
|---|---|
| Local | Se asigna una mesa; la mesa debe estar disponible (no reservada ni ocupada) |
| Domicilio | Se registra la dirección de entrega del cliente |

### Campos de un pedido

| Campo | Descripción |
|---|---|
| Tipo | Local o Domicilio |
| Cliente | Cliente que realiza el pedido |
| Mesa | Solo para pedidos locales |
| Dirección | Solo para pedidos a domicilio |
| Ítems | Platos del menú con cantidad, precio unitario y subtotal |
| Total | Suma de todos los ítems |
| Estado | Abierto / En preparación / Entregado / Cancelado |
| Referencia | Dirección o número de mesa (columna ampliada, hasta 24 caracteres) |

### Reglas de negocio

- No se puede asignar una mesa con estado **"reservada"** a un pedido nuevo.
- Al entregar un pedido, el gasto y las visitas del cliente se actualizan automáticamente.
- Los puntos de fidelización se acumulan: **1 punto por cada $1.000** gastados.

---

## 12. Módulo 8 — Empleados

Gestiona el personal del restaurante.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Nuevo empleado |
| 2 | Listar empleados |
| 3 | Consultar empleado (detalle) |
| 4 | Modificar empleado |
| 5 | Desactivar / Activar empleado |

### Campos de un empleado

| Campo | Descripción |
|---|---|
| Nombre | Nombre completo |
| Documento | Número de documento de identidad |
| Teléfono | Número de contacto |
| Email | Correo electrónico |
| Rol | Mesero / Cocinero / Cajero / Administrador / Domiciliario |
| Salario base | Debe ser mínimo 1 SMLV ($1.750.905 en 2026) |
| Tipo contrato | Término fijo / Indefinido / Prestación de servicios |
| Fecha ingreso | Formato DD/MM/AAAA |
| Estado | Activo / Inactivo |

> El sistema no permite eliminar empleados — solo desactivarlos, preservando su historial.

---

## 13. Módulo 9 — Nómina

Calcula y registra la nómina según la normativa laboral colombiana vigente 2026.

### Opciones del submenú

| Opción | Función |
|---|---|
| 1 | Generar nómina de periodo |
| 2 | Listar nóminas generadas |
| 3 | Consultar nómina por periodo |
| 4 | Ver detalle de nómina de un empleado |

### Normativa aplicada (Decretos 1469 y 1470 del 29 de diciembre de 2025)

| Concepto | Valor 2026 |
|---|---|
| SMLV | $1.750.905 |
| Auxilio de transporte | $249.095 (aplica si salario ≤ 2 SMLV) |

### Deducciones del empleado

| Concepto | Porcentaje | Base |
|---|---|---|
| Salud | 4% | Salario base |
| Pensión | 4% | Salario base (si salario ≥ SMLV) |

### Aportes del empleador

| Concepto | Porcentaje |
|---|---|
| Salud | 8,5% |
| Pensión | 12% |
| ARL | 0,522% |
| Caja de compensación | 4% |
| ICBF | 3% |
| SENA | 2% |

### Prestaciones sociales

| Concepto | Porcentaje |
|---|---|
| Cesantías | 8,33% |
| Intereses sobre cesantías | 1% (mensual) |
| Prima de servicios | 8,33% |
| Vacaciones | 4,17% |

> El auxilio de transporte **no** es base para aportes ni prestaciones (Art. 7, Ley 1ª de 1963).

### Formato del periodo

El periodo de nómina se ingresa y muestra en formato colombiano **MM/AA** (ej: `03/26` para marzo de 2026).

### Campos del comprobante de nómina

| Campo | Descripción |
|---|---|
| Periodo | MM/AA |
| Salario base | Salario bruto del empleado |
| Auxilio de transporte | Si aplica |
| Deducción salud | 4% |
| Deducción pensión | 4% |
| Salario neto | Lo que recibe el empleado |
| Costo total empleador | Salario + todos los aportes parafiscales |
| Cesantías | Provisión mensual |
| Prima | Provisión mensual |
| Vacaciones | Provisión mensual |

---

## 14. Módulo 10 — Dashboard y Estadísticas

Muestra un resumen ejecutivo del estado del restaurante. Se puede visualizar por diferentes periodos:

| Opción | Periodo |
|---|---|
| 1 | Hoy |
| 2 | Esta semana |
| 3 | Este mes |
| 4 | Este año |

### Información mostrada

- **Pedidos:** total, entregados, cancelados, abiertos (en curso)
- **Ingresos:** total del periodo, promedio por pedido, ticket máximo, mínimo y mediana
- **Pedidos por tipo:** local vs. domicilio
- **Top 5 platos más vendidos:** con barra de progreso y unidades vendidas
- **Inventario:** ingredientes activos, ingredientes con stock bajo; platos activos, platos sin stock
- **Nómina del periodo:** empleados con nómina generada, total neto, costo empleador (visible en todos los periodos)
- **Top 5 clientes:** por monto gastado, con barra visual de progreso y nivel de fidelización

---

## 15. Módulo 11 — Simulación del Restaurante

Genera datos de prueba realistas para demostrar el funcionamiento del sistema.

### Cómo funciona

1. Se solicita el número de días a simular (1 a 30).
2. El sistema genera automáticamente (si no existen):
   - **15 ingredientes** de cocina colombiana representativos
   - **15 platos del menú** compuestos por dichos ingredientes
   - **3 proveedores** de alimentos
   - **10 mesas** (completa hasta 10 si ya hay algunas)
   - **5 empleados** con roles variados
   - **20 clientes** con nombres colombianos
3. Por cada día simulado se generan entre **8 y 25 pedidos** de platos aleatorios.
4. Al finalizar muestra el resumen: total de pedidos entregados, ingresos simulados y reservas generadas.

### Características de los datos simulados

| Elemento | Detalle |
|---|---|
| Ingredientes | 15 ingredientes con categoría, unidad y precio preconfigurados |
| Platos | 15 platos compuestos por ingredientes existentes; precio de venta preconfigurado |
| Contratos de empleados | Aleatorio entre: Indefinido / Término fijo / Prest. servicios |
| Tipo de pedido | 65% local, 35% domicilio |
| Estado de pedidos | 82% entregado, 10% cancelado, 8% abierto |
| Fechas de pedidos | Se generan hacia atrás desde hoy, cubriendo los días solicitados |
| Reservas generadas | Entre 3 y 8 reservas con fechas futuras aleatorias (1 a 15 días adelante), con hora, personas válidas, estado pendiente o confirmada, y solicitudes especiales opcionales |

> Cada ejecución genera datos **diferentes** gracias a la semilla aleatoria. Los ingredientes, platos y clientes no se duplican si ya existen.

---

## 16. Módulo 12 — Planes de Promoción

Muestra la segmentación de clientes y los planes de fidelización vigentes.

### Segmentación de clientes

| Nivel | Color | Condición de gasto |
|---|---|---|
| VIP | Magenta | > $5.000.000 |
| Oro | Amarillo | $2.000.000 – $5.000.000 |
| Plata | Cyan | $500.000 – $2.000.000 |
| Bronce | Blanco | < $500.000 |

### Planes de descuento por nivel

**★ VIP Exclusivo**
- 15% de descuento permanente en todos los pedidos
- Reserva prioritaria — confirmación en 1 hora
- Menú degustación gratis en cumpleaños
- Invitación a eventos y maridajes especiales

**★ Programa Oro**
- 10% de descuento los martes y miércoles
- Postre gratis en pedidos mayores a $80.000
- Puntos dobles los fines de semana

**★ Programa Plata**
- 5% de descuento comprando 3 veces por semana
- Bebida gratis en pedidos mayores a $60.000

**★ Bienvenida (Bronce)**
- 10% de descuento en la segunda visita
- Entrada gratis en el primer pedido del mes

**Oferta Reactivación (todos los niveles)**
- "¡Te extrañamos! 20% OFF en tu próxima visita"

### Información adicional mostrada

- Lista de clientes por cada nivel (nombre, gasto total, visitas)
- Top 5 clientes por mayor gasto
- Top 5 clientes más frecuentes por número de visitas

> Todos los 4 niveles se muestran siempre, aunque no haya clientes en alguno de ellos.

---

## 17. Guardado de Datos (Opción 13)

Guarda manualmente todos los datos en los archivos `.dat`. Los datos también se guardan automáticamente:
- Al finalizar cada operación en cualquier módulo.
- Al salir del sistema (opción `0`).

---

## 18. Diferencias Técnicas entre las Versiones

| Aspecto | Python | C++ |
|---|---|---|
| Lenguaje | Python 3.x | C++17 |
| Estructura de datos | Diccionarios (`dict`) y listas (`list`) | Structs y `vector<>` |
| Colores ANSI | Variables `R`, `B`, `RE`, `GR`, etc. | Constantes `RST`, `BLD`, `RED`, `GRN`, etc. |
| Números aleatorios | `random` (stdlib) | `<random>` con `mt19937` |
| Entrada de datos | `input()` | `getline()` / `cin` |
| Formato de pesos | Función `pesos()` | Función `pesos()` |
| Archivos `.dat` | Mismo formato que C++ | Formato de referencia |
| Compatibilidad | Comparte los `.dat` con C++ | Comparte los `.dat` con Python |

---

## 19. Resumen de Funcionalidades Implementadas

| # | Módulo | Estado |
|---|---|---|
| 1 | Ingredientes — materia prima con categoría y unidad restringidas | Completo |
| 2 | Platos del menú con composición de ingredientes | Completo |
| 3 | Proveedores & Pedidos a Proveedor (ingredientes, subtotal inmediato) | Completo |
| 4 | Mesas | Completo |
| 5 | Clientes con niveles de fidelización | Completo |
| 6 | Reservas con validación de fecha (hoy + 31 días) | Completo |
| 7 | Pedidos Local y Domicilio (basados en platos del menú) | Completo |
| 8 | Empleados (sin eliminación, solo activar/desactivar) | Completo |
| 9 | Nómina con normativa colombiana 2026 | Completo |
| 10 | Dashboard con estadísticas por periodo | Completo |
| 11 | Simulación con ingredientes, platos y datos aleatorios reales | Completo |
| 12 | Planes de promoción por niveles | Completo |
| 13 | Persistencia en archivos `.dat` compatibles entre versiones | Completo |
| 14 | Anulación de pedidos pendientes a proveedores | Completo |
| 15 | Reservas generadas automáticamente en la simulación | Completo |
| 16 | Dashboard con pedidos abiertos visibles | Completo |
| 17 | Dashboard con nómina visible en todos los periodos | Completo |

---

*Documento generado el 22 de marzo de 2026*
*PGSR v1.0 — Restaurante «Donde Siempre»*
