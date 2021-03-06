#ifndef NANOSOFT_ASYNCSTREAM_H
#define NANOSOFT_ASYNCSTREAM_H

#include <maycloud/asyncobject.h>
#include <maycloud/config.h>
#include <maycloud/error.h>

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

/**
* Класс для асинхронной работы с потоками
*/
class AsyncStream: public AsyncObject
{
private:
	/**
	* Флаги
	*/
	int flags;

	/**
	 * Обработка поступивших сообщений об ошибках в потоке
	 */
	void handleError();
	
	/**
	* Обработка поступивших данных
	* 
	* Данные читаются из сокета и при необходимости обрабатываются
	* (сжатие, шифрование и т.п.). Обработанные данные затем передаются
	* виртуальному методу onRead()
	*/
	void handleRead();

	/**
	* Передать данные обработчику onRead()
	*/
	void putInReadEvent(const char *data, size_t len);
	
	/**
	* Отправка накопленных данных
	*
	* Эта функция вызывается когда сокет готов принять данные и производит
	* отправку данных накопленных в файловом буфере
	*/
	void handleWrite();
	
	/**
	* Обработка обрыва связи
	*/
	void handlePeerDown();
protected:
	
	/**
	* Вернуть маску ожидаемых событий
	*/
	virtual uint32_t getEventsMask();
	
	/**
	* Обработчик события
	*/
	virtual void onEvent(uint32_t events);
	
	/**
	 * Обработчик ошибки разрешения домена
	 */
	virtual void onResolveFault();
	
	/**
	 * Обработчик установления соединения
	 * 
	 * Этот обработчик вызывается после успешного вызова connect().
	 * В ассинхронном режиме это обычно происходит до полноценного
	 * установления соединения, но движок уже готов буферизовать данные
	 * и как только соединение будет завершено, данные будут отправлены.
	 * Если соединение установить не удасться, то будет вызван обработчик
	 * onError()
	 */
	virtual void onConnect();
	
	/**
	 * Обработчик ошибки соединения
	 * 
	 * Этот обработчик вызывается после неудачного вызова connect()
	 * 
	 * @param err код ошибки errno
	 */
	virtual void onConnectFault(int err);
	
	/**
	 * Обработчик ошибки в асинхронном потоке
	 *
	 * @param err код ошибки errno
	 */
	virtual void onAsyncError(int err);
	
	/**
	* Обработчик прочитанных данных
	*/
	virtual void onRead(const char *data, size_t len) = 0;
	
	/**
	* Обработчик события опустошения выходного буфера
	*
	* Вызывается после того как все данные были записаны в файл/сокет
	*/
	virtual void onEmpty();
	
	/**
	* Пир (peer) закрыл поток.
	*
	* Мы уже ничего не можем отправить в ответ,
	* можем только корректно закрыть соединение с нашей стороны.
	*/
	virtual void onPeerDown() = 0;

	/**
	* Передать данные в файловый буфер
	*
	* Данные записываются сначала в файловый буфер и только потом отправляются.
	* Для обеспечения целостности переданный блок либо записывается целиком
	* и функция возвращает TRUE, либо ничего не записывается и функция
	* возвращает FALSE
	*
	* @param data указатель на данные
	* @param len размер данных
	* @return TRUE данные приняты, FALSE данные не приняты - нет места
	*/
	bool putInBuffer(const char *data, size_t len);
public:
	
	/**
	* Для shutdown()
	*/
	enum { READ = 1, WRITE = 2 };
	
	/**
	* Конструктор
	*/
	AsyncStream(int afd);
	
	/**
	* Деструктор
	*/
	virtual ~AsyncStream();
	
	/**
	 * Установить соединение
	 * @param sa указатель на структуру sockaddr
	 * @param sa_len длина структуры sockaddr
	 */
	bool connectEx(void *sa, size_t sa_len);
	
	/**
	 * Установить соединение с сервером
	 * @param ip IP-адрес хоста
	 * @param port порт
	 * @return TRUE - соединение открыто, FALSE - произошла ошибка
	 */
	bool connectTo(unsigned int ip, unsigned int port);
	
	/**
	 * Установить TCP-соединение с хостом
	 * @param host доменное имя или IP-адрес хоста
	 * @param port порт
	 */
	bool connectTo(const char *host, unsigned int port);

	/**
	* Записать данные
	*
	* При необходимости данные обрабатываются (сжимаются, шифруются и т.п.)
	* и записываются в файловый буфер. Фактическая отправка данных будет
	* когда сокет готов будет принять очередную порцию данных. Эта функция
	* старается принять все переданные данные и возвращает TRUE если это удалось.
	* Если принять данные не удалось, то возвращается FALSE. В случае
	* использования сжатия и/или компрессии невозможно точно установить
	* какая часть данных была записана в буфер.
	*
	* @param data указатель на данные
	* @param len размер данных
	* @return TRUE данные приняты, FALSE данные не приняты - нет места
	*/
	bool put(const char *data, size_t len);
	
	/**
	* Завершить чтение/запись
	* @note только для сокетов
	*/
	void shutdown(int how);
	
	/**
	* Закрыть поток
	*/
	void close();
};

#endif // NANOSOFT_ASYNCSTREAM_H
