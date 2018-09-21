# libmaycloud

Основа сетевых приложений, используемых ООО «Оргтехсервис».

## Сборка и установка

В силу использования фирмой исключительно операционных систем семейста Debian (Ubuntu, Linux Mint), работа данного программного обеспечения на других дистрибутивах Linux не проверялась, а конечной целью сборки являются deb-пакеты:

* ``libmaycloud`` — Непосредственно библиотека
* ``libmaycloud-dev`` — Заголовочные файлы для разработки

Команды сборки:

```bash
git clone https://github.com/orgtechservice/libmaycloud.git
cd libmaycloud
cmake .
make deb
sudo dpkg -i *.deb
```

## Зависимости

Для того, чтобы скомпилировать libmaycloud, понадобится установить следующие пакеты (Ubuntu 18.04 LTS):

* ``libudns-dev``
* ``libmariadbclient-dev``, ``libperconaserverclient20-dev`` или ``libmysqlclient-dev``
* ``libsqlite3-dev``
* ``libexpat1-dev``
