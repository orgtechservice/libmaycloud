#ifndef MAYCLOUD_CONFIG_H
#define MAYCLOUD_CONFIG_H

/**
 * Путь к pid-файлу контроллера
 */
#define CONTROLLER_PID_FILE "/var/run/ots_controller.pid"

/**
 * Путь к конфигурационному файлу контроллера
 */
#define CONTROLLER_CONF_FILE "/etc/maycloud/ots_controller.conf"

/**
 * Путь к лог-файлу stderr контроллера
 */
#define CONTROLLER_LOG_FN "/tmp/stderr.log"

/**
 * Для хоста всё аналогично контроллеру
 */
#define HOST_PID_FILE "/var/run/ots_host.pid"
#define HOST_CONF_FILE "/etc/maycloud/ots_host.conf"
#define HOST_LOG_FN "/dev/null"

/**
 * Для рекордера всё аналогично контроллеру
 */
#define RECORDER_PID_FILE "/var/run/ots_recorder.pid"
#define RECORDER_CONF_FILE "/etc/maycloud/ots_recorder.conf"
#define RECORDER_LOG_FN "/dev/null"

/**
 * И тут в дело вступает Капитан Очевидность и в очередной раз спасает мир
 */
#define SPEAKER_PID_FILE "/var/run/ots_speaker.pid"
#define SPEAKER_CONF_FILE "/etc/maycloud/ots_speaker.conf"
#define SPEAKER_LOG_FN "/dev/null"

/**
* И тут в дело снова вступает Капитан Очевидность
*/
#define PPPOEMON_PID_FILE "/var/run/ots_pppoemon.pid"
#define PPPOEMON_CONF_FILE "/etc/maycloud/ots_pppoemon.conf"
#define PPPOEMON_LOG_FN "/dev/null"

/**
* В качестве примера для модуля Example
*/
#define EXAMPLE_PID_FILE "/var/run/ots_example.pid"
#define EXAMPLE_CONF_FILE "/etc/maycloud/ots_example.conf"
#define EXAMPLE_LOG_FN "/dev/null"

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
