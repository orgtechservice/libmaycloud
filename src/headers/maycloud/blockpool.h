#ifndef BLOCKPOOL_H
#define BLOCKPOOL_H

#include <stdint.h>
#include <sys/types.h>

/**
 * Структура, описывающая один блок буфера
 */
struct block_t {
	/**
	 * Ссылка на блок
	 *
	 * Размер блока всегда фиксирован и равен BLOCKPOOL_BLOCK_SIZE
	 */
	uint8_t *data;
	
	/**
	 * Ссылка на следующий блок
	 */
	block_t *next;
};

/**
 * Пул блоков памяти
 *
 * Данный класс предоставляет довольно низкоуровневый API и не обеспечивает
 * безопасность. Пользователь класса должен аккуратно работать с выделенными
 * блоками, не допускать запись информации за пределы блока (т.к. это повредит
 * информацию в других блоках или нарушит целостность всего объекта), не
 * должен пытаться освобождать или перераспределять блоки иным образом,
 * должен возвращать блоки обратно в тот экзепляр пула из котого блоки были
 * выделены.
 *
 * Непосредственная работа пользователя с этим классом не предполагается,
 * он является вспомогательным классом для более высокоуровневых классов
 */
class BlockPool
{
protected:
	
	/**
	 * Структура описывающая пул (макро-блок)
	 */
	struct pool_t {
		/**
		 * Указатель на блок
		 */
		void *data;
		
		/**
		 * Ссылка на следующий пул
		 */
		pool_t *next;
	};
	
	/**
	 * Общее число блоков в пуле
	 */
	size_t total_count;
	
	/**
	 * Число свободных блоков в пуле
	 */
	size_t free_count;
	
	/**
	 * Размер пула в байтах
	 */
	size_t pool_size;
	
	/**
	 * Стек свободных блоков
	 */
	block_t *stack;
	
	/**
	 * Пул макро-блоков
	 */
	pool_t *pools;
	
public:
	
	/**
	 * Конструктор
	 */
	BlockPool();
	
	/**
	 * Деструктор
	 *
	 * Высвобождает все блоки, в том числе те, что выданы клиентам
	 */
	~BlockPool();
	
	/**
	 * Вернуть общее число блоков
	 */
	inline size_t getTotalCount() const { return total_count; }
	
	/**
	 * Вернуть число свободных блоков
	 */
	inline size_t getFreeCount() const { return free_count; }
	
	/**
	 * Вернуть число свободных блоков
	 */
	inline size_t getBusyCount() const { return total_count - free_count; }
	
	/**
	 * Размер пула в байтах
	 */
	inline size_t getPoolSize() const { return pool_size; }
	
	/**
	 * Зарезервировать блоки
	 *
	 * Запрашивает у системы новые блоки и добавляет их в пул
	 */
	int reserve(size_t count);
	
	/**
	 * Зарезервировать блоки
	 *
	 * Запрашивает у системы новые блоки и добавляет их в пул
	 */
	int reserveBySize(size_t size);
	
	/**
	 * Очистить пул
	 *
	 * Высвобождает все блоки, в том числе те, что выданы клиентам
	 */
	void clear();
	
	/**
	 * Выделить цепочку блоков
	 *
	 * @param size требуемый размер в байтах
	 * @return список блоков или NULL если невозможно выделить запрощенный размер
	 */
	block_t* allocBySize(size_t size);
	
	/**
	 * Выделить цепочку блоков
	 *
	 * @param count требуемый размер в блоках
	 * @return список блоков или NULL если невозможно выделить запрощенный размер
	 */
	block_t* allocByBlocks(size_t count);
	
	/**
	 * Освободить цепочку блоков
	 *
	 * NOTE Этот метод не проверяет принадлежат ли ему эти блоки или другому
	 * пулу, просто добавит их в свой пул. Надо быть аккуратным чтобы
	 * возвращать блоки в тот пул из которого они были взяты
	 *
	 * @param list цепочка блоков
	 */
	void free(block_t *list);
	
};

/**
 * Вернуть глобальный пул
 *
 * Для инициализации используйте bpInitPool()
 */
BlockPool* bpPool();

/**
 * Вернуть глобальный пул
 *
 * Если пул не был инициализирован, то он будет автоматически инициализирован,
 * если пул имеет недостаточный размер, то он будет дополнен, если дополнить
 * не удается, то функция вернет NULL.
 */
BlockPool* bpPool(size_t min_size);

/**
 * Инициализация глобального пула
 *
 * Функция bpInitPool() является реэнтерабельной. Если пул еще не был
 * инициализирован, то он будет инициализирован с размером не менее указанного.
 * Если пул уже был инициализирован, то убедится размер соответствует указанному
 * минимальному размеру, если нет, то дополнит его до требуемого размера.
 */
bool bpInitPool(size_t min_size);

#endif // BLOCKPOOL_H
