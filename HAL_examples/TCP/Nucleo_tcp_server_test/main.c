#include "ch.h"
#include "hal.h"
#include <serial.h>

// Это связка lwip с chibios
#include <lwipthread.h>

// А это уже сам lwip. Там много всего есть, но я мало что понимаю
#include <lwip/netif.h>
#include <lwip/api.h>


void write_data(struct netconn *conn){
    // При записи просто отправляем указатель на буфер и количество байт. Последний аргумент - флаг для записи. Но я пока про них ничего не понимаю
    netconn_write(conn, "hi there!", 9, NETCONN_NOCOPY);
}

void read_data(struct netconn *conn){
    // При чтении немного сложнее
    struct netbuf *inbuf;
    uint8_t *buf;
    uint16_t buflen;

    // Ждем что нам напишут. Функция блокирующая. В inbuf лежит информация кто и что написал
    netconn_recv(conn, &inbuf);
    // Память мы сами не выделяем, все делает lwip. Просто подсовываем известное имя, чтобы оно начало указывать на начало данных. Заодно узнаем количество байт
    netbuf_data(inbuf, (void **)&buf, &buflen);
    // Выводим по юарту полученные данные. Я пользовался tcp терминалом и отправлял буквы, так что принтфом пользоваться не надо было
    sdWrite(&SD3, buf, buflen);
    // Очишаем память. Если этого не делать она очень быстро закончится
    netbuf_delete(inbuf);
}


THD_WORKING_AREA(wa_tcp_server, 1024);
THD_FUNCTION(tcp_server, p) {

// Чтобы убрать ворнинги
  (void)p;

// Определим переменные. conn - внутреннее соединение, чтобы начать слушать порт
// newconn - внешнее соединение с тем кто постучится
  struct netconn *conn, *newconn;
  err_t err;

// Запускаем соединение в режиме TCP
  conn = netconn_new(NETCONN_TCP);

// Поленился нормально прокинуть сюда адрес, поэтому определяю его заново. Он должен быть такй же как и в мейне
  struct ip4_addr ip;
  IP4_ADDR(&ip, 192, 168, 1, 123);
// Подключаемся к 80 порту
  netconn_bind(conn, &ip, 80);

// И начинаем его слушать
  netconn_listen(conn);

  while (true) {
    palToggleLine(LINE_LED3);

    // Ждем что кто-то подключится. Функция блокирующая
    err = netconn_accept(conn, &newconn);
    // Если что-то не так - ничего не делаем
    if (err != ERR_OK)
      continue;
    // Индикация что кто-то подключился
    // Напишем что-нибудь
    write_data(newconn);
    // Прочитаем что-нибудь
    read_data(newconn);
    // По окончанию работы закрываем соединение и удаляем подключение
    netconn_close(newconn);
    netconn_delete(newconn);
    // После этого готовы снова подключится
  }

}



int main(void) {

    halInit();
    chSysInit();
    debug_stream_init();
    dbgprintf("start\r\n");


// Задаем адрес стмки
    lwipthread_opts_t opts;
    struct ip4_addr ip, gateway, netmask;
    IP4_ADDR(&ip, 192, 168, 1, 123);
    IP4_ADDR(&gateway, 192, 168, 1, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
// Макадресс вроде может быть абсолютно любой. Главное чтобы в сети не было одинаковых
    uint8_t macaddr[6] = {0xC2, 0xAF, 0x51, 0x03, 0xCF, 0x46};

    opts.address = ip.addr;
    opts.gateway = gateway.addr;
    opts.netmask = netmask.addr;
    opts.macaddress = macaddr;
    opts.link_up_cb = NULL;
    opts.link_down_cb = NULL;


// Запускаем сетевой драйвер. С этого момента на разьеме начнется индикация и стм будет пинговаться, если другой конец шнура в той же сети
// Можем не передавать настройки, использовав NULL в качестве аргумента. Тогда будут использованы настройки по умолчанию из файла lwipthread.h
    lwipInit(&opts);
    chThdSleepSeconds(2);

// Запустим поток сервера
    chThdCreateStatic(wa_tcp_server, 1024, NORMALPRIO, tcp_server, NULL);

// Помигаем лампочкой, чтобы понимать что не зависли
    while (true) {
        chThdSleepMilliseconds(1000);
        palToggleLine(LINE_LED2);
    }
}
