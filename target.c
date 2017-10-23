#include <stdlib.h>
#include <assert.h>
#include "target.h"

typedef struct node_st{
  void* target;
  void* dependencies;
  void* rules;
  struct node_st* next;
} node_t;

struct list_struct {
  node_t* first;
  node_t* last;
};

static void list_init(list_t* list){
  assert(list);
  list -> first = NULL;
  list -> last = NULL;
}

list_t* list_new(){
  list_t* list = (list_t*)malloc(sizeof(list_t));
  if(list != NULL){
    list_init(list);
  }
  return list;
}

static void list_fini(list_t* list){
  assert(list != NULL);
  while (list->first != NULL) {
    node_t* node = list-> first;
    list->first = node -> next;
    free(node);
  }
  list -> first = NULL;
  list -> last = NULL;
}

void list_free(list_t* list){
  if(list != NULL){
    list_fini(list);
    free(list);
  }
}

static node_t* node_new(void* target, void* dependencies, void* rules){
  node_t* node = (node_t*)malloc(sizeof(node_t));
  if(node != NULL){
    node -> target = target;
    node -> dependencies = dependencies;
    node -> rules = rules;
    node -> next = NULL;
  }
  return node;
}

void list_append(list_t* list, void* target, void* dependencies, void* rules){
  assert(list != NULL);
  node_t* node = node_new(target, dependencies, rules);
  assert(node != NULL);

  if(list -> first == NULL){
    assert(list->last = NULL);
    list -> first = node;
    list -> last = node;
  }else{
    list->last->next = node;
    list ->last = node;
  }
}

size_t list_length(list_t* list){
  assert(list != NULL);
  size_t length = 0;
  node_t* node = list -> first;
  while(node != NULL){
    length++;
    node = node -> next;
  }
  return length;
}


list_iterator_t list_first(list_t* list){
  assert(list != NULL);
  return (list_iterator_t)list -> first;
}

list_iterator_t list_last(list_t* list){
  assert(list != NULL);
  return (list_iterator_t)NULL;
}

void* iterator_item(list_iterator_t* iterator){
  assert(iterator != NULL);
  node_t*  node = (node_t*)(*iterator);
  assert(node != NULL);
  return node -> target;
}

void iterator_next(list_iterator_t* iterator){
  assert(iterator != NULL);
  node_t* node = (node_t*)(*iterator);
  assert(node != NULL);
  *iterator = (list_iterator_t)(node -> next);
}
