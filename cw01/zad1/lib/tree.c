#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

#define NODE_ATTRIBUTES 6


tree_contacts * create_tree(key_of_structure key) {
  tree_contacts * tree = (tree_contacts *) malloc(sizeof(tree_contacts));
  tree->key=key;
  tree->root=NULL;

  return tree;
}


tree_contacts * create_tree_with_default_key() {
  return create_tree(SURNAME);
}

bool are_equal(char * arg1, char * arg2){
  return strcmp(arg1, arg2 ) == 0;
}

int comparator (char * arg1, char * arg2){
  return strcmp(arg1, arg2);
}


void print_in_order(node_tree_contact * node ) {

  if (node != NULL) {
      print_in_order(node->left);

      printf("%s %s %s %s %s %s \n ", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[BIRTHDATE],node->data[ADDRESS]);
      // if (node->left == NULL) {
      //   printf("L = NULL " );
      // }
      // else printf("L != NULL " );
      //
      // if (node->right == NULL) {
      //   printf(" ||  R = NULL\n" );
      // } else {
      //   printf(" ||  R != NULL\n" );
      // }

      print_in_order(node->right);
    }
}

void print_tree(tree_contacts * tree) {


  if (tree == NULL) {
    printf("Tree does not exist. \n" );
    return;
  }
  if (tree->root == NULL ) {
    printf("Tree is empty. Cannot Display\n" );
    return ;
  }

  print_in_order(tree->root);
  printf("\n" );
}



void insert_node(tree_contacts ** tree , node_tree_contact * nodeToInsert  ){
  if ((*tree)->root == NULL) {
    (*tree)->root = nodeToInsert;
    return ;
  }
  node_tree_contact * cp = (*tree)->root;

  while (true) {
    if (comparator(nodeToInsert->data[(*tree)->key] , cp->data[(*tree)->key]) <0 ) {
      if(cp->left == NULL) break;
      else cp=cp->left;

    } else {
      if(cp->right == NULL) break;
      else cp=cp->right;
    }
  }
  nodeToInsert->up = cp;
  if (comparator(nodeToInsert->data[(*tree)->key] , cp->data[(*tree)->key]) <0 ) {
    cp->left=nodeToInsert;
  }else{
    cp->right=nodeToInsert;
  }
}

void rebuild_in_order(tree_contacts ** tree, node_tree_contact * node ) {
  if (node != NULL) {

      rebuild_in_order(tree,node->left);
      // printf("%s %s %s %s %s %s \n", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[ADDRESS],node->data[BIRTHDATE]);
      if (node->up !=NULL) {


       if (node->up->left == node) {
         node->up->left = NULL;
       }
        else {
         node->up->right = NULL;
       }
      node->up=NULL;

        }
        // printf("%s %s %s %s %s %s \n", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[ADDRESS],node->data[BIRTHDATE]);
      rebuild_in_order(tree,node->right);
      insert_node(tree,node);

      // printf("XXX\n" );
      // print_tree(*tree);
  }
}


void rebuild_tree(tree_contacts ** tree, key_of_structure newKey  ) {

  if ((*tree) == NULL) {
    printf("Tree does not exist. \n" );
    return;
  }/* code */
  if ((*tree)->root == NULL ) {
    printf("Tree is empty.Cannot rebuild tree\n" );
    return ;
  }
  //node_tree_contact * node = (*tree)->root;
  node_tree_contact * node = (*tree)->root;



  (*tree)->root=NULL;
  (*tree)->key=newKey;
  // printf("%s %s %s %s %s %s ", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[ADDRESS],node->data[BIRTHDATE]);
  rebuild_in_order(tree, node);


}


bool add(tree_contacts * tree, char * name, char * surname, char * email, char * phone,char * birthdate, char * address){

  if(tree == NULL) {
    printf("Tree does not exist\n" );
    return false;
  }

  node_tree_contact * newNode = (node_tree_contact*) malloc(sizeof(node_tree_contact));
  newNode->data=(char**) malloc(sizeof(char*) * NODE_ATTRIBUTES);
  newNode->data[NAME]=name;
  newNode->data[SURNAME]=surname;
  newNode->data[EMAIL]=email;
  newNode->data[PHONE]=phone;
  newNode->data[ADDRESS]=address;
  newNode->data[BIRTHDATE]=birthdate;
  newNode->left=newNode->right=newNode->up=NULL;

  insert_node(&tree, newNode);
  // printf("%s %s %s %s %s %s \n", tree->root->data[NAME],tree->root->data[SURNAME],tree->root->data[EMAIL],tree->root->data[PHONE],tree->root->data[ADDRESS],tree->root->data[BIRTHDATE]);
  return true;
}


node_tree_contact * treeSuccessor(node_tree_contact * n) {

  if (n->right != NULL) {
    n=n->right;
    while (n->left != NULL) {
      n=n->left;
    }
    return n;
  }

  node_tree_contact * y = n->up;
  while (y != NULL && y->right == n ) {
    n=y;
    y=y->up;
  }
  return y;
}




node_tree_contact * search_node (tree_contacts * tree, char * contactKey ) {
  node_tree_contact * searchedNode=tree->root;
  while (searchedNode != NULL && !are_equal(searchedNode->data[tree->key], contactKey)) {
    if (comparator(searchedNode->data[tree->key] , contactKey)  > 0 ) {
      searchedNode=searchedNode->left;
    } else {
      searchedNode=searchedNode->right;
    }
  }
  return searchedNode;
}

bool delete_node(tree_contacts ** tree, char * contactKey ) {
  if (tree == NULL) {
    printf("Tree does not exist. Cannot remove any contact \n" );
    return false;
  }
  if ((*tree)->root == NULL ) {
    printf("Tree is empty. Cannot remove any contact\n" );
    return false;
  }

  node_tree_contact * nodeToDelete = search_node(*tree, contactKey);
  if (nodeToDelete == NULL) {
    printf("Cannot delete node. Node \"%s\" not found\n", contactKey );
    return false;
  }

  node_tree_contact * y, * x;
  if(nodeToDelete->left == NULL || nodeToDelete->right == NULL) {
    y=nodeToDelete;
  }else {
    y=treeSuccessor(nodeToDelete);
  }
  if (y->left != NULL) {
    x=y->left;
  } else {
    x=y->right;
  }

  if (x != NULL) {
    x->up = y->up;
  }

  if (y->up == NULL) {
    (*tree)->root=x;
  } else if (y == y->up->left) {
    y->up->left=x;
  } else {
    y->up->right=x;
  }
  if (y != nodeToDelete) {
    nodeToDelete->data[(*tree)->key]=y->data[(*tree)->key];
  }
  printf("%s %s %s %s %s %s  is being removed ...\n", y->data[NAME],y->data[SURNAME],y->data[EMAIL],y->data[PHONE],y->data[BIRTHDATE],y->data[ADDRESS]);
  free(y->data);
  free(y);
  printf("Contact  was removed\n" );
  return true;
}

void delete_in_order( node_tree_contact * node ) {
  if (node != NULL) {

      delete_in_order(node->left);
      // printf("%s %s %s %s %s %s \n", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[ADDRESS],node->data[BIRTHDATE]);
      if (node->up !=NULL) {


       if (node->up->left == node) {
         node->up->left = NULL;
       }
        else {
         node->up->right = NULL;
       }
      node->up=NULL;

        }
        // printf("%s %s %s %s %s %s \n", node->data[NAME],node->data[SURNAME],node->data[EMAIL],node->data[PHONE],node->data[ADDRESS],node->data[BIRTHDATE]);

      delete_in_order(node->right);
      free(node->data);
      free(node);

      // printf("XXX\n" );
      // print_tree(*tree);
  }
}

void delete_tree(tree_contacts ** tree) {
  if ((*tree) == NULL) {
    printf("Tree does not exist. Cannot remove tree \n" );
    return;
  }
  if ((*tree)->root == NULL ) {
    free(*tree);
    return;
  }
  delete_in_order((*tree)->root);
  free(*tree);
  }
