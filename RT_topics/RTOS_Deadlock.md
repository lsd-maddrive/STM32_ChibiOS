# Взаимная блокировка

## Описание 

Про такую вещь, как взаимная блокировка (ещё зовется *тупик*), рассказывать много не приходится. Классифицировать данный эффект можно как проблему синхронизации. Если кратко - рассматриваемая проблема встречается в системах, где используется многопоточность и примитивы синхронизации. Описать проблему можно следующим образом:

+ Работник А взял дрель и для продолжения работы ему нужна маска
+ Работник Б взял маску и для дальнейшей работы нужна дрель
+ Работник А ждет маску, когда работник Б ждет дрель, и на компромисс идти ни один не собирается

Можно сказать, что ситуация достаточно абстрактна, при этом такая проблема встречается достаточно часто, чтобы о ней стоило поговорить.

В контексте ChibiOS <strike> была уже попытка создания deadlock, при этом заблокировать не удалось и есть подозрения, что существуют встроенные механизмы защиты, или просто кривые руки Леши не смогли воспроизвести тупичок)</strike> пример таки реализован, вот ссыль на [код](https://github.com/KaiL4eK/RTOS_classes_examples/blob/master/mutex_leds/main.c).

```
#include <ch.h>
#include <hal.h>

#include <chprintf.h>

#define DEADLOCK_ENABLE

MUTEX_DECL(led_mtx);

#ifdef DEADLOCK_ENABLE
MUTEX_DECL(dl_mtx);
#endif

static THD_WORKING_AREA(waBlinkerGreen, 128);
static THD_FUNCTION(BlinkerGreen, arg)
{
  arg = arg;                  //Just to avoid Warning

  while ( true )
  {
    chMtxLock(&led_mtx);

#ifdef DEADLOCK_ENABLE
    chThdSleepMilliseconds( 500 );
    chMtxLock(&dl_mtx);
#endif

    palClearLine( LINE_LED2 );
    for ( int i = 0; i < 4; i++ )
    {
      palToggleLine( LINE_LED1 );
      chThdSleepMilliseconds( 1000 );
    }

#ifdef DEADLOCK_ENABLE
    chMtxUnlock(&dl_mtx);
#endif

    chMtxUnlock(&led_mtx);
  }
}

static THD_WORKING_AREA(waFastBlinkerGreen, 128);
static THD_FUNCTION(FastBlinkerGreen, arg)
{
  arg = arg;                  //Just to avoid Warning

  while ( true )
  {
#ifdef DEADLOCK_ENABLE
    chMtxLock(&dl_mtx);
    chThdSleepMilliseconds( 500 );
#endif

    chMtxLock(&led_mtx);

    palSetLine( LINE_LED2 );
    for ( int i = 0; i < 10; i++ )
    {
      palToggleLine( LINE_LED1 );
      chThdSleepMilliseconds( 250 );
    }
    chMtxUnlock(&led_mtx);

#ifdef DEADLOCK_ENABLE
    chMtxUnlock(&dl_mtx);
#endif
  }
}

int main(void)
{
    chSysInit();
    halInit();

    chThdCreateStatic( waBlinkerGreen, sizeof(waBlinkerGreen), NORMALPRIO, BlinkerGreen, NULL );
    chThdCreateStatic( waFastBlinkerGreen, sizeof(waFastBlinkerGreen), NORMALPRIO, FastBlinkerGreen, NULL );

    while (true)
    {
      chThdSleepSeconds( 1 );
    }
}
```

К теме тестов можно глянуть в окно отладки и увидеть следующее:

<p align="center">
<img src="https://github.com/serykhelena/RTOS_guides/blob/master/images/image__1_.png">
</p>

Оба потока висят на ожидании мютекса `WTMTX`.

Если посмотреть на точки выполнения потоков, то картина куда интереснее:

<p align="center">
<img src="https://github.com/serykhelena/RTOS_guides/blob/master/images/image__2_.png">
</p>

<p align="center">
<img src="https://github.com/serykhelena/RTOS_guides/blob/master/images/image__3_.png">
</p>

Первый `BlinkerGreen` и второй `FastBlinkerGreen` потоки висят на `chMtxLock()`, причем один висит на `dl_mtx`, а другой на `led_mtx`.

Тем не менее, говоря о целом классе системы ОСРВ, важно отметить, что данная проблема играет важную роль в надежности системы. Таким образом, необходимо достаточно тщательно задумываться о проектировании использования примитивов синхронизации. Чаще всего deadlock ассоциируют с мютексами.

Теперь пару картинок, чтобы разбавить скучный текст :blush:.

<p align="center">
<img src="https://www.studytonight.com/operating-system/images/deadlock.png">
</p>

Картинка из одной из ссылок - чётко показывает стандартный шаблон взаимной блокировки.

<p align="center">
<img src="http://images.slideplayer.com/12/3361286/slides/slide_10.jpg">
</p>

Ещё одна интересная картинка, которая уже относится ближе ко временным диаграммам. В данном случае:
+ поток J2 захватывает примитив S1,
+ поток J1 вытесняет J2 и захватывает S2,
+ поток J1 пытается захватить S1 и блокируется,
+ поток J2 получает обратно управление и пытается захватить S2, что приводит к блокировке,
+ оба висят - PROFIT! :grin:

Можно также отметить ещё один эффект, который наблюдается реже, при этом его упоминание говорит о существовании такового - *livelock*. Данный эффект оличается одной важной особенностью: если deadlock блокирует систему, что приводит к полному зависанию потоков или целой системы, то livelock приводит к постоянному переключению состояний потоков, при этом работа не имеет дальнейшего результативного хода. То есть в случае последнего потоки не зависают, но обмениваются информацией или сигналами, при этом сами стоят на одном месте в терминах хода работы.

### Рекомендации

В качестве рекомендаций можно сказать, что для отслеживания рассматриваемой ситуации может очень сильно помочь отладка, в которой отображаются состояния потоков. При этом на этапе проектирования важно рассматривать возможные ситуации которые могут привести к нежелательным блокировкам. Так что в случае, если взаимная блокировка наблюдается или может быть предсказана - рекомендуется пересмотреть архитектуру синхронизации доступа к ресурсу. Также по ссылке вики присутствуют некоторые способы избежания такого рода проблем :relaxed:

### Полезные ссылки
1. [Wiki](https://ru.wikipedia.org/wiki/%D0%92%D0%B7%D0%B0%D0%B8%D0%BC%D0%BD%D0%B0%D1%8F_%D0%B1%D0%BB%D0%BE%D0%BA%D0%B8%D1%80%D0%BE%D0%B2%D0%BA%D0%B0)
2. [What is Dedalock](https://www.studytonight.com/operating-system/deadlocks)
3. [StackOverflow](https://stackoverflow.com/questions/6155951/whats-the-difference-between-deadlock-and-livelock)