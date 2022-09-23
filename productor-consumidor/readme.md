# Problema de tipo productor-consumidor

Un proceso/hilo produce datos que son posteriormente procesados por otro proceso/hilo

- El productor no puede escribir en el buffer si está lleno
- El consumidor no puede leer del buffer si está vacío
- El productor y el consumidor comparten un contador de recursos disponibles