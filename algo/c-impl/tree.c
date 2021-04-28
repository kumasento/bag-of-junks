#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int ElementType;

struct TreeNode;
typedef struct TreeNode *SearchTree;
typedef struct TreeNode *Position;

struct TreeNode {
  ElementType elem;
  SearchTree left, right;
};

void erase(SearchTree tree) {
  if (tree != NULL) {
    erase(tree->left);
    erase(tree->right);
    free(tree);
  }
}

SearchTree insert(SearchTree node, ElementType elem) {
  if (node == NULL) {
    node = malloc(sizeof(struct TreeNode));
    if (node == NULL)
      exit(1);

    node->elem = elem;
    node->left = NULL;
    node->right = NULL;
    return node;
  }

  if (elem < node->elem)
    node->left = insert(node->left, elem);
  else
    node->right = insert(node->right, elem);

  return node;
}

Position find(SearchTree node, ElementType elem) {
  if (node == NULL)
    return NULL;

  if (node->elem == elem)
    return node;
  if (node->elem > elem)
    return find(node->left, elem);
  return find(node->right, elem);
}

Position find_max(SearchTree node) {
  if (node == NULL)
    return NULL;

  if (node->right == NULL)
    return node;

  return find_max(node->right);
}

Position find_min(SearchTree node) {
  if (node == NULL)
    return NULL;

  if (node->left == NULL)
    return node;

  return find_min(node->left);
}

SearchTree delete (SearchTree tree, ElementType elem) {
  Position temp;

  if (tree->elem < elem)
    tree->left = delete (tree->left, elem);
  else if (tree->elem > elem)
    tree->right = delete (tree->right, elem);
  else if (tree->left && tree->right) {
    temp = find_min(tree->right);
    tree->elem = temp->elem;
    tree->right = delete (tree->right, tree->elem);
  } else {
    temp = tree;
    if (tree->left == NULL)
      tree = tree->right;
    else if (tree->right == NULL)
      tree = tree->left;
    free(temp);
  }
  return tree;
}

void main() {
  SearchTree tree;

  tree = insert(tree, 1);
  tree = insert(tree, 2);
  tree = insert(tree, 3);
  tree = insert(tree, 4);
  tree = insert(tree, 5);

  assert(find(tree, 1) != NULL);
  assert(find(tree, 2) != NULL);
  assert(find(tree, 3) != NULL);
  assert(find(tree, 4) != NULL);
  assert(find(tree, 5) != NULL);

  assert(find_max(tree)->elem == 5);
}
