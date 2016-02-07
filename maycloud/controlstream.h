#ifndef CONTROL_STREAM_H
#define CONTROL_STREAM_H

#include <maycloud/netdaemon.h>
#include <maycloud/asyncxmlstream.h>
#include <maycloud/asyncdns.h>
#include <maycloud/tagbuilder.h>
#include <maycloud/taghelper.h>
#include <maycloud/xmlwriter.h>

class ControlStream: public AsyncXMLStream, protected nanosoft::XMLWriter
{
private:
	/**
	* Состояние потока
	*/
	enum {
		/**
		* Начальное состоение поток
		*
		* Поток не открыт и ещё не может отправлять станзы
		*/
		INIT,
		
		/**
		* Обычное рабочее состояние потока
		*
		* Поток открыт и может отправлять станзы
		*/
		ACTIVE,
		
		/**
		* Состояние закрытия потока
		*
		* Поток закрыт и уже не может отправлять станзы, после того
		* как все данные будут отправлены поток самоудалиться.
		*/
		CLOSING
	} state;
protected:
	/**
	 * Построитель дерева тегов
	 * 
	 * TODO унаследовать от билдера
	 * TODO depth - неотъемлемая часть билдера
	 */
	ATTagBuilder builder;
	
	/**
	 * Глубина обрабатываемого тега
	 * TODO depth - неотъемлемая часть билдера
	 */
	int depth;
	
	/**
	 * Вернуть маску ожидаемых событий
	 */
	virtual uint32_t getEventsMask();
	
	/**
	* Событие: выходной буфер пуст (всё отправлено)
	*/
	virtual void onEmpty();
	
	/**
	 * Запись XML
	 */
	virtual void onWriteXML(const char *data, size_t len);
	
	/**
	* Открыть поток
	*/
	void openStream();
	
	/**
	* Закрыть поток
	*/
	void closeStream();
	
	/**
	 * Сброс потока в начальное состояние
	 */
	void resetStream();
	
	/**
	 * Событие закрытия исходящего потока,
	 * когда мы сказали свое последнее слово
	 */
	virtual void onStreamClosed();
	
	/**
	 * Событие: начало потока
	 */
	virtual void onStartStream(const std::string &name, const attributes_t &attributes) = 0;
	
	/**
	 * Событие: конец потока
	 */
	virtual void onEndStream() = 0;
	
	/**
	 * Обработчик ошибок парсера
	 */
	virtual void onParseError(const char *message);
public:
	/**
	 * Конструктор потока
	 */
	ControlStream(int sock);
	
	/**
	 * Деструктор потока
	 */
	~ControlStream();
	
	/**
	 * Обработчик открытия тега
	 */
	virtual void onStartElement(const std::string &name, const attributes_t &attributes);
	
	/**
	 * Обработчик символьных данных
	 */
	virtual void onCharacterData(const std::string &cdata);
	
	/**
	 * Обработчик закрытия тега
	 */
	virtual void onEndElement(const std::string &name);
	
	/**
	 * Обработчик станзы
	 */
	virtual void onStanza(TagHelper stanza) = 0;
	
	/**
	 * Отправить станзу в поток (thread-safe)
	 * @param stanza станза
	 * @return TRUE - станза отправлена (или буферизована для отправки), FALSE что-то не получилось
	 */
	bool sendStanza(TagHelper stanza);

	bool streamError(std::string from, std::string to, std::string description);
};

#endif // CONTROL_STREAM_H