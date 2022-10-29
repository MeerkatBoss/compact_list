#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"
#include "cringe.h"
#include "logger.h"

typedef void* (*member_func)(void* instance);

struct cringe
{
    size_t number;
    const compact_list* list;
    member_func function;
};

static const uintptr_t BUILDER_SIG = 0xbaf14894ad27f6ef;
static const uintptr_t FACTORY_SIG = 0xb9d28061cefd381b;
static const uintptr_t GETTER_SIG  = 0xd1ab01ca1c0c0a55;

static const char guid1[] = "94260767-d212-4eff-bffd-92babd805c49";
static const char guid2[] = "bea3701c-1147-48bb-974c-792312afd323";
static const char guid3[] = "6799bcb4-925a-449c-a289-7dbe90129843";
static const char guid4[] = "42520269-c3b7-4bf0-8f73-4e435db77568";

static void* cringe_dispatcher(void*);
static void* return_self(void*);
static void* get_iterator(void*);

void* abstract_linked_list_iterator_object_getter_factory_builder_getter_get_builder(void* param1, void* param2, void* param3)
{
    cringe* result = (cringe*)calloc(1, sizeof(*result));

    *result = {
        .number = (size_t)param2,
        .list   = (const compact_list*)param1,
        .function  = return_self
    };

    if (strcmp(guid3, (const char*) param3) != 0)
        result->function = cringe_dispatcher;

    return (void*) ((uintptr_t)result ^ BUILDER_SIG);
}

void* abstract_linked_list_iterator_object_getter_factory_builder_build_factory(void* param1, void* param2)
{
    cringe* result = (cringe*)((uintptr_t)param1 ^ BUILDER_SIG);
    result = (cringe*) result->function(result);

    if (strcmp(guid1, (const char*) param2) != 0)
        result->function = cringe_dispatcher;

    return (void*) ((uintptr_t)result ^ FACTORY_SIG);
}

void* abstract_linked_list_iterator_object_getter_factory_make_object_getter(void* param1, void* param2)
{
    cringe* result = (cringe*)((uintptr_t)param1 ^ FACTORY_SIG);
    result = (cringe*) result->function(result);

    if (strcmp(guid2, (const char*) param2) != 0)
        result->function = cringe_dispatcher;
    else
        result->function = get_iterator;

    return (void*) ((uintptr_t)result ^ GETTER_SIG);
}

void* abstract_linked_list_iterator_object_getter_get_abstract_iterator_object(void* param1, void* param2)
{
    cringe* result = (cringe*)((uintptr_t)param1 ^ GETTER_SIG);
    list_iterator iterator = (list_iterator) result->function(result);

    if (strcmp(guid4, (const char*) param2) != 0)
        cringe_dispatcher(NULL);

    return (void*) iterator;
}

static void* return_self(void* param) { return param; }

static void* get_iterator(void* param)
{
    cringe* cng = (cringe*) param;

    size_t cur = 0;

    for (list_iterator it = list_begin(cng->list); it != 0; it = next_element(cng->list, it))
        if (cur++ == cng->number)
            return (void*) it;
        
    cringe_dispatcher(NULL);
}

static void segfault(void);
static void call_random(void);
static void wrong_return(void);
static void divide_by_zero(void);
static void stack_overflow(void);

typedef void (*procedure)(void);

static procedure make_cringe[] = {
    segfault,
    call_random,
    wrong_return,
    divide_by_zero,
    stack_overflow
};

static const size_t cringe_cnt = sizeof(make_cringe)/sizeof(*make_cringe);

static void* cringe_dispatcher(void*)
{
    make_cringe[rand()%cringe_cnt]();
    return NULL;
}

static void segfault(void)
{
    int* ptr = NULL;
    *ptr = 0;

    printf("%d", *ptr); /*Avoid compiler optimization*/
}

static void call_random(void)
{
    uintptr_t addr = (rand() << 16) + rand();
    procedure proc = (procedure) addr;

    proc();
}

static void wrong_return(void)
{
    int pivot = 0;

    memset(&pivot, rand(), 1024);

    return;
}

static void divide_by_zero(void)
{
    int x = 1;
    int y = 0;
    int c = x/y;

    printf("%d", c);    /*Avoid compiler optimization*/
}

static void stack_overflow(void)
{
    stack_overflow();
    stack_overflow();

    puts("cringe");     /*Avoid compiler optimization*/
}