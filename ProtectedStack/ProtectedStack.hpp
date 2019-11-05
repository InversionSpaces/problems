#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <string.h>

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

// typedef типа элемента стэка
//typedef int stack_el_t;

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

// Код ошибки выхода программы при срабатывании PS_ASSERT
// Больше не возвращает
#define ASSERT_EXIT_CODE (2)

// Напечатать причину проверки
#define PRINT_REASON(reason) {						\
	if (reason == COMMON)							\
		printf("## COMMON CHECK REASON;\n");		\
	if (reason == BEFORE_INIT)						\
		printf("## BEFORE_INIT CHECK REASON;\n");	\
	if (reason == AFTER_INIT)						\
		printf("## AFTER_INIT CHECK REASON;\n");	\
	if (reason == BEFORE_DEINIT)					\
		printf("## BEFORE_DEINIT CHECK REASON;\n");	\
	if (reason == BEFORE_PUSH)						\
		printf("## BEFORE_PUSH CHECK REASON;\n");	\
	if (reason == AFTER_PUSH)						\
		printf("## AFTER_PUSH CHECK REASON;\n");	\
	if (reason == BEFORE_POP)						\
		printf("## BEFORE_POP CHECK REASON;\n");	\
	if (reason == AFTER_POP)						\
		printf("## AFTER_POP CHECK REASON;\n");		\
	if (reason == BEFORE_HASH)						\
		printf("## BEFORE_HASH CHECK REASON;\n");	\
}

// PS_ASSERT проверяет стэк
#ifndef PS_NDEBUG
#define PS_ASSERT(stackp, reason) {					\
	PS_ERROR err = PStackCheck(stackp, reason);		\
	if (err & ~NO_ERROR) {							\
		printf("## STACK ASSERT FAILED;\n");		\
		PRINT_REASON(reason)						\
		printf("## IN FUNCTION %s;\n", __func__);	\
        printf("## ON LINE %d;\n", __LINE__);         \
		PStackDump(stackp, err);					\
        return err;                                 \
		/*exit(ASSERT_EXIT_CODE);*/					\
	}												\
}
#else
#define PS_ASSERT(stackp, reason) ;
#endif

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
inline void HashLyAdd(uint32_t* hash, const void* data, size_t size)
{
	assert(hash != NULL);
	assert(data != NULL);
	
    for(const uint8_t* i = reinterpret_cast<const uint8_t*>(data); size--; ++i)
        *hash = (*hash * 1664525) + reinterpret_cast<unsigned char>(*i) + 1013904223;
}

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

// Вывод ошибок
#define PRINT_ERRORS(error) {					\
	if (error == NO_ERROR)						\
		printf("## NO ERROR;\n");				\
	else 										\
		printf("## ERRORS OCCURED:\n");			\
	if (error & NULL_STACKP)					\
		printf("## NULL STACK POINTER;\n");		\
	if (error & NULL_ARRAYP)					\
		printf("## NULL ARRAY POINTER;\n");		\
	if (error & NULL_CAPACITY)					\
		printf("## NULL CAPACITY;\n");			\
	if (error & TOO_BIG_SIZE)					\
		printf("## TOO BIG SIZE;\n");			\
	if (error & TOO_SMALL_SIZE)					\
		printf("## TOO SMALL SIZE;\n");			\
	if (error & GUARD_GORRUPTED)				\
		printf("## GUARD BYTE CORRUPTED;\n");	\
	if (error & HASH_NOT_MATCH)					\
		printf("## HASH DOESNT MATCH;\n");		\
}

// Вывод capacity и size
#define PRINT_CAPACITY_AND_SIZE(stackp)				\
	printf("## Capacity:\t%lu;\n## Size:\t%lu;\n",	\
				stackp->capacity, stackp->size);

// Вывод "защитных" полей структуры
#ifndef PS_NDEBUG			
#define PRINT_STRUCT_GUARDS(stackp)									\
	printf("## Struct guards (should all be |%x|):", GUARD_BYTE);	\
	printf("\n## Front guards:\t");									\
	uint8_t* ptr = stackp->front_guard;								\
	for (uint8_t* i = ptr; i < ptr + ARRAY_OFFSET; ++i)				\
		printf("|%x|", *i);											\
	printf("\n## Back guards:\t\t");								\
	ptr = stackp->back_guard;										\
	for (uint8_t* i = ptr; i < ptr + ARRAY_OFFSET; ++i)				\
		printf("|%x|", *i);											\
	printf("\n");
#else
#define PRINT_STRUCT_GUARDS(stackp)								\
	printf("## No struct guards support\n");
#endif

// Вывод "защитных" полей массива
#ifndef PS_NDEBUG
#define PRINT_ARRAY_GUARDS(stackp)	{								\
	printf("## Array guards (should all be |%x|):", GUARD_BYTE);	\
	printf("\n## Front guards:\t");									\
	uint8_t* ptr = ((uint8_t*)stackp->array) - ARRAY_OFFSET;		\
	for (uint8_t* i = ptr; i < ptr + ARRAY_OFFSET; ++i)				\
		printf("|%x|", *i);											\
	printf("\n## Back guards:\t\t");								\
	ptr = (uint8_t*)(stackp->array + stackp->capacity);				\
	for (uint8_t* i = ptr; i < ptr + ARRAY_OFFSET; ++i)				\
		printf("|%x|", *i);											\
	printf("\n");													\
}
#else
#define PRINT_ARRAY_GUARDS(stackp)								\
	printf("## No array guards support\n");
#endif

// Вывод хэша
#ifndef PS_NDEBUG
#define PRINT_HASH(stackp) {					                    \
    uint32_t hash = 0;                                              \
    PStackCalcHash(stackp, &hash);                                  \
	printf("## Hash:\t%u (Should be %u);\n", stackp->hash, hash);   \
}
#else 													
#define PRINT_HASH(stackp) 						\
	printf("## No hash support;\n");
#endif

// Вывод элементов массива
#define PRINT_ELEMENTS(stackp) {								\
	printf("## Elements:\n");									\
	for (size_t i = 0; i < stackp->size; i++) {					\
		printf("## + [%lu]\t%d", i, stackp->array[i]);			\
		if (IsDead(stackp->array + i, sizeof(stack_el_t)))		\
			printf(" (POSSIBLY DEAD)");							\
		printf("\n");											\
	}															\
	for (size_t i = stackp->size; i < stackp->capacity; i++) {	\
		printf("## - [%lu]\t%d", i, stackp->array[i]);			\
		if (IsDead(stackp->array + i, sizeof(stack_el_t)))		\
			printf(" (DEAD)");									\
		printf("\n");											\
	}															\
}
			
// Вывод имени	
#ifndef PS_NDEBUG
#define PRINT_NAME(stackp) printf("## Stack name:\t%s;\n", \
										stackp->name);
#else
#define PRINT_NAME(stackp) printf("## No name support;\n");
#endif

inline int IsGuard(const void* ptr, size_t size)
{
#ifndef PS_NDEBUG
	for (const uint8_t* tmp = reinterpret_cast<const uint8_t*>(ptr); size--; ++tmp)
		if (*tmp != GUARD_BYTE)
			return 0;
	return 1;
#else
	return 1;
#endif
}

inline int IsDead(const void* ptr, size_t size) 
{
#ifndef PS_NDEBUG
	for (const uint8_t* tmp = reinterpret_cast<const uint8_t*>(ptr); size--; ++tmp)
		if (*tmp != DEAD_BYTE)
			return 0;
	return 1;
#else
	return 0;
#endif
}

inline PS_ERROR PStackCalcHash(PStack_t* stackp, uint32_t* hash)
{
    assert(hash != NULL);
	PS_ASSERT(stackp, BEFORE_HASH)

    *hash = 0;

#ifndef PS_NDEBUG	
	HashLyAdd(hash, stackp->name, 
		strlen(stackp->name));
#endif

	HashLyAdd(hash, &stackp->capacity, 
		sizeof(stackp->capacity));
		
	HashLyAdd(hash, &stackp->size, 
		sizeof(stackp->size));

#ifndef PS_NDEBUG
	HashLyAdd(hash, stackp->array, 
		sizeof(stack_el_t) * stackp->capacity);
#else
	HashLyAdd(hash, stackp->array, 
		sizeof(stack_el_t) * stackp->size);
#endif	
	
	return NO_ERROR;
}

inline PS_ERROR PStackCheck(PStack_t* stackp, PS_CHECK_REASON reason)
{
	PS_ERROR retval = NO_ERROR;
	
	if (stackp == NULL) {
		retval |= NULL_STACKP;
		return retval;
	}

	if (reason & ~BEFORE_INIT) {
		if (stackp->capacity < stackp->size)
			retval |= TOO_BIG_SIZE;
				
		if (stackp->capacity == 0)
			retval |= NULL_CAPACITY;
			
		if (stackp->array == NULL)
			retval |= NULL_ARRAYP;
#ifndef PS_NDEBUG
		else {
			uint8_t* start 	= reinterpret_cast<uint8_t*>(stackp->array) - ARRAY_OFFSET;
			uint8_t* end	= reinterpret_cast<uint8_t*>(stackp->array + stackp->capacity);
			
			if (!IsGuard(start, ARRAY_OFFSET) ||
				!IsGuard(end,	ARRAY_OFFSET))
				retval |= GUARD_GORRUPTED;
		}
		
		if (!IsGuard(stackp->front_guard, 	ARRAY_OFFSET) ||
			!IsGuard(stackp->back_guard,	ARRAY_OFFSET))
			retval |= GUARD_GORRUPTED;
			
		if (reason & ~BEFORE_HASH) {
            uint32_t hash = 0;
            PStackCalcHash(stackp, &hash);
			if (hash != stackp->hash)
				retval |= HASH_NOT_MATCH;
        }
#endif
	}
	
	if (reason & BEFORE_PUSH)
		if (stackp->capacity <= stackp->size)
			retval |= TOO_BIG_SIZE;
			
	if (reason & BEFORE_POP)
		if (stackp->size == 0)
			retval |= TOO_SMALL_SIZE;
		
	return retval;
}

inline void PStackDump(PStack_t* stackp, PS_ERROR error)
{
	printf("## STACK DUMP;\n");

	PRINT_ERRORS(error)
	
	// Иначе нет смысла что-то писать
	if (error & NULL_STACKP)
		return;
		
	PRINT_NAME(stackp)
	PRINT_CAPACITY_AND_SIZE(stackp)
	PRINT_STRUCT_GUARDS(stackp)
	
	// Иначе в рекурсию войдём
	if (!(error & ~(HASH_NOT_MATCH | NO_ERROR))) 
		PRINT_HASH(stackp)
	
	// Иначе не знаешь, сколько выписывать
	if (!(error & (NULL_ARRAYP | TOO_BIG_SIZE | TOO_SMALL_SIZE))) {
		PRINT_ELEMENTS(stackp)
		PRINT_ARRAY_GUARDS(stackp)
	}
}

inline PS_ERROR PStackInit(PStack_t* stackp, size_t capacity
#ifndef PS_NDEBUG
, const char* name
#endif
)
{
	PS_ASSERT(stackp, BEFORE_INIT)
	
	stackp->capacity 	= capacity;
	stackp->size 		= 0;
	
	// Размер области памяти с данными
	size_t elem_size = capacity * sizeof(stack_el_t);
	
	// Размер всей области памяти
	size_t full_size = elem_size + 2 * ARRAY_OFFSET;
	
	// Аллоцируем память
	uint8_t* tmp_ptr 	= reinterpret_cast<uint8_t*>(malloc(full_size));
	
	// Присваиваем указатель на данные
	stackp->array 		= reinterpret_cast<stack_el_t*>(tmp_ptr + ARRAY_OFFSET);
		
#ifndef PS_NDEBUG
	// Начало "защитной" области памяти спереди
	uint8_t* start 	= tmp_ptr;
	
	// Начало "защитной" области памяти сзади
	uint8_t* end	= tmp_ptr + ARRAY_OFFSET + elem_size;

	// Заполняем области соответственно
	memset(start, 			GUARD_BYTE, ARRAY_OFFSET);
	memset(stackp->array, 	DEAD_BYTE, 	elem_size);
	memset(end, 			GUARD_BYTE, ARRAY_OFFSET);
	
	// Заполняем защитные области структуры
	memset(stackp->front_guard, GUARD_BYTE, ARRAY_OFFSET);
	memset(stackp->back_guard,  GUARD_BYTE, ARRAY_OFFSET);
	
    assert(name != NULL);
    
	stackp->name = name;
	
    PStackCalcHash(stackp, &(stackp->hash));
#endif
	
	PS_ASSERT(stackp, AFTER_INIT)
    
    return NO_ERROR;
}

inline PS_ERROR PStackPush(PStack_t* stackp, stack_el_t elem)
{
	PS_ASSERT(stackp, BEFORE_PUSH)
	
	stackp->array[stackp->size++] = elem;
	
#ifndef PS_NDEBUG
	PStackCalcHash(stackp, &(stackp->hash));
#endif

	if (stackp->size == stackp->capacity) {
        int reserved = 0;
		PStackReserve(stackp, stackp->capacity * 2, &reserved);
        if (!reserved)
            return TOO_BIG_SIZE;
    }
	
	PS_ASSERT(stackp, AFTER_PUSH)
    
    return NO_ERROR;
}

inline PS_ERROR PStackPop(PStack_t* stackp, stack_el_t* elem)
{
    assert(elem != NULL);
	PS_ASSERT(stackp, BEFORE_POP)
	
	*elem = stackp->array[--stackp->size];
	
#ifndef PS_NDEBUG
	stack_el_t* el_ptr = stackp->array + stackp->size;
	memset(el_ptr, DEAD_BYTE, sizeof(stack_el_t));

	PStackCalcHash(stackp, &(stackp->hash));
#endif
	
	PS_ASSERT(stackp, AFTER_POP)
	
	return NO_ERROR;
}

inline PS_ERROR PStackReserve(PStack_t* stackp, size_t capacity, int* done)
{	
    assert(done != NULL);
	PS_ASSERT(stackp, COMMON)

	// Сужение пока не поддерживается
	assert(stackp->capacity < capacity);
	
	// Размер новой области с данными в байтах
	size_t data_size = capacity * sizeof(stack_el_t);
	// Размер всей новой области в байтах
	size_t new_size	 = data_size + 2 * ARRAY_OFFSET;

	// Вычисляем настоящий указатель на выделенную память
	uint8_t* real_ptr 	= reinterpret_cast<uint8_t*>(stackp->array) - ARRAY_OFFSET;
	// Реаллоцируем память
	uint8_t* tmp_ptr 	= reinterpret_cast<uint8_t*>(realloc(real_ptr, new_size));
	
	if (tmp_ptr == NULL) {
		*done = 0;
        return NO_ERROR;
    }
	
	// Сохраняем старую вместимость
	size_t old_capacity = stackp->capacity;
	
	// Обновляем поля стэка
	stackp->array = reinterpret_cast<stack_el_t*>(tmp_ptr + ARRAY_OFFSET);
	stackp->capacity = capacity;
	
#ifndef PS_NDEBUG
	// Начало защитной области памяти сзади
	uint8_t* guard_start = reinterpret_cast<uint8_t*>(stackp->array) + data_size;
	
	// Заполняем эту область
	memset(guard_start, GUARD_BYTE, ARRAY_OFFSET);
	
	// Окончание области данных в старой памяти
	uint8_t* data_end = reinterpret_cast<uint8_t*>(stackp->array + old_capacity);
	
	// Размер области, которую надо заполнить "мёртвыми" байтами
	size_t dead_size = guard_start - data_end;
	
	// Заполняем эту область
	memset(data_end, DEAD_BYTE, dead_size);
	
	// Пересчитываем хэш
    PStackCalcHash(stackp, &(stackp->hash));
#endif
	
	PS_ASSERT(stackp, COMMON)
	
    *done = 1;
	return NO_ERROR;
}

inline PS_ERROR PStackIsEmpty(PStack_t* stackp, int* empty)
{
    assert(empty != NULL);
	PS_ASSERT(stackp, COMMON);
	
    *empty = (stackp->size == 0);
    
	return NO_ERROR;
}

inline PS_ERROR PStackDeInit(PStack_t* stackp)
{
	PS_ASSERT(stackp, BEFORE_DEINIT);
	
	// Вычисляем настоящий указатель на выделенную память
	uint8_t* real_ptr = reinterpret_cast<uint8_t*>(stackp->array) - ARRAY_OFFSET;
	free(real_ptr);
	
	// Зануляем переменные
	stackp->size 		= 0;
	stackp->capacity 	= 0;
	stackp->array		= NULL;
    
    return NO_ERROR;
}
