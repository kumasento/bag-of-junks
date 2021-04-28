/**
 * Various sequential data structure
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked list.
struct Node {
  int val;
  struct Node *next;
};
typedef struct Node *node_p;

node_p create_node(int val) {
  node_p node = malloc(sizeof(struct Node));
  node->val = val;
  node->next = NULL;
  return node;
}

void print_list(node_p head) {
  node_p curr = head;
  while (curr != NULL) {
    printf("%d -> ", curr->val);
    curr = curr->next;
  }
  printf("NIL\n");
}

void push_back(node_p head, int val) {
  node_p curr = head;
  while (curr->next != NULL)
    curr = curr->next;
  curr->next = create_node(val);
}

void insert_node(node_p node, node_p new_node) {
  new_node->next = node->next;
  node->next = new_node;
}

node_p pop_front(node_p head) {
  node_p new_head = head->next;
  free(head);
  return new_head;
}

void main() {
  node_p head = create_node(1);
  print_list(head);
  push_back(head, 2);
  print_list(head);
  insert_node(head, create_node(3));
  print_list(head);
  head = pop_front(head);
  print_list(head);
}
