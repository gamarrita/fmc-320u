# fmc_320u - Computador de Caudales.

Instruccion Para Clonar y Versión del STM32CubeIDE:
- El proyecto corre en STM32Cube IDE 1.12.1
- Aunque se trabaja con carpetas no absolutas, clonar el proyecto en d:\githubs\fm-320u\
- File -> Switch Workspace -> d:\githubs\fmc-320u
- File -> Open Project From File System -> Directory -> d:\githubs\fmc-320u -> (Se debe desticlear la opción que dice libs/fm_lcd para que no figure como una carpeta externa y aparezca efectivamente dentro de libs/)-> Finish

En este punto debería ver en Project Explorer todos los proyectos relacionados, si no los ve es posible que presionando en el pequeño ícono "restore", arriba a la izquierda, los muestre.

Configurar el límite de linea para que sea visible en 80 caracteres:
- Windows -> Preferences -> General -> Editors -> Text Editors.
- Macar Show print margin.
- Print margin columm: 80.

Configuraciones Code Analysis
 - Windows -> Preferenes -> C/C++ -> Code Analysis ->  Seleccionar todos los items menos "Line comments" que permite hacer comentarios de una linea a la derecha del código (permitido por el standard usado).
 
Para el código que no generemos nosotros habrá que incluir excepciones en la herramienta de Code Analysis, las librerias HAL y los archivos del RTOS, es código que no estará excluido del Code Analysis. Para un warning o error que nos marque el code analysis, notamos de cual se trata. Con este dato el procedimiento para exlcuirlo es:
- Windows -> Preferences -> Code Analysis. De la lista debemos buscar cual es la regla que no se cumple, luego continuamos con:
- Customize Selected -> Scope -> Exclusion Patterns Add ... Típicamente se hace Add para dos reglas:
Core/
Source/

Configuración Formato:
- Windows -> Preferences -> C/C++ -> Code Style -> Formatter -> Import... ->  d:\githubs\fm-320u\stm32cubeide_preferences\ide_formatter

Configuración Diccionario:
- Windows -> Preferences -> General -> Editors -> Text Editors -> Spelling.
- En la sección de Dictionaries -> User defined dictionary: Browse... ->  d:\githubs\fm-320u\stm32cubeide_preferences\user_defined_dictionary_esp -> abrir.
- El diccionario en Español no está completo por lo que se debe actualizar al ver que el IDE subraya en rojo las palabras que están correctamente escritas. Para agregar palabras nuevas se debe apoyar el cursor en la palabra subrayada y seleccionar la opción de "Add PALABRA to dictionary".
- Se debe conservar este diccionario a lo largo del tiempo o buscar una forma de que incluya un volumen de palabras mucho mayor.


Al finalizar, no olvidar de hacer click en la opción de Apply and close del menu Preferences.
