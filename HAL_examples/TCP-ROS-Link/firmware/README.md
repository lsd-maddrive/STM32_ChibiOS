# Firmware for ROS by TCP communication

## Notes

- LWIP и SPI (нога A7) ломают друг-друга =(
- Для работы над данным проектом были добавлены PWM драйверы.
  - Для установки патча скопируйте папку (не удаляйте предыдущую) `os` из папки `chibios_patch` к себе в папку `chibios*`.

## Установка для ChibiOS

### Установка LWIP (драйвер сетевой коммуникации)

- Распакуйте архив liwp_*.zip в папке /ext

## Управление через команды ROS

- Необходимо реализовать в функциях *_cb в файле [src/ros.cpp](src/ros.cpp)

## References

Полезные ссылки:
* [F767ZI Reference manual (RM)](http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/96/8b/0d/ec/16/22/43/71/DM00224583/files/DM00224583.pdf/jcr:content/translations/en.DM00224583.pdf)
* [F767ZI Datasheet (DS)](http://www.st.com/content/ccc/resource/technical/document/datasheet/group3/c5/37/9c/1d/a6/09/4e/1a/DM00273119/files/DM00273119.pdf/jcr:content/translations/en.DM00273119.pdf)
* [F767ZI Nucleo pinout](https://os.mbed.com/platforms/ST-Nucleo-F767ZI/)
---
* [F767ZI Alternate functions](http://www.st.com/content/ccc/resource/technical/document/datasheet/group3/c5/37/9c/1d/a6/09/4e/1a/DM00273119/files/DM00273119.pdf/jcr:content/translations/en.DM00273119.pdf#page=89)
* [F767ZI Pins assignment (+analogs)](http://www.st.com/content/ccc/resource/technical/document/datasheet/group3/c5/37/9c/1d/a6/09/4e/1a/DM00273119/files/DM00273119.pdf/jcr:content/translations/en.DM00273119.pdf#page=65)
* [F767ZI Timer triggers](http://www.st.com/content/ccc/resource/technical/document/reference_manual/group0/96/8b/0d/ec/16/22/43/71/DM00224583/files/DM00224583.pdf/jcr:content/translations/en.DM00224583.pdf#page=452)
