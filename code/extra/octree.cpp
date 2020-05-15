#include <bits/stdc++.h>
#include <cstring>
#include <cstdlib>

#define NO_CHILDREN 8
using namespace std;

struct node{
	ifstream fp;
	struct node *child[NO_CHILDREN];
	struct node *parent;
	int level;
	int a[3];
	int no_children;
	int is_leaf;
	string name;
	//boolean is_active=false;
};

int main() {
	freopen("oo.txt", "w", stdout);
	struct node *root = new node;

	int i;
	
	root-> fp.open ("root.raw", ifstream::binary);
	root->child[0] = NULL;
	root->parent = NULL;
	root->level = 0;
	root->a[0] = 0;
	root->no_children  = 0;
	root->is_leaf = 0;
	root -> name = "";

	queue<node *> q;

	q.push(root);
	string tmp;

	while(!q.empty()) {
		struct node * cur = new node;
		cur = q.front();
		q.pop();

		for (i = 0; i < 8; i++) {
			cur->child[i] = new node;
			cur->child[i]->child[0] = NULL;
			cur->child[i]->parent = cur;
			cur->child[i]->name = cur->name + (char)(48 + i);
			tmp = cur->child[i]->name + ".raw";
			cur->child[i]->fp.open(tmp, ifstream::binary);
			cur->child[i]->level = cur->level + 1;
			cur->child[i]->a[0] = 0;
			cur->child[i]->no_children = 0;
			if (cur->level == 2) {
				cur->child[i]->is_leaf = 1;
			} else {
				cur->child[i]->is_leaf = 0;
				q.push(cur->child[i]);
			}
			cur->no_children++;
		}
	}

	q.push(root);
	
	while(!q.empty()) {
		struct node *cur = q.front();
		q.pop();
		
		if (cur->level == 3) {
			continue;
		} else {
			for (i = 0; i < 8; i++) {
				q.push(cur->child[i]);
				cout << cur->child[i]->name << endl;
			}
		}
					
	}
	
	return 0;
}
