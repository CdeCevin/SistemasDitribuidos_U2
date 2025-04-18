import subprocess
import time
import re

# Archivos C
programas = {
    "Paralelo": "Paralelo.c",
    "Secuencial": "Secuencial.c"
}

# Bases de datos a probar
bases_datos = ["Test-BD-100.txt", "Test-BD-1000.txt", "Test-BD-95000.txt"]

# Compilación
for nombre, archivo in programas.items():
    try:
        optimizacion = "-fopenmp -O3"
        subprocess.run(f"gcc {optimizacion} {archivo} -o {nombre}", shell=True, check=True)
        print(f"Compilado {nombre} exitosamente.")
        time.sleep(1)  # Espera para evitar conflictos al compilar
    except subprocess.CalledProcessError:
        print(f"Error compilando {nombre}")
        exit(1)

# Resultados
resultados = []

# Regex para extraer el tiempo desde la salida
regex_tiempo = re.compile(r"Tiempo de ejecucion:\s*([\d.]+)\s*segundos")

# Ejecución y medición
for bd in bases_datos:
    print(f"\n📂 Ejecutando con base de datos: {bd}")
    tiempos = {}

    for nombre in programas:
        print(f"\n🔧 Ejecutando {nombre}...")
        print("⏳ Esperando 10 segundos antes de continuar con la siguiente...\n")
        time.sleep(10)
        try:
            result = subprocess.run(f"{nombre} < {bd}", shell=True, check=True, capture_output=True, text=True)
            salida = result.stdout
            print(salida)  # 👈 Imprime TODO lo que genera el programa en C

            match = regex_tiempo.search(salida)
            if match:
                tiempo = float(match.group(1))
                tiempos[nombre] = tiempo
                print(f"✅ Tiempo capturado de {nombre}: {tiempo:.6f} segundos")
            else:
                print(f"⚠ No se encontró el tiempo en la salida de {nombre}")
                tiempos[nombre] = None
        except subprocess.CalledProcessError:
            print(f"❌ Error ejecutando {nombre} con {bd}")
            tiempos[nombre] = None

        time.sleep(2)  # Espera para liberar recursos entre ejecuciones

    # Delay de 10 segundos después de procesar ambas versiones


    # Cálculo del tamaño de la BD
    with open(bd, "r") as f:
        SIZE = sum(1 for _ in f) - 1  # Restar 1 por la cabecera

    Ts = tiempos.get("Secuencial")
    Tp = tiempos.get("Paralelo")
    speed_up = Ts / Tp if Tp and Ts else None

    resultados.append({
        "BD": bd,
        "SIZE": SIZE,
        "Tp": Tp,
        "Ts": Ts,
        "SpeedUp": speed_up
    })

# Guardar en archivo .txt
with open("resultados.txt", "w") as out:
    out.write("Archivo de resultados de rendimiento\n")
    out.write("===================================\n")
    out.write("BD|\t\tSIZE|\tTs (s)|\tTp (s)|\tSpeedUp (Ts/Tp)|\n")
    for r in resultados:
        if r["Tp"] and r["Ts"]:
            out.write(f"{r['BD']}|\t{r['SIZE']}|\t{r['Ts']:.6f}|\t{r['Tp']:.6f}|\t{r['SpeedUp']:.2f}|\n")
        else:
            out.write(f"{r['BD']}|\t{r['SIZE']}|\tError de tiempo|\n")

print("\n✅ Resultados guardados en 'resultados.txt'")
