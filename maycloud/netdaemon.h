#ifndef NANOSOFT_NETDAEMON_H
#define NANOSOFT_NETDAEMON_H

#include <maycloud/asyncobject.h>
#include <maycloud/error.h>
#include <maycloud/object.h>
#include <maycloud/config.h>
#include <maycloud/easylib.h>

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <iostream>
#include <queue>

#ifdef HAVE_GNUTLS
#include <gnutls/gnutls.h>
#endif // HAVE_GNUTLS

/**
* Callback таймера
*/
typedef void (*timer_callback_t) (const timeval &tv, void *data);

/**
* callback обработчика завершения подпроцесса
*/
typedef void (*exit_callback_t)(pid_t pid, int exit_code, void *data);

/**
* Главный класс сетевого демона
*/
class NetDaemon
{
private:
	/**
	* Файловый дескриптор epoll
	*/
	int epoll;
	
	/**
	* Активность демона
	* TRUE - активен, FALSE остановлен или в процессе останова
	*/
	bool active;
	
	struct timer
	{
		/**
		* Время когда надо активировать таймер
		*/
		int expires;
		
		/**
		* Callback таймера
		*/
		timer_callback_t callback;
		
		/**
		* Указатель на пользовательские данные
		*/
		void *data;
		
		/**
		* Конструктор по умолчанию
		*/
		timer() {
		}
		
		/**
		* Конструктор
		*/
		timer(int aExpires, timer_callback_t aCallback, void *aData):
			expires(aExpires), callback(aCallback), data(aData)
		{
		}
		
		/**
		* Оператор сравнения для приоритетной очереди
		*/
		bool operator < (const timer &t) const {
			return expires < t.expires;
		}
		
		/**
		* Запустить таймер
		*/
		void fire(const timeval &tv) {
			callback(tv, data);
		}
	};
	
	/**
	* Очередь таймеров
	*/
	typedef std::priority_queue<timer> timers_queue_t;
	
	/**
	* Таймеры
	*/
	timers_queue_t timers;
	
	/**
	 * Глобальный таймер
	 */
	timer_callback_t gtimer;
	
	/**
	 * Пользовательские данные для глобального таймера
	 */
	void *gtimer_data;
	
	/**
	* Максимальное число обслуживаемых объектов
	*/
	size_t limit;
	
	/**
	* Текущее число объектов
	*/
	size_t count;
	
	/**
	* Итератор объектов для корректного останова
	*/
	size_t iter;
	
	/**
	* Число таймеров в очереди
	*/
	int timerCount;
	
	/**
	* Структура описывающая один блок буфера
	*/
	struct block_t
	{
		/**
		* Ссылка на следующий блок
		*/
		block_t *next;
		
		/**
		* Данные блока
		*/
		char data[FDBUFFER_BLOCK_SIZE];
	};
	
	/**
	* Структура описывающая файловый дескриптор
	*/
	struct fd_info_t
	{
		/**
		* Указатель на объект
		*/
		ptr<AsyncObject> obj;
		
		/**
		* Размер буферизованных данных (в байтах)
		*/
		size_t size;
		
		/**
		* Смещение в блоке к началу не записанных данных
		*/
		size_t offset;
		
		/**
		* Размер квоты для файлового дескриптора (в байтах)
		*/
		size_t quota;
		
		/**
		* Указатель на первый блок данных
		*/
		block_t *first;
		
		/**
		* Указатель на последний блок данных
		*/
		block_t *last;
	};
	
	/**
	* Размер буфера (в блоках)
	*/
	size_t buffer_size;
	
	/**
	* Число свободных блоков
	*/
	size_t free_blocks;
	
	/**
	* Буфер
	*/
	block_t *buffer;
	
	/**
	* Стек свободных блоков
	*/
	block_t *stack;
	
	/**
	* Таблица файловых дескрипторов
	*
	* хранит указатели на первый блок данных дескриптора или NULL
	* если нет буферизованных данных
	*/
	fd_info_t *fds;
	
	/**
	* Действие активного цикла
	*/
	void doActiveAction();
	
	/**
	* Возобновить работу с асинхронным объектом
	*/
	bool resetObject(ptr<AsyncObject> &object);
	
	/**
	* Установить таймер
	* @param calltime время запуска таймера
	* @param callback функция обратного вызова
	* @param data указатель на пользовательские данные
	* @return TRUE - таймер установлен, FALSE - таймер установить не удалось
	*/
	bool callAt(time_t calltime, timer_callback_t callback, void *data);
	
	/**
	* Обработать таймеры
	*/
	void processTimers();
	
	/**
	* Выделить цепочку блоков достаточную для буферизации указаного размера
	* @param size требуемый размер в байтах
	* @return список блоков или NULL если невозможно выделить запрощенный размер
	*/
	block_t* allocBlocks(size_t size);
	
	/**
	* Освободить цепочку блоков
	* @param top цепочка блоков
	*/
	void freeBlocks(block_t *top);
	
	/**
	* Добавить данные в буфер (thread-unsafe)
	*
	* @param fd файловый дескриптор
	* @param fb указатель на описание файлового буфера
	* @param data указатель на данные
	* @param len размер данных
	* @return TRUE данные приняты, FALSE данные не приняты - нет места
	*/
	bool put(int fd, fd_info_t *fb, const char *data, size_t len);
protected:
	/**
	* Обработка системной ошибки
	*/
	void stderror();
	
public:
	/**
	* Конструктор демона
	* @param fd_limit максимальное число одновременных виртуальных потоков
	* @param buf_size размер файлового буфера в блоках
	*/
	NetDaemon(int fd_limit, int buf_size);
	
	/**
	* Деструктор демона
	*/
	virtual ~NetDaemon();
	
	/**
	* Вернуть число подконтрольных объектов
	*/
	int getObjectCount();
	
	/**
	* Вернуть максимальное число подконтрольных объектов
	*/
	int getObjectLimit() { return limit; }
	
	/**
	* Вернуть размер буфера в блоках
	*/
	int getBufferSize() const { return buffer_size; }
	
	/**
	* Добавить асинхронный объект
	*/
	bool addObject(ptr<AsyncObject> object);
	
	/**
	* Добавить асинхронный объект
	*/
	bool addObject(ptr<AsyncObject> object, bool enabled);
	
	/**
	* Активировать объект
	*/
	bool enableObject(ptr<AsyncObject> object);
	
	/**
	* Деактивировать объект
	*
	* Объект остается под контролем демона и остается в его внутренних
	* структурах, но на время отключается от epoll
	*/
	bool disableObject(ptr<AsyncObject> object);
	
	/**
	* Уведомить NetDaemon, что объект изменил свою маску
	*/
	bool modifyObject(ptr<AsyncObject> object);
	
	/**
	* Удалить асинхронный объект
	*/
	bool removeObject(ptr<AsyncObject> object);
	
	/**
	* Запустить демона
	*/
	int run();
	
	/**
	* Вернуть статус демона
	* TRUE - активен, FALSE отключен или в процессе останова
	*/
	bool getDaemonActive() { return active; }
	
	/**
	* Установить таймер однократного вызова
	* 
	* Указанная функция будет вызвана один раз в указанное время.
	* Можно устанавливать несколько таймеров и они будут вызваны в указанное
	* время
	* 
	* @param calltime время запуска таймера
	* @param callback функция обратного вызова
	* @param data указатель на пользовательские данные
	* @return TRUE - таймер установлен, FALSE - таймер установить не удалось
	*/
	template <class data_t>
	bool setTimer(time_t calltime, void (*callback)(const timeval &tv, data_t *data), data_t *data)
	{
		return callAt(calltime, reinterpret_cast<timer_callback_t>(callback), data);
	}
	
	/**
	* Установить глобальный таймер
	* 
	* Указанная функция будет вызываться перодически примерно каждые 200-300мс
	* Повторная установка таймера заменяет предыдущий, таким образом можно
	* указать только один глобальный таймер
	* 
	* @param calltime время запуска таймера
	* @param callback функция обратного вызова
	* @param data указатель на пользовательские данные
	* @return TRUE - таймер установлен, FALSE - таймер установить не удалось
	*/
	template <class data_t>
	bool setGlobalTimer(void (*callback)(const timeval &tv, data_t *data), data_t *data)
	{
		gtimer = reinterpret_cast<timer_callback_t>(callback);
		gtimer_data = data;
	}

	pid_t exec(std::string path, const EasyVector &args, const EasyRow & env, exit_callback_t callback, void *data);
	
	/**
	* Обработчик ошибок
	*
	* По умолчанию выводит все ошибки в stderr
	*/
	virtual void onError(const char *message);
	
	/**
	* Вернуть число свободных блоков в буфере
	* @return число свободных блоков в буфере
	*/
	size_t getFreeSize() { return free_blocks; }
	
	/**
	* Вернуть размер буферизованных данных
	* @param fd файловый дескриптор
	* @return размер буферизованных данных (в байтах)
	*/
	size_t getBufferedSize(int fd);
	
	/**
	* Вернуть квоту файлового дескриптора
	* @param fd файловый дескриптор
	* @return размер квоты (в блоках)
	*/
	size_t getQuota(int fd);
	
	/**
	* Установить квоту буфер файлового дескриптора
	* @param fd файловый дескриптор
	* @param quota размер квоты (в блоках)
	* @return TRUE квота установлена, FALSE квота не установлена
	*/
	bool setQuota(int fd, size_t quota);
	
	/**
	* Добавить данные в буфер (thread-safe)
	*
	* @param fd файловый дескриптор в который надо записать
	* @param data указатель на данные
	* @param len размер данных
	* @return TRUE данные приняты, FALSE данные не приняты - нет места
	*/
	bool put(int fd, const char *data, size_t len);
	
	/**
	* Записать данные из буфера в файл/сокет
	*
	* @param fd файловый дескриптор
	* @return TRUE буфер пуст, FALSE в буфере ещё есть данные
	*/
	bool push(int fd);
	
	/**
	* Удалить блоки файлового дескриптора
	*
	* @param fd файловый дескриптор
	*/
	void cleanup(int fd);

	/**
	* Подпроцесс
	*/
	struct process_t {
		pid_t pid;
		exit_callback_t callback;
		void *data;
	};

	typedef std::map<pid_t, process_t> process_list_t;
	typedef std::list<pid_t> pid_list_t;

	/**
	* Список подпроцессов
	*/
	process_list_t processes;
};

#endif // NANOSOFT_NETDAEMON_H
