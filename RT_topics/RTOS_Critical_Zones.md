# Критические секции (зоны)

## Описание

Пост охватывает сразу две важные темы, но по отношению к нашей системе их лучше рассмотреть вместе. Первая тема (**критические зоны**) относится к тема ОСРВ целиком. Вторая тема (**классы функций и критических зон**) специфичная для используемой нами системы.

### Критические зоны 

Для разбора данной темы необходимо ввести понятие **атомарности операции** - свойство операции, при котором вся операция выполняется целиком (без прерываний). Нашел интересную [статью на хабре](https://habr.com/post/244881/), в которой рассказывается и разбирается на практике указанное свойство.

Свойство атомарности сильно связано с понятием критических зон. В ОСРВ **критические зоны** - блоки кода, которые должны выполняться атомарно. Это означает, что к ОСРВ предъявляется требование, при котором обернутый блок кода должен выполниться без прерываний.

Для разработчика критические секции являются инструментом, который позволяет придать блоку кода свойство атомарности.

### Классы критических зон

[Ссыль на доки](http://chibios.sourceforge.net/docs3/rt/concepts.html#system_states)

Мы разобрались, что определяют критические зоны в любой ОСРВ. Конкретно в ChibiOS этими критическими зонами автор предлагает пользоваться поактивнее не только из-за обеспечения атомарности операций разработчиков (нас с вами), но и для того, чтобы обеспечить атомарность его функций, а также для разграничения областей, где определенные функции можно пользовать.

Здесь важно отметить **правило**:
 функции системы (начинаются на `ch*()`) можно использовать только в контекстах потоков (кодах потоков). 

Под контекстом здесь понимается область, в которой пишется код. Это означает, что взять и использовать функцию `chThdSleep()` внутри обработчика прерывания (обычно являются `callback` функциями драйверов) **нельзя**.

Таким образом выделяем следующие области, для которых разграничиваем применение функций системы:
+ Thread context
	Обыкновенный код потока, в данном режиме все прерывания работают, потоки таже работают по нормальному состоянию диспетчеризации.
+ S-class critical zone
	Ядро ОСРВ заблокировано, основные прерывания отключены, быстрые прерывания работают (типы прерываний разберем в отдельном посте), диспетчеризация остановлена (с целью получения атомарности). В данное состояние можно перейти только внутри потока.
+ I-class critical zone
	Ядро ОСРВ также заблокировано, все типы прерываний отключены, диспетчеризация остановлена (с целью получения атомарности - ну здесь уж о диспетчере вообще никто не говорит, скорее для невозможности прерывания другим прерыванием). Возможно выполнение I-class функций. В данное состояние можно перейти только внутри обработчика прерывания.

### Классы функций 

[Ссыль на доки](http://chibios.sourceforge.net/docs3/rt/concepts.html#system_states)

Когда критические зоны не являются секретом для нас, в ChibiOS всплывает определение класса функций, которые определяют области использования функций. Перечислим основные классы, используемые с системе.
+ Normal
	Самые обычные функции, которые могут быть использованы в контексте потока (код потока). Являются таковыми, если документация не твердит чего-то особенного для определенной функции.
	Не имеют суффикса, например, `chThdResume()`.
+ S-class
	Функции, которые могут быть вызваны только из S-class критической зоны. По определению класса критической зоны, такие функции могут использоваться только в критических зонах потоков.
	Имеют суффикс `*S()`, например, `chThdResumeS()`.
+ I-class
	Данный класс функций может быть вызван как в S-class критической зоне, так и в I-class критической зоне. При вызове данного класса функций в S-class критической зоне необходимо далее вызывать функцию `chSchRescheduleS()` перед выходом из критической зоны, дабы пнуть диспетчер и заставить его обновить данные по потокам.
	Проще запомнить, что данный класс можно вызывать в I-class критической зоне без всякой суеты. Определение такого класса критической зоны говорит о возможности вызова только внутри критической зоны прерывания (обычно являются `callback` функциями драйверов).
	Имеют суффикс `*I()`, например, `chThdResumeI()`.
+ X-class
	Данный класс функций может быть вызван как из контекста потока (код потока), так и из S-class и I-class критических зон.
	Имеют суффикс `*X()`, например, `chThdGetSelfX()`.

Для определения, к какому классу относится функция, можно также обратиться к документам API, в которых для каждой функции есть отдельный подпункт *Function class*.

<p align="center">
<img src="https://github.com/serykhelena/RTOS_guides/blob/master/images/cz_image.png">
</p>

## Программное использование

В данном разделе имеет смысл показать основные функции для перехода в состояния S-class critical zone и I-class critical zone.

```
void chSysLock (void)
void chSysUnlock (void)
```

Данная пара функций определяет критическую зону S-class. Соответственно, чтобы начать критическую секцию S-class используется функция `chSysLock()`, чтобы закрыть - `chSysUnlock()`.

```
void chSysLockFromISR (void)
void chSysUnlockFromISR (void)
```

Следующая пара функций имеет идентичное назначение за единственным различием - работаем с I-class критической зоной.
Хотелось бы еще раз обратить внимание: S-class критическая зона может быть определена только внутри потока, I-class критическая зона - только внутри ISR (Interrupt Service Routine) обработчике прерывания. Запомнить можнос с помощью названия функций.

### Пример

```
THD_FUNCTION(myThread, arg)
{
	/* Enter S-class critical section */
	chSysLock();
	
	/* Protected code */
	
	/* Leave critical section */
	chSysUnlock();
}
```

Пример описывает использование критической секции в потоке (S-class).

```
void extcb(EXTDriver *extp, expchannel_t channel)
{
	/* Enter I-class critical section */
	chSysLockFromISR();
	
	/* Protected code */
	
	/* Leave critical section */
	chSysUnlockFromISR();
}
```

Пример описывает использование критической секции в ISR (обработчик прерывания) (I-class).

## Рекомендации
+ Во-первых, хочется отметить, что вся система ChibiOS (API - `сh*()`) строится на разграничении использования функций в разных секциях. При этом, на одну и ту же функцию могут присутствовать прототипы для разных секций. Легко заметить, что некоторые функции не предоставляют I-class варианта вовсе, что означает, что функцию нельзя использовать в прерывании.
+ Во-вторых, необходимо помнить о **важном** моменте: код внутри критической секции должен быть максимально кратким. Данное правило распространяется на все семейство ОСРВ, не только ChibiOS. При этом, не нужно бояться ими пользоваться. Если вам необходимо, чтобы пять определенных операций выполнялись атомарно, значит так надо. Правило является скорее предупреждением, которое рекомендует обратить внимание на использование критических секций в одну из первых очередей, если система сбоит :smirk: 

### Полезные ссылки
1. [Книга (критические секции)](http://www.chibios.org/dokuwiki/doku.php?id=chibios:book:embedded#critical_zones)
2. [Книга (классы)](http://www.chibios.org/dokuwiki/doku.php?id=chibios:book:kernel#api_classes)
3. [Доки (критические секции)](http://chibios.sourceforge.net/docs3/rt/group__system.html) 