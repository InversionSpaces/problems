#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <string.h>

// typedef типа элемента стэка
typedef int stack_el_t;

// "Мёртвый" и "защитный" байты
#ifndef PS_NDEBUG
#define DEAD_BYTE (0x4D)
#define GUARD_BYTE (0xBA)
#endif

// Размер канареек у структуры стэка и его массива в байтах
#ifndef PS_NDEBUG
#define ARRAY_OFFSET (5) 
#else
#define ARRAY_OFFSET (0)
#endif

// Структура стэка
struct PStack_t {
#ifndef PS_NDEBUG
	// Передняя "защитная" зона
	uint8_t front_guard[ARRAY_OFFSET];
	// Хэш
	uint32_t hash;
#endif
	
	// Максимальный размер стэка в элементах
	size_t capacity;
	// Текущий размер стэка в элементах
	size_t size;
	
	// Динамический внутренний массив
	stack_el_t* array;
	
#ifndef PS_NDEBUG
	// Имя переменной стэка
	const char* name;
	// Задняя "защитная" зона
	uint8_t back_guard[ARRAY_OFFSET];
#endif
};

// Объявляем тип
typedef struct PStack_t PStack_t;

// Возможные ошибки
#define	NO_ERROR 	    (1)
#define	NULL_STACKP	    (1 << 1)
#define	NULL_ARRAYP 	(1 << 2)
#define	NULL_CAPACITY 	(1 << 3)
#define	TOO_BIG_SIZE	(1 << 4)
#define	TOO_SMALL_SIZE	(1 << 5)
#define	GUARD_GORRUPTED (1 << 6)
#define	HASH_NOT_MATCH 	(1 << 7)

// Тип для ошибок
typedef unsigned long long PS_ERROR;

// Возможные причины проверки стэка
#define	COMMON		    (1)
#define	BEFORE_INIT	    (1 << 1)
#define	AFTER_INIT	    (1 << 2)
#define	BEFORE_DEINIT	(1 << 3)
#define	BEFORE_PUSH	    (1 << 4)
#define	AFTER_PUSH	    (1 << 5)
#define	BEFORE_POP	    (1 << 6)
#define	AFTER_POP	    (1 << 7)
#define	BEFORE_HASH	    (1 << 8)

// Тип для причины проверки
typedef unsigned long long PS_CHECK_REASON;

// Макрос для инициализации стэка - автоматически устанавливает имя
#ifndef PS_NDEBUG
#define PStackInitMACRO(stackp, capacity) \
PStackInit(stackp, capacity, #stackp)
#else
#define PStackInitMACRO(stackp, capacity) \
PStackInit(stackp, capacity)
#endif

// Макрос для вызова PStackDump
#define PStackDumpMACRO(stackp) \
		PStackDump(stackp, PStackCheck(stackp, COMMON))

/*! Функция аддитивного хэширования
 * @param [in, out] hash Указатель на переменную хэша. Начальное значение - 0
 * @param [in] data Указатель на хэшируемые данные
 * @param [in] size Размер данных в байтах
 */
void HashLyAdd(uint32_t* hash, const void* data, size_t size);

/*! Функция проверки данных на то, являются ли они мёртвыми
 * @param [in] ptr Указатель на данные
 * @param [in] size Размер данных
 * @return 1, если они мёртвые, 0 - иначе
 * @note Если существует PS_NDEBUG, все данные считаются не мёртвыми
 */
int IsDead(const void* ptr, size_t size);

/*! Функция проверки данных на то, являются ли они защитными
 * @param [in] ptr Указатель на данные
 * @param [in] size Размер данных
 * @return 1, если они защитные, 0 - иначе
 * @note Если существует PS_NDEBUG, все данные считаются защитными
 */
int IsGuard(const void* ptr, size_t size);

/*! Функция инициализации стэка
 * @param [in, out] stackp Указатель на стэк
 * @param [in] capacity Максимальный размер стэка в элементах
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackInit(PStack_t* stackp, size_t capacity
#ifndef PS_NDEBUG
, const char* name
#endif
);

/*! Функция добавления в стэк
 * @param [in] stackp Указатель на стэк
 * @param [in] elem Добавляемое значение
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackPush(PStack_t* stackp, stack_el_t elem);

/*! Функция удаления из стэка
 * @param [in] stackp Указатель на стэк
 * @param [out] elem Возвращаемый элемент
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackPop(PStack_t* stackp, stack_el_t* elem);

/*! Функция изменения максимального размера стэка
 * @param [in] stackp Указатель на стэк
 * @param [in] capacity Новый максимальный размер в элементах
 * @param [out] done Получилось ли расширить стэк
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackReserve(PStack_t* stackp, size_t capacity, int* done);

/*! Функция проверки стэка на пустоту
 * @param [in] stackp Указатель на стэк
 * @param [out] empty Пустой ли стэк
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackIsEmpty(PStack_t* stackp, int* empty);

/*! Функция деинициализации стэка
 * @param [in] stackp Указатель на стэк
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackDeInit(PStack_t* stackp);

/*! Функция вычисления хэша стэка
 * @param [in] stackp Указатель на стэк
 * @param [out] hash Хэш стэка
 * @return Ошибка работы со стэком
 */
PS_ERROR PStackCalcHash(PStack_t* stackp, uint32_t* hash);

/*! Функция проверки стэка на валидность
 * @param [in] stackp Указатель на стэк
 * @param [in] reason Причина проверки
 * @return Ошибка валидности стэка
 * @note Чтобы просто проверить стэк можно использовать причину COMMON
 */
PS_ERROR PStackCheck(PStack_t* stackp, PS_CHECK_REASON reason);

/*! Функция дампа стэка в stdout
 * @param [in] stackp Указатель на стэк
 */
void PStackDump(PStack_t* stackp, PS_ERROR error);
