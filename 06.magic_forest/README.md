# Magic Forest

## Table of Contents
  - [Información básica](#información-básica)
  - [Compilar](#compilar)
  - [Manual de usuario](#manual-de-usuario)
  - [Análisis de rendimiento](#análisis-de-rendimiento)
## Información básica
***
Este proyecto es con la finalidad de generar simulaciones respecto a un bosque encantado creado por un rey, donde cada noche en el bosque se efectuarán cambios, resultando al día siguiente en un nuevo bosque que los ciudadanos podran disfrutar. Para ello se desan hacer simulaciones para ver el estado del bosque en futuros días, semanas, meses e incluso años. Esto es similar proyectos creados anteriormente en la ECCI, tales cómo Cellular Automata, y Conway's Game of Life.
***

## Compilar
Para realizar la compilación de este proyecto basta con realizar los siguientes pasos en una terminal de comandos:
```
$ git clone https://git.ucr.ac.cr/knights-pi-calculus/concurrente21a-emmanuel-bryan-marco.git
$ cd magic_forest
$ make
$ bin/magic_forest #job_file.txt
```

## Manual de usuario
Respecto a la evaluación de este código se puede decir que tiene 3 clases principales:
1. Forest_Controller: es la mente de la aplicación, es el responsable por una ejecución correcta de la misma y que los metodos y las siguientes dos clases sean ejecutadas correctamente.
2. Forest_Reader_Writer: como sabemos que nuestro trabajo es recibido por medio de un parámetro por linea de comandos y a partir de ese archivo salen los demás archivos a ejecutar; esta clase se encarga de analizar todo este texto, de este forma crea el bosque con sus respectivas características que luego podrá ser usado por la siguiente clase para la resolución del problema. Al mismo tiempo se encarga de guardar en un archivo, o mostrar según se desee, los resultados los cambios que se van efectuando en el bosque.
3. Forest_Evaluator: como su nombre en el inglés lo indica es una clase que se encarga de analizar el bosque actual y a partir de él crear uno nuevo siguiendo las respectivas reglas del encantamiento.

A partir de este conocimiento podemos saber lo siguiente respecto a los fundamentos de como utilizar este programa. El programa luego de ser invocado como un ejecutando esperar recibir el nombre de un archivo tal como:
```
jobXXX.txt
```
 donde XXX representa el numero del archivo, dentro de este se deben incluir los nombres de los archivos a ejecutar, a saber, los:
 ```
mapXXX.txt Y
```
las 'X' representan lo mismo que para job pero 'Y' son las cantidad de veces que se desea hacer esta simulación, además de esto, se debe considerar lo siguiente respecto al número:
> Positivo: el programa producira un nuevo archivo para cada simulación que se solicite; siguiendo uno de los dos formatos, donde Y es el numero de noches que quiere ejecutar:
```
mapXXX-Y.txt
mapXXX.txt -Y
```
donde 'Z' es la noche que se está ejecutando.
> Negativo: significa que el programa producirá una única salida, la final que queda despues de hacer todas las simulaciones.

Se provee un ejemplo de como debe ser un archivo de texto 'mapa' para ser incluido, siga las siguientes reglas:
1. El nombre del archivo debe ser el incluido en el archivo jobXXX.txt que desee procesar, luego del nombre mapXXX.txt incluya la cantidad de noches que desea que se realice la simulacion, ponga el numero como negativo si desea que se generen archivos .txt de los resultados de cada noche, de lo contrario solo se imprimira el resultado final.
2. En la primera linea del archivo escriba las dimensiones de la matriz, sea honesto con estas dimensiones:
   ```
   cantidad_filas cantidad_columnas
   ```
3. Introduzca el mapa que desea ejecutar, usando solo los caracteres **'l', 'a', '-'**. Un ejemplo de como seria una matriz es la siguiente:

|  |  |  |  |  |
|---|---|---|---|---|
| l | - | a | - | l |
| l | - | a | - | l |
| l | - | a | - | l |


## Análisis de rendimiento
Hay tres mediciones principales que deben realizarse con el caso de prueba job002.txt:
1. Version concurrente con Open MP:
   1. **Duracion:** 59 minutos.
   2. **Incremento de velocidad:** 1.000, pues esta es la version base.
   3. **Eficiencia:** 1.000, pues es la version base.
2. Version distribuida con MPI:
   1. **Duracion:** 7.5 minutos.
   2. **Incremento de velocidad:** 1.4844, comparado con la version que solo implementa Open MP.
   3. **Eficiencia:** 0.0619, comparado con la version que solo implementa Open MP.
3. Version con ambas tecnologias:
   1. **Duracion:** 5.5 minutos.
   2. **Incremento de velocidad:** 2.0242, comparado con la version que solo implementa Open MP.
   3. **Eficiencia:** 0.0843, comparado con la version que solo implementa Open MP.

Estos resultados estan, a su vez, representados en el siguiente grafico:
![](design/Grafico_Rendimiento.png)

Con las soluciones de _Open MP_ y _MPI_ que fueron implementadas en este codigo si se obtivieron los resultados que se esperaban. _Open MP_ permitio que una de las tareas mas dificiles, calcular los siguientes dias debido al uso de matrices, se volviera mucho mas rapido; y a su vez _MPI_ permitio que los mapas que se deseaban resolver se procesaran varios al mismo tiempo. Consideramos que los resultados son los que esperabamos; sin embargo, si desearamos optimizar o mejorar la eficiencia de nuestro codigo aun mas, tomariamos el siguiente camino:
1. Hacer un analisis de regiones criticas pero no tomar en cuenta las regiones que ya se hacen paralelamente.
2. En base a los resultados del punto anterior mejorar el codigo de C++ tratando de acotar calculos innecesarios o mejorando funciones de codigo.
3. Tratariamos de implementar '_trucos_' para tomar ventaja de los compiladores que estemos usando para mejorar el lenguaje maquina que estos crean.
4. Como ultima opcion, si fuera necesario, implementariamos funciones en codigo ensamblador tratando de hacerlas lo mas optimas posibles, esto podria incluso incluir el uso de las instrucciones AVX y SSE que son las instrucciones vectorizadas que permitien hacer varios calculos con una sola instruccion.