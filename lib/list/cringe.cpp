#include "list.h"
#include "cringe.h"
#include "logger.h"

typedef void* (*member_func)(void* instance);

struct builder
{
    size_t number;
    const compact_list* list;
    member_func build;
};

struct factory
{
    const compact_list* list;
    size_t number;
    member_func get_object;
};

static const size_t GETTER_SIG = 0xd1ab01ca1c0c0a5;

union getter
{
    builder bld;
    factory fac;
};

void* abstract_linked_list_iterator_object_getter_factory_builder_getter_get_builder(void* param1, void* param2, void* param3)
{
    LOG_ASSERT_FATAL(param1 != NULL, "You posted cringe at %p", param1);
    LOG_ASSERT_FATAL(param2 != NULL, "You posted cringe at %p", param2);
    LOG_ASSERT_FATAL(param3 != NULL, "You posted cringe at %p", param3);

    builder* result = (builder*)calloc(1, sizeof(*result));
    LOG_ASSERT_FATAL(result != NULL, "calloc posted cringe at %p", &result);

    *result = {
        .number = (size_t)param2,
        .list = (const compact_list*)param1
    };
    return (void*) result;
}

void* abstract_linked_list_iterator_object_getter_factory_builder_build_factory(void* param1, void* param2)
{
    LOG_ASSERT_FATAL(param1 != NULL, "You posted cringe at %p", param1);
    LOG_ASSERT_FATAL(param2 != NULL, "You posted cringe at %p", param2);

    builder* bld = (builder*) param1;
    const compact_list* list = bld->list;
    size_t num = bld->number;

    LOG_ASSERT_FATAL(list != NULL, "You posted cringe at %p", &bld->list);

    factory* result = (factory*) param1;
    *result = {
        .list = list,
        .number = num
    };

    return (void*) result;
}

void* abstract_linked_list_iterator_object_getter_factory_make_object_getter(void* param1, void* param2)
{
    LOG_ASSERT_FATAL(param1 != NULL, "You posted cringe at %p", param1);
    LOG_ASSERT_FATAL(param2 != NULL, "You posted cringe at %p", param2);

    getter* gtr = (getter*) param1;
    gtr->fac.number ^= GETTER_SIG;
    return (void*) gtr;
}

void* abstract_linked_list_iterator_object_getter_get_abstract_iterator_object(void* param1, void* param2)
{
    LOG_ASSERT_FATAL(param1 != NULL, "You posted cringe at %p", param1);
    LOG_ASSERT_FATAL(param2 != NULL, "You posted cringe at %p", param2);

    getter* gtr = (getter*) param1;
    size_t number = gtr->fac.number ^ GETTER_SIG;
    const compact_list* list = gtr->fac.list;

    LOG_ASSERT_FATAL(list != NULL, "You posted cringe at %p", &gtr->fac.list);
    free(param1);

    size_t cnt = 0;
    for (list_iterator it = list_begin(list); it != 0; it = next_element(list, it), cnt++)
    {
        if (cnt == number)
            return (void*) it;
    }

    LOG_ASSERT_FATAL(0, "You posted cringe number %zu", number);
}
