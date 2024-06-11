# TP5 Sistema de Computación
## Device Driver Kernel Module
Para superar este TP se deberá diseñar y construir un CDD que permita sensar dos señales externas con un periodo de UN segundo. Luego, una aplicación a nivel de usuario deberá leer UNA de las dos señales y graficarla en función del tiempo. La aplicación tambien debe poder indicarle al CDD cuál de las dos señales leer. Las correcciones de escalas de las mediciones, de ser necesario, se harán a nivel de usuario. Los gráficos de la señal deben indicar el tipo de señal que se está sensando, unidades en abcisas y tiempo en ordenadas. Cuando se cambie de señal el gráfico se debe "resetear" y acomodar a la nueva medición.

Se recomienda utilizar una Raspberry Pi para desarrollar este TP.

## Definiciones
### Driver
Un driver es un programa de software que permite que un dispositivo hardware o un componente interactúe con el sistema operativo u otro software del sistema. Actúan como intermediarios entre el hardware y el software, permitiendo que el sistema operativo y las aplicaciones utilicen el hardware de manera eficiente y adecuada. Sin los drivers adecuados, los dispositivos no podrían funcionar correctamente en un sistema informático.

### Device Driver
Los device drivers son un tipo específico de driver que permite que un dispositivo de hardware interactúe con un sistema operativo o con otro software del sistema. Son esenciales para garantizar que los dispositivos funcionen correctamente en un entorno informático. Por ejemplo, los controladores de dispositivos pueden ser necesarios para tarjetas gráficas, impresoras, unidades de disco, dispositivos de red, entre otros.

### Bus Driver
Un bus driver es un componente de hardware o software que gestiona y controla el acceso a un bus de sistema. Un bus es un conjunto de cables que permite la transferencia de datos entre diferentes componentes de hardware en una computadora. Los controladores de bus aseguran que los datos se transfieran de manera eficiente y coordinada entre los dispositivos conectados al bus.

### Device Controller
 Un device controller es un componente de hardware o software que controla las operaciones de un dispositivo específico. Por ejemplo, en el contexto de almacenamiento de datos, un controlador de dispositivo puede ser responsable de controlar el acceso y la transferencia de datos hacia y desde una unidad de disco. En resumen, los controladores de dispositivos supervisan y coordinan las operaciones del hardware para garantizar un funcionamiento adecuado.
