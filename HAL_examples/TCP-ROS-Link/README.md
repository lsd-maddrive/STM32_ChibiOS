# Пример настройки связи ROS и uC по TCP

- [firmware](firmware) - директория с прошивкой для МК
- [software](software) - вспомогательные файлы для примера

## Что мне делать?

- Прошить микроконтроллер F767ZI-Nucleo прошивкой
- Подключить Ethernet кабель uC-комп
- Настройть статическую сеть на компе:
  - IP: 9.9.9.1
  - Mask: 255.255.255.0 (или возле IP ставить суффикс "/24")
  - Gateway: 9.9.9.1
- Если еще не стоит [ROS Noetic](http://wiki.ros.org/noetic/Installation), то поставить (катали ток на Linux, на Windows можно накатить WSL)
  - Также, поставить пакет `rosserial-server`

    ```bash
    sudo apt install ros-noetic-rosserial-server
    ```

- Находясь в директории рядом с ридми (этим файлом) запустить Socket сервер для ROS

```bash
roslaunch software/uc_socket_server.launch 
```

- Проверить, что из контроллера идут команды

```bash
# В отдельном терминале
rostopic echo /robot/out_data
```

- Можно отправить на uC данные (надо проверять в терминале):

```bash
rostopic pub /robot/in_data std_msgs/Int8 "data: 10"
```

## Notes

- Пример предполагает использование стандартных типов из пакета [std_msgs](http://wiki.ros.org/std_msgs), но можно делать собственные типы.
