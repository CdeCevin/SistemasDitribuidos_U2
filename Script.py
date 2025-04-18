import subprocess
import time

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
    except subprocess.CalledProcessError:
        print(f"Error compilando {nombre}")
        exit(1)

# Resultados
resultados = []

# Ejecución y medición
for bd in bases_datos:
    print(f"\nEjecutando con base de datos: {bd}")
    tiempos = {}

    for nombre in programas:
        try:
            start = time.time()
            subprocess.run(f"{nombre} < {bd}", shell=True, check=True)
            end = time.time()
            duracion = end - start
            tiempos[nombre] = duracion
            print(f"  {nombre}: {duracion:.4f} segundos")
        except subprocess.CalledProcessError:
            print(f"Error ejecutando {nombre} con {bd}")
            tiempos[nombre] = None

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
        out.write(f"{r['BD']}|\t{r['SIZE']}|\t{r['Ts']:.4f}|\t{r['Tp']:.4f}|\t{r['SpeedUp']:.2f}|\n")

print("\n✅ Resultados guardados en 'resultados.txt'")
