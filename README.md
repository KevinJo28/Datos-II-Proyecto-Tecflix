# Proyecto Tecflix - Repositorio Principal

Bienvenido al repositorio del proyecto Tecflix. Este archivo contiene lineamientos importantes para el flujo de trabajo en equipo durante las 7 semanas de desarrollo.

## Flujo de Ramas (Git Flow Simplificado)

**REGLA DE ORO:** Nadie, bajo ninguna circunstancia, debe subir código directamente a la rama `main`. Todos deben usar ramas.

### Ejemplo de Flujo de Trabajo (Dev 1 en Sprint 1):

1.  **Sincronizar:** Descarga los últimos cambios de la rama principal.
    `git pull origin main`

2.  **Crear Rama:** Crea una rama específica para tu tarea de Jira. Usa un prefijo como `feature/` o `fix/` seguido del código de la tarea.
    `git checkout -b feature/DIPT-17`

3.  **Programar y Guardar localmente:** Avanza en tu código y haz commits locales descriptivos diarios (o más frecuentes).
    `git commit -m "feat: implementar insercion de nodos en Arbol B"`
    `git commit -m "feat: agregar serializacion a binario del nodo"`

4.  **Subir al Servidor (GitHub):** Al terminar la tarea, o al final del día para tener un respaldo en la nube, sube tu rama.
    `git push origin feature/DIPT-17`

## Estructura del Repositorio

- `api-usuarios/`: Código C++ (Dev 1)
- `api-metadata/`: Código C++ (Dev 1)
- `api-streaming/`: Código C++ (Dev 2)
- `web-app/`: Código ReactJS (Dev 3)
- `docs/`: Documentación, SPEC.md, Paper V2.0, Scripts DB (Dev 4 y Todos)
