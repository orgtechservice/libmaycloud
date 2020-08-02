#ifndef NANOSOFT_NETDAEMON_H
#define NANOSOFT_NETDAEMON_H

#include <maycloud/asyncobject.h>
#include <maycloud/error.h>
#include <maycloud/object.h>
#include <maycloud/config.h>
#include <maycloud/easylib.h>
#include <maycloud/processmanager.h>
#include <maycloud/blockpool.h>

#include <cstdio>
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/wait.h>


/**
* Callback таймера
*/
typedef void (*timer_callback_t) (const timeval &tv, void *data);

/**
* Главный класс сетевого демона
*/
class NetDaemon: public ProcessManager
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
	
	/**
	 * Время ожидания для epoll_wait
	 *
	 * см. setSleepTime() для пояснений
	 */
	int sleep_time;
	
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
		timer(int aExpires, timer_callback_t aCallback, void *aData): expires(aExpires), callback(aCallback), data(aData) {
		}
		
		/**
		* Оператор сравнения
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
	* Карта таймеров
	* @note раньше здесь был priority_queue и это было сильно эффективнее, но создавало проблему:
	* нельзя просто так взять и убрать таймер — ни удалив его, ни пометив его как выключенный добавлением bool-флага
	* С картой стало работать гораздо удобнее, но уже не так эффективно. С другой стороны, а нужна ли здесь эффективность?
	* Если вдруг однажды станет нужна, нужно рассмотреть вариант двух структур — основным будет priority_queue,
	* но вдобавок к нему будет карта указателей для быстрого и удобного обращения/отключения таймера по его ID
	* © WST
	*/
	typedef std::map<long, timer> timer_map_t;
	
	/**
	* Таймеры
	*/
	timer_map_t timers;
	
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
	int limit;
	
	/**
	* Текущее число объектов
	*/
	size_t count;
	
	/**
	* Итератор объектов для корректного останова
	*/
	size_t iter;
	
	/**
	* Текущий ID таймера
	*/
	long timer_id;
	
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
	 * Пул блоков для отложенной отправки
	 */
	BlockPool *bp;

	/**
	 * Вернуть размер буфера в блоках
	 */
	inline int getBufferSize() const {
		return bp ? bp->getPoolSize() : 0;
	}

	/**
	 * Вернуть число свободных блоков в буфере
	 */
	size_t getFreeSize() const {
		return bp ? bp->getFreeCount() : 0;
	}
	
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
	* @return ID таймера
	*/
	long callAt(time_t calltime, timer_callback_t callback, void *data);
	
	/**
	* Обработать таймеры
	*/
	void processTimers();
	
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
	 * Вернуть время ожидания для epoll_wait
	 */
	int getSleepTime() const { return sleep_time; }
	
	/**
	 * Установить время ожидания для epoll_wait
	 *
	 * Оказвыает влияние на частоту таймеров. Если указать
	 * слишком маленькое значение, то частота таймера (глобального)
	 * повыситься, но будет тратиться больше процессорного времени
	 * на обработку таймера. Если указать слишком высокое значение,
	 * то таймеры будут обратываться реже и возможно будут запаздывать.
	 *
	 * Значение по умолчанию (200мс) выбрано из расчета что обычно
	 * не требуется большая точность таймеров и оверхед на CPU нежелателен
	 */
	void setSleepTime(int v);
	
	/**
	* Вернуть число подконтрольных объектов
	*/
	int getObjectCount() const;
	
	/**
	* Вернуть максимальное число подконтрольных объектов
	*/
	int getObjectLimit() const { return limit; }
	
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
	 * Провести некоторые подготовительные действия перед выполнением стороннего процесса
	 */
	virtual int prepareExec();
	
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
	* @return ID таймера — таймер установлен, -1 - таймер установить не удалось
	*/
	template <class data_t>
	long setTimer(time_t calltime, void (*callback)(const timeval &tv, data_t *data), data_t *data) {
		return callAt(calltime, reinterpret_cast<timer_callback_t>(callback), data);
	}

	/** Удалить таймер однократного вызова
	 * @param timer_id ID таймера, подлежащего удалению
	 */
	void removeTimer(long timer_id) {
		timers.erase(timer_id);
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
	*/
	template <class data_t>
	void setGlobalTimer(void (*callback)(const timeval &tv, data_t *data), data_t *data)
	{
		gtimer = reinterpret_cast<timer_callback_t>(callback);
		gtimer_data = data;
	}

	/**
	* Обработчик ошибок
	*
	* По умолчанию выводит все ошибки в stderr
	*/
	virtual void onError(const char *message);
	
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
};

#endif // NANOSOFT_NETDAEMON_H
