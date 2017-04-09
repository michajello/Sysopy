#ifndef list_header
#define list_header

typedef enum {NAME,SURNAME,EMAIL,PHONE,BIRTHDATE,ADDRESS} key_of_structure;
#define NODE_ATTRIBUTES 6
typedef struct node_tree_contact node_tree_contact;
typedef struct tree_contacts tree_contacts;


struct node_tree_contact {
  node_tree_contact * left;
  node_tree_contact * right;
  node_tree_contact * up;
  int id;
  char ** data;
};

struct tree_contacts {
  node_tree_contact * root;
  key_of_structure key;
  int amount;
};

tree_contacts * create_tree(key_of_structure );
tree_contacts * create_tree_with_default_key(void);
bool are_equal(char * , char * );
int comparator (char * , char * );
void print_in_order(node_tree_contact *  );
void print_tree(tree_contacts * );
void insert_node(tree_contacts **  , node_tree_contact *   );
void rebuild_in_order(tree_contacts ** , node_tree_contact *  );
void rebuild_tree(tree_contacts ** , key_of_structure   );
bool add(tree_contacts * , char * , char * , char * , char * ,char * address, char * );
node_tree_contact * treeSuccessor(node_tree_contact * );
node_tree_contact * search_node (tree_contacts * , char *  );
bool delete_node(tree_contacts ** , char * contactKey );
void delete_in_order( node_tree_contact *  );
void delete_tree(tree_contacts ** );

#endif
