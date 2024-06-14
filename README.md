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

### Tipos de Dispositivos
Los dispositivos se dividen en dos tipos: dispositivos de caracteres y dispositivos de bloques. La diferencia es que los dispositivos de bloque tienen un búfer para solicitudes, por lo que pueden elegir el mejor orden en el que responder a las solicitudes. Esto es importante en el caso de dispositivos de almacenamiento, donde es más rápido leer o escribir sectores que están cerca uno del otro, en lugar de los que están más separados. Otra diferencia es que los dispositivos de bloque solo pueden aceptar entrada y devolver salida en bloques (cuyo tamaño puede variar según el dispositivo), mientras que los dispositivos de caracteres pueden usar tantos o tan pocos bytes como deseen. La mayoría de los dispositivos son de caracteres, porque no necesitan este tipo de almacenamiento en búfer y no operan con un tamaño de bloque fijo.

Para saber si un archivo de dispositivo es para un dispositivo de bloque o de caracteres, se puede ver el primer carácter en la salida de ls -l. Si es 'b', entonces es un dispositivo de bloque, y si es 'c', entonces es un dispositivo de caracteres.

Ejemplo de salida de ls -l /dev:

![image](https://github.com/marcosraimondi1/tp4-siscom/assets/69517496/fa90e6b8-e231-4721-a088-5dac381bac02)

Se puede observar que el dispositivo /dev/sda y sus particiones son dispositivos de bloque. Corresponde al disco de estado solido de la PC y sus respectivas particiones.

Los archivos /dev/tty en Linux son dispositivos de caracteres que representan las terminales del sistema y proporcionan un medio para interactuar con ellas desde programas en el sistema.

Las dos columnas de numeros separadas por coma corresponde al número mayor y numero menor del dispositivo. El número mayor indica qué controlador se usa para acceder al hardware. A cada controlador se le asigna un número mayor único; todos los archivos de dispositivo con el mismo número mayor son controlados por el mismo controlador. El número menor es utilizado por el controlador para distinguir entre el hardware variado que controla. 

Se observa en el ejemplo que en la entrada de /dev/sda son controlados por el mismo driver ya que tienen el mismo numero mayor 8. Ese driver distingue lo que son las particiones por el numero menor de cada una.

![Bus Driver](https://github.com/AndyTaborda/tp5-siscom/assets/102503527/ce43ab99-0b04-4151-a0b1-1e7ef978df76)

## Implementación
### Dispositivo utilizado y SO
En el desarrollo del Tp se opto por el uso de una placa RaspberryPi Zero 2. La misma es ideal para el desarollo y dispone de una gran variedad de funcionalidades y pines:

![Raspberrypi Zero pinout](https://github.com/Giuli2803/tp5-siscom/assets/66461191/528c8b29-4f8e-49aa-bdb0-a7f9a5a6091f)

Para observar el numero que corresponde a un GPIO especifico se puede utilizar el comando: `cat /sys/kernel/debug/gpio` que muestra ese mapeo. Basicamente se tiene que sumar 512 al numero de GPIO que se quiere usar. (Ej. GPIO26 -> 538 en el codigo de C).

Esta placa dispone de conexión wifi, bluetooth y varios pines. Para la implementación fue muy util la preconfiguración de red wifi que se puede realizar al momento de la carga del sistema operativo, esto puso a disposición y nos permitió acceder mediante SSH (o Secure Shell) a la terminal de la placa, esto nos facilito el manejo remoto de la misma ya que no se disponia de un monitor ni de adaptadores para conectar la placa y manipularla directamente.

![Config_Raspberry_imager](https://github.com/Giuli2803/tp5-siscom/assets/66461191/380c55b8-02f4-4bb4-957d-40de0a67b12e)

La placa fue cargada con el sistema operativo que pone a disposición el fabricante en la pagina oficial, el "Raspberry Pi OS with desktop" de 64-bits y con la version de kernel 6.6.

![Raspberry](https://github.com/Giuli2803/tp5-siscom/assets/66461191/d2d1d328-bd32-4104-97cb-0a3e9a476f98)

Una vez obtenida la conexión con la placa se pudieron enviar archivos mediante ssh para poder ejecutar todo lo necesario para el desarrollo del tp. En la proxima sección se empieza el despliegue del codigo sobre la placa de forma remota.

### Topología
![1](https://github.com/AndyTaborda/tp5-siscom/assets/102503527/e0225075-77e7-4b78-be50-f8f779517f87)

Para acceder y controlar la Raspberry Pi, primero se instaló Raspbian en una tarjeta microSD y se habilitó SSH creando un archivo vacío llamado `ssh` en la partición de arranque. Luego, se conectó la Raspberry Pi a la red WiFi provista por un celular. Para encontrar la dirección IP asignada a la placa, se verificó en la configuración del punto de acceso del celular. Una vez obtenida la dirección IP, se procedió a establecer una conexión SSH desde la computadora. Para mejorar la seguridad, se cambió la contraseña por defecto y se utilizó el usuario `neuronas`.

### Driver Lifecycle

El driver se inicializa en el metodo `my_init` que se ejecuta cuando se carga el modulo con `insmod`:

```c
int __init my_init(void) {
  printk("Hello World - GPIO\n");

  int status;
  int ret;
  struct device *dev_ret;

  // request GPIO pins

  status = gpio_request(IO_A, "IO_A");
  if (status != 0) {
    printk("Error requesting IO_A\n");
    return status;
  }
  // ...

  // set directions

  status = gpio_direction_input(IO_A);
  if (status != 0) {
    printk("Error setting direction input IO_A\n");
    gpio_free(IO_A);
    gpio_free(IO_B);
    return status;
  }

  // ...

  // register char device

  if ((ret = alloc_chrdev_region(&first, 0, 1, "my_module")) < 0) {
    gpio_free(IO_A);
    gpio_free(IO_B);
    return ret;
  }

  if (IS_ERR(cl = class_create(THIS_MODULE, "my_module"))) {
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return PTR_ERR(cl);
  }

  if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "my_module"))) {
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return PTR_ERR(dev_ret);
  }

  cdev_init(&c_dev, &my_fops);
  if ((ret = cdev_add(&c_dev, first, 1)) < 0) {
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return ret;
  }

  // Create workqueue
  my_wq = create_singlethread_workqueue("my_wq");
  if (!my_wq) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    gpio_free(IO_A);
    gpio_free(IO_B);
    return -ENOMEM;
  }

  // Initialize delayed work
  INIT_DELAYED_WORK(&my_work, work_function);

  // Queue the first work
  queue_delayed_work(my_wq, &my_work, read_period);

  return 0;
}
```
Aqui se solicitan los pines GPIO, se configuran como entradas, se registra el dispositivo de caracter, se crea la workqueue y se inicializa el trabajo asincrono.

El driver se libera en el metodo `my_exit` que se ejecuta cuando se descarga el modulo con `rmmod`:

```c
void __exit my_exit(void) {
  printk("Goodbye! \n");

  // Cancel the delayed work and destroy the workqueue
  cancel_delayed_work_sync(&my_work);
  destroy_workqueue(my_wq);
  
  // Unregister device
  cdev_del(&c_dev);
  device_destroy(cl, first);
  class_destroy(cl);
  unregister_chrdev_region(first, 1);
  
  // free gpios
  gpio_free(IO_A);
  gpio_free(IO_B);
}
```

Aqui se cancela el trabajo asincrono y se destruye la workqueue, se desregistra el dispositivo, se destruye la clase y se libera el rango de numeros de dispositivo. Finalmente se liberan los pines GPIO.

### Device Driver File Operations: Lectura y Escritura del Driver

Se carga en la estructura `file_operations` las operaciones de lectura y escritura del driver:
```c
static struct file_operations my_fops = {
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release, 
};
```

Para la escritura del driver, se implemente la siguiente operacion:

```c
static ssize_t my_write(struct file *file, const char __user *buf, size_t cnt,
                        loff_t *off) {
  u8 val;

  if (cnt == 0)
    return 0;

  val = buf[0] - '0'; // convert ascii char to int

  if (val == 0) {
    chosen_pin = IO_A;
  } else {
    chosen_pin = IO_B;
  }

  printk("chosen GPIO: %d\n", chosen_pin - 512);

  return cnt;
}
```

Al escribir en el character device driver se selecciona el pin a utilizar, si el valor es 0 se selecciona el IO_A, si el valor es 1 se selecciona el IO_B.

Para la lectura del driver, se implementa la siguiente operacion:

```c
ssize_t my_read(struct file *file, char __user *buf, size_t cnt, loff_t *off) {
  int to_copy = cnt > 8 ? 8 : cnt;
  return to_copy - copy_to_user(buf, last_value, to_copy);
}
```

Al leer del character device driver se copia el valor de `last_value` al buffer de usuario. 
Este valor se actualiza asincronamente cada 1 segundo, cuando se lee el driver se copia el ultimo valor leido del GPIO seleccionado.
El worker thread se encarga de leer el valor del GPIO seleccionado y actualizar el valor de `last_value`:

```c
static void work_function(struct work_struct *work) {
  u8 value;

  value = gpio_get_value(chosen_pin);
  snprintf(last_value, 8, "%d\n", value);

  // Queue the work again to run after 1 second
  queue_delayed_work(my_wq, &my_work, read_period);
}
```


### Crear el Modulo
Se crea un archivo my_module.c con el codigo fuente y un archivo Makefile para compilarlo.

![image](https://github.com/marcosraimondi1/tp5-siscom/assets/69517496/6b1555b4-c5b5-4a5f-ade8-c8978d2cd83f)

Para cargar el modulo se usa el comando insmod *.ko .

El modulo al cargarse registra un dispositivo llamado my_module con un numero major asignado dinamicamente segun esten disponibles.

![image](https://github.com/marcosraimondi1/tp5-siscom/assets/69517496/65dc6101-45a2-4399-8cec-c996c5ae7198)

![image](https://github.com/marcosraimondi1/tp5-siscom/assets/69517496/c45f844e-5e8e-48cd-bdc1-0f53bd947e5c)

![image](https://github.com/marcosraimondi1/tp5-siscom/assets/69517496/459a07f8-9439-4cdd-ba94-a1d6a8e46416)

Para eliminar el modulo se usa el comando rmmod:

![image](https://github.com/marcosraimondi1/tp5-siscom/assets/69517496/318eb8b0-b089-463f-b347-ce42893ccfe3)


### Aplicacion de Usuario
La aplicacion de usuario se crea en python, se crean 2 scripts:
1. El primero corre en la raspberry y le el character device para obtener y registrar los valores de los GPIO indicado por el usuario. Estos valores se guardan en un archivo .csv con un timestamp asociado al momento de lectura.
2. El segundo corre en la computadora principal que se conecta por ssh, y se encarga de generar los graficos a partir del archivo .csv obtenido en el script anterior. Esto se hace asi ya que no se contaba con una interfaz grafica en la raspberry pi porque estabamos conectados por ssh.

Para transferir el archivo .csv desde la raspberry pi hacia la computadora principal se utiliza el comando `scp` (secure copy) que lo copia a traves de ssh:
```sh
scp neuronas@<raspi_ip>:<path_to_csv> <local destination directory>
```

Un ejemplo del grafico generado se puede observar a continuacion:
![image](https://github.com/marcosraimondi1/tp5-siscom/assets/69517496/d3d91599-7fba-448d-977f-35e13988d1dd)


### Demos
Se pone a disposición 2 videos realizados durante el desarrollo del tp, los cuales explican y muestran el desarrollo realizado por parte del equipo. Tambien se adjunta un video muy interesante que muestra detalles mas a fondo a la hora de armar un driver, el mismo fue de mucha utilidad para tomar referencias durante el desarrollo.
- [Demo 2 Con Explicacion](https://drive.google.com/file/d/1pB57ipW9DzwsvEO1VDaYb4qELzmfnMeI/view?usp=sharing)
- [Demo 1](https://drive.google.com/file/d/1dYLAVaglvw03ymg3qfwNMQR9RRIIWmg3/view?usp=sharing)
- [Video de referencia](https://www.youtube.com/watch?v=xk0xdN2SzV0&t=1167s)

