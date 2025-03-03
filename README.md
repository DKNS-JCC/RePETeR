# RePETeR - Máquina de Reciclado de PET a Filamento de Impresión 3D

RePETeR es un proyecto basado en Arduino que convierte botellas de PET en filamento para impresoras 3D. Este proyecto utiliza un controlador PID para mantener la temperatura adecuada durante el proceso de extrusión.

## Componentes

- Arduino
- Pantalla SSD1306
- Relé
- Potenciómetro
- Termistor NTC
- Fuente de alimentación
- Elemento calefactor

## Instalación

1. Clona este repositorio en tu máquina local.
2. Abre el archivo `Controlador.ino` en el IDE de Arduino.
3. Conecta los componentes según el esquema de conexión.
4. Sube el código a tu Arduino.

## Esquema de Conexión

- **RELAY**: Pin 6
- **ENCODER**: Pin 2 y 3, BTN: Pin 4
- **THERM**: Pin A0
- **Pantalla SSD1306**: Conexión I2C (SDA, SCL)

## Funcionamiento

El sistema utiliza un controlador PID para mantener la temperatura del extrusor. El potenciómetro ajusta el setpoint de temperatura, y el termistor mide la temperatura actual. La pantalla SSD1306 muestra el setpoint, la temperatura actual y la salida del PID.


## Licencia

Este proyecto está bajo la Licencia MIT. Consulta el archivo `LICENSE` para más detalles.
