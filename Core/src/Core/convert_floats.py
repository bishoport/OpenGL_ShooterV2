import re
import os

# Definir la ruta del archivo original y el archivo de salida
input_file = r'C:\Produccion\PROPIOS\OpenGL_DEV\OpenGLSandbox\Core\src\Core\Itc_matrix.hpp'
output_file = r'C:\Produccion\PROPIOS\OpenGL_DEV\OpenGLSandbox\Core\src\Core\Itc_matrix_floats.hpp'

# Verificar si el archivo existe
if not os.path.exists(input_file):
    print(f"Error: El archivo {input_file} no existe.")
else:
    # Leer el archivo original
    with open(input_file, 'r') as file:
        content = file.read()

    # Expresión regular para encontrar números flotantes y agregar 'f' al final si no lo tienen
    content = re.sub(r'(?<!\w)(-?\d+\.\d+(e-?\d+)?)(?!\w)', r'\1f', content)

    # Escribir el contenido modificado a un nuevo archivo
    with open(output_file, 'w') as file:
        file.write(content)

    print(f"Conversión completada. Nuevo archivo generado: {output_file}")


