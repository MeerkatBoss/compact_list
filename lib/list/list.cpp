#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "list.h"
#include "logger.h"
#include "graph.h"

static const size_t default_cap_  = 16;
static const size_t growth_coeff_ = 2;
static const element_t POISON     = NAN;

#define BGCOLOR "lightsteelblue1"
#define NODE_COLOR "springgreen"
#define NODE_HEAD_COLOR "springgreen4"
#define FREE_COLOR "deepskyblue"
#define FREE_HEAD_COLOR "dodgerblue2"
#define HEAD_FONT_COLOR "azure"
#define BORDER_COLOR "darkslategray"
#define NEXT_EDGE_COLOR "deeppink"
#define PREV_EDGE_COLOR "darkviolet"
#define FREE_EDGE_COLOR "midnightblue"
#define ROOT_COLOR "navajowhite"
#define ROOT_HEAD_COLOR "peru"
#define SPEC_HEAD_COLOR "darkslategrey"

#define NODE_FORMAT(name, bg, head) \
                        "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" BGCOLOR=\"" bg "\">\n"\
                        "<TR><TD ALIGN=\"CENTER\" COLSPAN=\"2\" BGCOLOR=\"" head "\"><FONT COLOR=\"" HEAD_FONT_COLOR "\"><B>" name " [%zu]</B></FONT></TD></TR>\n"\
                        "<TR><TD COLSPAN=\"2\"><I>value: </I> %lg</TD></TR>\n"\
                        "<TR><TD><I>next: </I>%zu</TD><TD><I>prev: </I>%zu</TD></TR>\n"\
                        "</TABLE>"

#define NODE_ARGS(i, node) i, node.value, node.next, node.prev

static void try_shrink_list(compact_list* list);
static void try_grow_list(compact_list* list);
static void resize_list(compact_list* list, size_t new_size);
static void memswap(void* mem1, void* mem2, size_t mem_size);

compact_list list_ctor(void)
{
    node* nodes = (node*) calloc(default_cap_, sizeof(*nodes));
    LOG_ASSERT(nodes != NULL, return {});

    nodes[0] = {
        .value   = (element_t) POISON,
        .next    = (list_iterator)0,
        .prev    = (list_iterator)0,
        .is_free = 0
    };
    for (size_t i = 1; i < default_cap_; i++)
        nodes[i] = {
            .value   = (element_t) POISON,
            .next    = (list_iterator) ((i + 1) % default_cap_),
            .prev    = (list_iterator) 0,
            .is_free = 1
        };
    return {
        .nodes    = nodes,
        .free     = (list_iterator) 1,
        .size     = 0,
        .capacity = default_cap_,
        .is_linear= 1
    };
}

void list_dtor(compact_list* list)
{
    LOG_ASSERT(list != NULL, return);

    free(list->nodes);
    memset(list, 0, sizeof(*list));
}

list_iterator list_begin(const compact_list* list)
{
    LOG_ASSERT(list        != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);
    return list->nodes->next;
}

list_iterator list_end(const compact_list* list)
{
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);
    return list->nodes->prev;
}

list_iterator next_element(const compact_list* list, const list_iterator iterator)
{
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);
    return list->nodes[iterator].next;
}

list_iterator prev_element(const compact_list* list, const list_iterator iterator)
{
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);
    return list->nodes[iterator].prev;
}

list_iterator insert_after(compact_list* list, list_iterator iterator, element_t value)
{
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);

    list_iterator cur = iterator;
    list_iterator added = list->free;
    list_iterator nxt = next_element(list, iterator);

    if (!(cur + 1 == added && added + 1 == nxt))
        list->is_linear = 0;

    list->free = next_element(list, list->free);

    list->nodes[added] = {
        .value = value,
        .next = nxt,
        .prev = cur,
        .is_free = 0
    };
    list->nodes[cur].next = added;
    list->nodes[nxt].prev = added;

    list->size++;
    try_grow_list(list);
    return added;
}

list_iterator insert_before(compact_list* list, list_iterator iterator, element_t value)
{
    //LOG_ASSERT(iterator != 0, return 0);
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);

    list_iterator cur = iterator;
    list_iterator added = list->free;
    list_iterator prv = prev_element(list, iterator);

    if(!(prv + 1 == added && added + 1 == cur))
        list->is_linear = 0;

    list->free = next_element(list, list->free);

    list->nodes[added] = {
        .value = value,
        .next = cur,
        .prev = prv,
        .is_free = 0
    };
    list->nodes[cur].prev = added;
    list->nodes[prv].next = added;

    list->size++;
    try_grow_list(list);
    return added;
}

list_iterator push_back(compact_list* list, element_t value)
{
    return insert_after(list, list_end(list), value);
}

list_iterator push_front(compact_list* list, element_t value)
{
    return insert_before(list, list_begin(list), value);
}

element_t get_element(const compact_list* list, const list_iterator iterator)
{
    LOG_ASSERT(iterator != 0, return POISON);
    LOG_ASSERT(list != NULL, return POISON);
    LOG_ASSERT(list->nodes != NULL, return POISON);

    return list->nodes[iterator].value;
}

void erase_element(compact_list* list, list_iterator iterator)
{
    LOG_ASSERT(iterator != 0, return);
    LOG_ASSERT(list != NULL, return);
    LOG_ASSERT(list->nodes != NULL, return);

    list_iterator nxt = next_element(list, iterator);
    list_iterator prv = prev_element(list, iterator);

    list->nodes[prv].next = nxt;
    list->nodes[nxt].prev = prv;

    list->nodes[iterator] = {
        .value = POISON,
        .next = list->free,
        .prev = 0,
        .is_free = 1
    };

    list->free = iterator;

    if (!(nxt == 0 || prv == 0))
        list->is_linear = 0;

    list->size--;
    try_shrink_list(list);
}

void pop_back(compact_list* list)
{
    erase_element(list, list_end(list));
}

void pop_front(compact_list* list)
{
    erase_element(list, list_begin(list));
}

void linearize(compact_list* list)
{
    if (list->is_linear)
        return;

    size_t cnt = 1;
    for (list_iterator it = list_begin(list); it != 0; it = next_element(list, it), cnt++)
    {
        list->nodes[it].prev = cnt;
    }
    for (size_t i = 1; i < list->capacity; i++)
    {
        while(!list->nodes[i].is_free && list->nodes[i].prev != i)
        {
            size_t prv = list->nodes[i].prev;
            memswap(&list->nodes[i], &list->nodes[prv], sizeof(node));
        }
    }

    list->nodes->next = 1;
    list->nodes->prev = list->size;

    for (size_t i = 1; i < list->capacity; i++)
    {
        if (list->nodes[i].is_free)
        {
            list->nodes[i].next = (i + 1) % list->capacity;
            list->nodes[i].prev = 0;
        }
        else
        {
            list->nodes[i].next = (i + 1) % (list->size + 1);
            list->nodes[i].prev = i - 1;
        }
    }

    list->free = list->size + 1;
    list->is_linear = 1;

    try_shrink_list(list);
}

list_iterator element_by_number(const compact_list* list, size_t num)
{
    if (!list->is_linear) return 0;
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);
    LOG_ASSERT(num < list->size, return 0);

    return list_begin(list) + num;
}

int list_check(compact_list* list)
{
    LOG_ASSERT(list != NULL, return 0);
    LOG_ASSERT(list->nodes != NULL, return 0);
    LOG_ASSERT(list->free != 0, return 0);
    LOG_ASSERT(list->size+2 <= list->capacity, return 0);
    LOG_ASSERT(list->capacity >= default_cap_, return 0);

    list_iterator last = 0;
    list_iterator cur = list_begin(0);
    for (size_t i = 0; i < list->size; i++)
    {
        LOG_ASSERT(list->nodes[cur].prev == last, return 0);
        LOG_ASSERT(!list->nodes[cur].is_free, return 0);

        last = cur;
        cur = next_element(list, cur);
    }

    LOG_ASSERT(cur == 0, return 0);

    cur = list->free;
    for (size_t i = list->size + 1; i < list->capacity; i++)
    {
        LOG_ASSERT(list->nodes[cur].prev == 0, return 0);
        LOG_ASSERT(list->nodes[cur].is_free, return 0);
        
        cur = next_element(list, cur);
    }

    LOG_ASSERT(cur == 0, return 0);

    return 1;
}

void list_dump(compact_list* list, const char* filename)
{
    graph_builder builder = get_builder();
    add_property(&builder, "bgcolor=%s", BGCOLOR);
    add_property(&builder, "node [shape=plaintext color=%s]", BORDER_COLOR);
    
    size_t *node_num = (size_t*) calloc(list->capacity, sizeof(*node_num));

    node_num[0] = add_node(&builder,
            "label=<"NODE_FORMAT("root", ROOT_COLOR, ROOT_HEAD_COLOR)">",
            NODE_ARGS(0, list->nodes[0]));
    for (size_t i = 1; i < list->capacity; i++)
    {
        if (list->nodes[i].is_free)
            node_num[i] = add_node(&builder,
                "label=<"NODE_FORMAT("free", FREE_COLOR, FREE_HEAD_COLOR)">",
                NODE_ARGS(i, list->nodes[i]));
        else if (i == list_begin(list))
            node_num[i] = add_node(&builder,
                "label=<"NODE_FORMAT("head", NODE_COLOR, SPEC_HEAD_COLOR)">",
                NODE_ARGS(i, list->nodes[i]));
        else if (i == list_end(list))
            node_num[i] = add_node(&builder,
                "label=<"NODE_FORMAT("tail", NODE_COLOR, SPEC_HEAD_COLOR)">",
                NODE_ARGS(i, list->nodes[i]));
        else
            node_num[i] = add_node(&builder,
                "label=<"NODE_FORMAT("node", NODE_COLOR, NODE_HEAD_COLOR)">",
                NODE_ARGS(i, list->nodes[i]));
    }

    for (size_t i = 0; i < list->capacity; i++)
    {
        if (i + 1 < list->capacity)
            add_edge(&builder, node_num[i], node_num[i + 1], "weight=999999 color=none");
        if (list->nodes[i].is_free)
        {
            add_edge(&builder, node_num[i], node_num[list->nodes[i].next],
                "weight=0 color=%s", FREE_EDGE_COLOR);
            continue;
        }
        add_edge(&builder, node_num[i], node_num[list->nodes[i].next],
            "weight=0 color=%s", NEXT_EDGE_COLOR);
        add_edge(&builder, node_num[i], node_num[list->nodes[i].prev],
            "weight=0 color=%s style=dashed", PREV_EDGE_COLOR);

    }

    draw(&builder, filename);
    builder_dtor(&builder);
    free(node_num);
}

static void try_shrink_list(compact_list* list)
{
    if (!list->is_linear) return;
    if (list->capacity <= default_cap_) return;
    if ((list->size+2)*growth_coeff_ <= default_cap_) return;

    if ((list->size+2) * growth_coeff_ * growth_coeff_ > list->capacity)
        return;
    
    size_t new_size = (list->size+2)*growth_coeff_;

    if (list_begin(list) + list->size + 1 < new_size)
        new_size = list_begin(list) + list->size + 1;

    resize_list(list, new_size);
}

static void try_grow_list(compact_list* list)
{
    if (list->free)
        return;
    
    resize_list(list, list->capacity*growth_coeff_);
}

static void resize_list(compact_list* list, size_t new_size)
{
    size_t old_size = list->capacity;
    list->nodes = (node*) reallocarray(list->nodes, new_size, sizeof(list->nodes));
    list->capacity = new_size;

    for (size_t i = old_size; i < new_size; i++)
        list->nodes[i] = {
            .value   = POISON,
            .next    = (i + 1) % new_size,
            .prev    = 0,
            .is_free = 1
        };

    list_iterator lst_free = list->free;    

    list_iterator nxt = 0;
    while((nxt = next_element(list, lst_free)))
        lst_free = nxt;
    if (old_size > new_size)
        list->nodes[lst_free].next = 0;
    else
        list->nodes[lst_free].next = old_size;
}

static void memswap(void* mem1, void* mem2, size_t mem_size)
{
    char* bytes1 = (char*)mem1;
    char* bytes2 = (char*)mem2;
    for (size_t i = 0; i < mem_size; i++)
    {
        bytes1[i] ^= bytes2[i];
        bytes2[i] ^= bytes1[i];
        bytes1[i] ^= bytes2[i];
    }
}
