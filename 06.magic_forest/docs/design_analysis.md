# Diseño de la aplicación distribuida

## Indice
  - [Descomposición y mapeo por MPI](#descomposición-y-mapeo-por-mpi)
  - [Descomposición y mapeo por Open MP](#descomposición-y-mapeo-por-open-mp)
  - [Ventajas y desventajas de MPI y Open MP](#ventajas-y-desventajas-de-mpi-y-open-mp)
  - [Manejo de entrada y salida](#manejo-de-entrada-y-salida)

## Descomposición y mapeo por MPI
La implemementacion utilizada con la tecnologia de _MPI_ consiste en tener tantos procesos como nodos disponibles, en el caso del cluster 8, la descomposicion que se desea implementar es una con **mapeo dinámico**, los procesos distintos del cero le pedirán trabajo al proceso cero y este se los irá repartiendo según se lo pidan dinamicamente.

## Descomposición y mapeo por Open MP
El tipo de mapeo implementado con _OpenMP_ es un **mapeo estático**, una cantidad de hilos en encargaran de procesar un mapa es decir una matriz, cada uno trabajando en una celda sin interferir ni producir condiciones de carrera contra otros hilos.

## Ventajas y desventajas de MPI y Open MP
* Ventajas: una de las principales ventajas del tipo de mapeo estático y OpenMP es que se logra en objetivo con pocas lineas de código y el mapeo permite un trabajo equitativo entre hilos pues la cantidad de celdas asignadas se busca que sean la misma y el trabajo por celda es casi el mismo en todos los casos. Mientras que con MPI una de las ventajas del tipo de mapeo dinámico es que la descomposición es de las más óptimas posibles, pues cada proceso estará siempre trabajando.

* Desventajas: una de las desventajas al usar OpenMP con este tipo de mapeo es que al ser una tecnología declarativa es necesario tener mucho cuidado con lo que escribimos pues pocas palabras tienen un resultado en implementación muy grande. Mientras que respecto a MPI la principal desventaja es que estamos usando uno de los procesos solo para repartir trabajo, entonces imaginemos que tenemos procesadores de muy alto poder computacional y usarlo solo para distribuir trabajo será no estar aprovechando los recursos disponibles.

## Manejo de entrada y salida
Los procesos van a manejar la salida por medio de la escritura de archivos, esto lo estarían haciendo todos los procesos menos el proceso cero, que en cuyo caso será el único que no escribirá en archivos pero que por el contrario es el único que lee del archivo jobXXX.txt para pasarle el trabajo a los demás procesos.