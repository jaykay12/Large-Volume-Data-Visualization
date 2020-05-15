#include <bits/stdc++.h>

using namespace std;

int n, q, octant_no;

#include <bits/stdc++.h>
#include <cstring>
#include <cstdlib>

#define NO_CHILDREN 8
using namespace std;

int vis_lev;

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

struct node *root = new node;
queue <node *> que;
queue <node *> cut;

void cut_needed(int lev,int part_no) {
	int i;
	
	que.push(root);
	cut.push(root);
	
	while(1) {
		struct node *cur = new node;
		cur = que.front();
		
		if (cur->level == lev) {
			break;
		} else {
			for (i = 0; i < 8; i++) {
				que.push(cur->child[i]);
				cut.push(cur->child[i]);
			}
		}
		
		que.pop();
		cut.pop();
	}
	
	if (part_no == 0) {
		return;
	} else {
		while(!cut.empty()) {
			cut.pop();
		}
		
		int siz = que.size();
		
		for (i = 1; i < part_no; i++) {
			for (i = 1; i <= siz/8; i++) {
				que.pop();
			}
		}
		
		for (i = 1; i <= siz/8; i++) {
			cut.push(que.front());
			que.pop();
		}
	}
	
}

void octree_creation() {
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
	
	cout << "hello" << endl;	
}

int main() {
		
//creation of octree
	cout << "hii " << endl;
	octree_creation();
	cout << "octree done"<< endl;
	
//octree traversal code
/*	q.push(root);
	
	
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
					
	} */
//---------------------------------------------------------------------------------------------------------	

	while(1) {
		cout << "Enter level and octant (type 888 to exit)" << endl;
		cin >> n >> q;
		
		if (n == 888)
		return 0;
		
		cut_needed(n, q);
		
	//	freopen("oo.txt", "w", stdout);
		while(!cut.empty()) {
			cout << cut.front()->name << endl;
			cut.pop();
		}
		
		while(!que.empty()) {
			que.pop();
		}
		
		while(!cut.empty()) {
			cut.pop();
		}
		
	}	
}

