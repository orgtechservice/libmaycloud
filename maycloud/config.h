#ifndef MAYCLOUD_CONFIG_H
#define MAYCLOUD_CONFIG_H

/**
 * На данном этапе есть смысл обрабатывать выключение/выключение перекодирования аудио дефайнами
 * Возможно, в дальнейшем это не понадобится
 */
//#define CAMCAP_REMUX_AUDIO_ON
#define CAMCAP_RECODE_AUDIO_ON

/**
* Поддержка GNU TLS
*/
#undef HAVE_GNUTLS

/**
* Размер блока файлового буфера
*/
#define FDBUFFER_BLOCK_SIZE 1024

/**
* Размер файлового буфера в блоках
*/
#define FDBUFFER_DEFAULT_SIZE 16

/**
* Размер буфера чтения
*/
#define FD_READ_CHUNK_SIZE 4096

/**
* Поддержка zlib сконфигурирована?
*/
#undef HAVE_LIBZ

/**
* Уроверь компресси в zlib
*/
#define ZLIB_COMPRESS_LEVEL 6

/**
* Размер блока-буфера компрессии zlib
*/
#define ZLIB_DEFLATE_CHUNK_SIZE 4096

/**
* Размер блока-буфера для декомпрессии zlib
*/
#define ZLIB_INFLATE_CHUNK_SIZE (FD_READ_CHUNK_SIZE * 8)

/**
 * utm_services
 */

#define UTM_SERVICES_CONFIG_FN "/etc/utm_services.conf"

#define NF_SERVICE_PID_FILE "/var/run/nf_service.pid"
#define NF_SERVICE_LOG_FN "/var/log/nf_service.log"
#define NF_SERVICE_TMP_STDIN_FN "/tmp/nf_service_in"
#define NF_SERVICE_TMP_STDOUT_FN "/tmp/nf_service_out"
//#define NF_SERVICE_DEBUG

#endif // MAYCLOUD_CONFIG_H
