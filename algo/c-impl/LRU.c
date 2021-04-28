#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int KeyType;
typedef int ValueType;

/** Doubly linked list definition */
struct Node_Record;

typedef struct Node_Record *Node;

typedef struct {
  KeyType key;
  Node ptr;
} EntryType;

enum Node_Type { ENTRY, VALUE, UNKNOWN };

struct Node_Record {
  union {
    EntryType entry; // Hash table entry
    ValueType value;
  } elem;

  enum Node_Type type;
  Node next, prev;
};

Node ValueNode_Create(ValueType);
Node EntryNode_Create(EntryType);
void Node_Free(Node);
void Node_Print(Node);

struct List_Record {
  Node head, tail;
};
typedef struct List_Record *List;

List List_Create(void);
void List_Free(List);
void List_Append(List, Node);
void List_Prepend(List, Node);
void List_Print(List);
Node List_FindEntry(List, KeyType);
Node List_FindValue(List, ValueType);

/** Hash table definition */

#define MAX_HASH_ENTRIES 16

struct HashTable_Record {
  List *chains;
};

typedef struct HashTable_Record *HashTable;

HashTable HashTable_Create(void);
void HashTable_Free(HashTable);

size_t HashTable_Hash(KeyType);
void HashTable_Insert(HashTable, EntryType);
void HashTable_Print(HashTable);
Node HashTable_Find(HashTable, KeyType);

/** LRU definition. */

#define MAX_LRU_CAPACITY 16

struct LRU_Record {
  int capacity;
  int size;

  HashTable htable;
  List cache;
};

typedef struct LRU_Record *LRU;

void LRU_Put(LRU, KeyType, ValueType);
ValueType LRU_Get(LRU, KeyType);
LRU LRU_Create(void);
void LRU_Free(LRU);
void LRU_Print(LRU);

void main() {
  LRU lru = LRU_Create();

  LRU_Put(lru, 1, -10);
  LRU_Put(lru, 2, -20);
  LRU_Put(lru, 5, 72);
  assert(LRU_Get(lru, 1) == -10);
  assert(LRU_Get(lru, 2) == -20);
  assert(LRU_Get(lru, 5) == 72);

  LRU_Print(lru);
  LRU_Free(lru);
}

/** ------------------ List ------------------------------ */

static Node Node_Create() {
  Node node;

  node = malloc(sizeof(struct Node_Record));
  node->prev = NULL;
  node->next = NULL;
  node->type = UNKNOWN;
  assert(node != NULL);

  return node;
}

Node ValueNode_Create(ValueType value) {
  Node node;
  node = Node_Create();
  node->elem.value = value;
  node->type = VALUE;
  return node;
}

Node EntryNode_Create(EntryType entry) {
  Node node;
  node = Node_Create();
  node->elem.entry = entry;
  node->type = ENTRY;
  return node;
}

void Node_Free(Node node) { free(node); }

void Node_Print(Node node) {
  switch (node->type) {
  case ENTRY:
    printf("Entry(");
    printf("%d, %p", node->elem.entry.key, node->elem.entry.ptr);
    printf(")");
    break;
  case VALUE:
    printf("Value[%p](", node);
    printf("%d", node->elem.value);
    printf(")");
    break;
  default:
    assert(0);
  }
}

List List_Create(void) {
  List list;

  list = malloc(sizeof(struct List_Record));
  assert(list != NULL);

  list->head = malloc(sizeof(struct Node_Record));
  assert(list->head != NULL);
  list->tail = malloc(sizeof(struct Node_Record));
  assert(list->tail != NULL);

  list->head->next = list->tail;
  list->tail->prev = list->head;

  return list;
}

void List_Free(List list) {
  Node next;
  Node curr = list->head->next;

  while (curr != list->tail) {
    next = curr->next;
    free(curr);
    curr = next;
  }

  free(list->head);
  free(list->tail);
  free(list);
};

static void List_InsertBefore(List l, Node n1, Node n2) {
  assert(n1 != l->head);
  assert(n2->prev == NULL && n2->next == NULL);

  // n2 -> n1
  n1->prev->next = n2;
  n2->next = n1;
  n2->prev = n1->prev;
  n1->prev = n2;
}

static void List_InsertAfter(List l, Node n1, Node n2) {
  assert(n1 != l->tail);
  assert(n2->prev == NULL && n2->next == NULL);

  // n1 -> n2
  n1->next->prev = n2;
  n2->next = n1->next;
  n2->prev = n1;
  n1->next = n2;
}

void List_Append(List l, Node n) { List_InsertBefore(l, l->tail, n); }
void List_Prepend(List l, Node n) { List_InsertAfter(l, l->head, n); }

void List_Print(List l) {
  Node curr = l->head->next;

  printf("HEAD -> ");
  while (curr != l->tail) {
    Node_Print(curr);
    printf(" -> ");
    curr = curr->next;
  }
  printf("NIL");
}

Node List_FindEntry(List l, KeyType key) {
  Node curr;
  curr = l->head->next;

  while (curr != l->tail) {
    assert(curr->type == ENTRY);
    if (curr->elem.entry.key == key)
      return curr;
    curr = curr->next;
  }

  return NULL;
}

Node List_FindValue(List l, ValueType value) {
  Node curr;
  curr = l->head->next;

  while (curr != l->tail) {
    assert(curr->type == VALUE);
    if (curr->elem.value == value)
      return curr;
    curr = curr->next;
  }

  return NULL;
}

/** ------------------ Hash Table ------------------------ */
HashTable HashTable_Create(void) {
  int i;
  HashTable htable;

  htable = malloc(sizeof(struct HashTable_Record));
  assert(htable != NULL);

  htable->chains = malloc(sizeof(Node) * MAX_HASH_ENTRIES);
  assert(htable->chains != NULL);

  for (i = 0; i < MAX_HASH_ENTRIES; i++)
    htable->chains[i] = List_Create();

  return htable;
}

void HashTable_Free(HashTable htable) {
  int i;
  for (i = 0; i < MAX_HASH_ENTRIES; i++)
    List_Free(htable->chains[i]);

  free(htable->chains);
  free(htable);
}

size_t HashTable_Hash(KeyType key) {
  return (key + MAX_HASH_ENTRIES) % MAX_HASH_ENTRIES;
}

void HashTable_Insert(HashTable htable, EntryType entry) {
  Node node;
  size_t hash;

  node = EntryNode_Create(entry);
  hash = HashTable_Hash(entry.key);
  List_Append(htable->chains[hash], node);
}

void HashTable_Print(HashTable htable) {
  int i;
  for (i = 0; i < MAX_HASH_ENTRIES; i++) {
    printf("[%03d]: ", i);
    List_Print(htable->chains[i]);
    printf("\n");
  }
}

Node HashTable_Find(HashTable htable, KeyType key) {
  size_t hash = HashTable_Hash(key);
  return List_FindEntry(htable->chains[hash], key);
}

/** ------------------ LRU ------------------------ */

LRU LRU_Create(void) {
  LRU lru = malloc(sizeof(struct LRU_Record));
  assert(lru != NULL);

  lru->size = 0;
  lru->capacity = MAX_LRU_CAPACITY;
  lru->htable = HashTable_Create();
  lru->cache = List_Create();

  return lru;
}

void LRU_Free(LRU lru) {
  HashTable_Free(lru->htable);
  List_Free(lru->cache);
  free(lru);
}

void LRU_Put(LRU lru, KeyType key, ValueType value) {
  Node vnode;
  EntryType entry;

  if (lru->size >= lru->capacity) {
    // Evict
    assert(0);
  }

  vnode = ValueNode_Create(value);
  List_Append(lru->cache, vnode);
  lru->size++;

  entry.key = key;
  entry.ptr = vnode;
  HashTable_Insert(lru->htable, entry);
}

ValueType LRU_Get(LRU lru, KeyType key) {
  Node entry;
  Node vnode;

  entry = HashTable_Find(lru->htable, key);
  assert(entry != NULL);

  vnode = entry->elem.entry.ptr;
  assert(vnode->type == VALUE);

  return vnode->elem.value;
}

void LRU_Print(LRU lru) {
  printf("LRU:\n");
  printf("  Size:     %3d\n", lru->size);
  printf("  Capacity: %3d\n", lru->capacity);

  printf("\n-- Hash table: \n");
  HashTable_Print(lru->htable);

  printf("\n-- Cache: \n");
  List_Print(lru->cache);
  printf("\n\n");
}
