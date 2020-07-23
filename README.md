# libmaycloud

Данная библиотека представляет собой основу сетевых приложений, разрабатываемых фирмой ООО «Оргтехсервис» (г. Майкоп) для внутреннего использования. Сама библиотека опубликована на условиях лицензии MIT и может быть использована в любых открытых или закрытых проектах. Библиотека содержит компоненты, упрощающие разработку сетевых серверов и клиентов, главным образом, для протоколов, основанных на потоковом XML (XMPP-подобных), но не ограничиваясь ими. Репозиторий [libmaycloud-example-webserver](https://github.com/orgtechservice/libmaycloud-example-webserver) содержит пример простого веб-сервера, построенного на базе данной библиотеки. Поддерживается реализация нескольких раздельных серверов в рамках одного приложения. Встраиваемый веб-сервер находится на раннем этапе разработки.

## Сборка и установка

В силу использования фирмой преимущественно операционных систем семейста Debian (Ubuntu, Linux Mint), работа данного программного обеспечения на других дистрибутивах Linux проверялась мало, а конечной целью сборки является deb-пакет. Вопреки принятой в Debian системе, когда сама библиотека и её заголовочные файлы разбиваются на два раздельных пакета, формируется один-единственный deb-пакет, включающий всё сразу, это сделано для удобства и минимизации ручной работы. Также имеется возможность формирования rpm-пакета для установки на CentOS 6.x и основанных на ней системах.

Работа данного программного обеспечения на операционных системах, отличных от Linux (Windows, FreeBSD, OpenIndiana и прочие) невозможна в силу использования системных вызовов ОС Linux (тем не менее, возможно использование в среде WSL).

Команды сборки (debug-версия):

```bash
git clone https://github.com/orgtechservice/libmaycloud.git
cd libmaycloud
./debug.sh
```

Команды сборки (release-версия):

```bash
git clone https://github.com/orgtechservice/libmaycloud.git
cd libmaycloud
./release.sh
```

## Зависимости

Для того, чтобы скомпилировать libmaycloud в минимальной конфигурации, понадобится установить следующие пакеты (Ubuntu 20.04 LTS):

* ``cmake``
* ``build-essential``
* ``pkg-config``
* ``libudns-dev``
* ``libsqlite3-dev``
* ``libexpat1-dev``

```bash
sudo apt install libudns-dev libsqlite3-dev libexpat1-dev cmake build-essential pkg-config
```
