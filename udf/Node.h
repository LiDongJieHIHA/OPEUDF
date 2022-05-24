#pragma once
#include <array>
#include <math.h>
#include <assert.h>
#include <vector>
#include <map> 
#include <fstream>
#include<iomanip>
#include <assert.h>
using namespace std;
/*initalize all*/

class Node {
public:
	int type;
	int parent_index;
	Node* parent = NULL;
	virtual void rebalance() {};
	virtual long long insert(int pos, string cipher) { return 0; };
	virtual long long search(int pos) {return 0; };
	virtual void traverse(){return ; };
};

int M = 128;
extern Node *root;
extern long long start_update;
extern long long end_update;
extern std::map<string, long long> update;


class InternalNode : public Node {
public:
	/*internal node*/
	std::vector<int> child_num; //子节点共有的加密值个数
	std::vector<Node*> child; //子节点
	InternalNode();
	void insert_node(int index, Node *new_node);
	void rebalance() override;
	long long insert(int pos, string cipher) override;
	long long search(int pos) override;
	void traverse() override;
};

class LeafNode : public Node {
	/*leaf node*/
public:
	std::vector<std::string> cipher;//密文
	std::vector<long long> encoding;//密文
	LeafNode* left_bro = NULL;
	LeafNode* right_bro = NULL;
	long long lower = -1;
	long long upper = -1;

	LeafNode();
	long long Encode(int pos);
	void rebalance()override;
	long long insert(int pos, string cipher) override;
	long long search(int pos) override;
	void traverse() override;
};



InternalNode::InternalNode() {
	this->type = 2;
	this->parent_index = -1;
	this->parent = NULL;
}


void InternalNode::insert_node(int index, Node *new_node) {

	this->child.insert(this->child.begin() + index, new_node);
	if (new_node->type == 1) {
		this->child_num.insert(this->child_num.begin() + index, ((LeafNode *)new_node)->cipher.size());
		((LeafNode *)new_node)->parent = this;
	}
	else {
		int res = 0;
		for (size_t i = 0; i < ((InternalNode *)new_node)->child_num.size(); i++)
		{
			res += ((InternalNode *)new_node)->child_num.at(i);
		}
		this->child_num.insert(this->child_num.begin() + index, res);
		((InternalNode *)new_node)->parent = this;
	}
	
	for (int i = 0; i < this->child.size(); i++) {
		this->child.at(i)->parent_index = i;
		if (this->child.at(i)->type == 1) {
			LeafNode *tmp = (LeafNode*)this->child.at(i);
			this->child_num.at(i) = tmp->cipher.size();
		}
	}
	/*
	for (int i = 1; i < this->child.size(); i++) {
		this->child.at(i)->parent_index = i;
	}
	*/
	
	if (this->child.size() >= M) {
		this->rebalance();
	}
}


void InternalNode::rebalance() {
	InternalNode* new_node = new InternalNode();
	int middle = floor(this->child.size()*0.5);
	while (middle > 0) {
		new_node->child.insert(new_node->child.begin(), this->child.at(this->child.size()-1));
		new_node->child_num.insert(new_node->child_num.begin(), this->child_num.at(this->child_num.size()-1));
		this->child.pop_back();
		this->child_num.pop_back();
		middle--;
	}
	for (int i = 0; i < new_node->child.size(); i++) {
		new_node->child.at(i)->parent_index = i;
		new_node->child.at(i)->parent = new_node;
	}
	if (!this->parent) {
		InternalNode *new_root = new InternalNode();
		new_root->insert_node(0, this);
		new_root->insert_node(1, new_node);
		root = new_root;
	} else {
		int res = 0;
		for (size_t i = 0; i < this->child_num.size(); i++) {
			res += this->child_num.at(i);
		}
		((InternalNode*)this->parent)->child_num.at(this->parent_index) = res;
		((InternalNode*)this->parent)->insert_node(this->parent_index + 1, new_node);
		
	}

}

long long InternalNode::insert(int pos, string cipher) {
	
	for (int  i = 0; i < this->child.size(); i++) {
		if (pos > this->child_num.at(i)) {
			pos = pos - this->child_num.at(i);
		}
		else {
			this->child_num.at(i)++;
			return this->child.at(i)->insert(pos, cipher);
		}
	}
	this->child_num.back() = this->child_num.back()++;
	return this->child.back()->insert(pos, cipher);
	
}

long long InternalNode::search(int pos) {
	int i = 0;
	for (; i < this->child.size(); i++) {
		if (pos < this->child_num.at(i)) {
			return this->child.at(i)->search(pos);
		}
		else {
			pos = pos - this->child_num.at(i);
		}
	}
	
}



LeafNode::LeafNode() {
	this->type = 1;
	this->parent_index = -1;
	this->parent = NULL;

}

void Recode(vector<LeafNode*> node_list) {
	// ofstream out("logfh.txt",ios::app);
	// out<<"enter recode"<<"\n";
	// out.close();
	long long left_bound = node_list.at(0)->lower;
	long long right_bound = node_list.back()->upper;
	int total_cipher_num = 0;
	
	for (size_t i = 0; i < node_list.size(); i++) {
		total_cipher_num += node_list.at(i)->cipher.size();
	}
	// ofstream out1("logfh.txt",ios::app);
	// out1<<right_bound<<" "<<left_bound<<" "<<total_cipher_num<<"\n";
	// out1.close();
	
	if ((right_bound - left_bound) > total_cipher_num) {
        // ofstream out3("logfh.txt",ios::app);
		// out3<<"enter if"<<"\n";
		// out3.close();
		
		start_update = left_bound;
		end_update = right_bound;
		long long frag = floor((right_bound - left_bound ) / total_cipher_num);
		assert(frag >= 1);
		long long cd = left_bound;
		// out<<"**************************\n";
		// out.setf(ios::fixed);
		// out<<cd<<"\n";
		// out<<setprecision(15)<<frag<<"\n";
		for (size_t i = 0; i < node_list.size(); i++) {
			node_list.at(i)->lower = cd;
			// out<<"############################\n";
			for (int j = 0; j < node_list.at(i)->encoding.size(); j++) {
				node_list.at(i)->encoding.at(j) = cd;
				update.insert(make_pair(node_list.at(i)->cipher.at(j), cd));
				// out<<setprecision(15)<<cd<<" "<<node_list.at(i)->cipher.at(j)<<"\n";
				cd = cd + frag;
			}
			node_list.at(i)->upper = cd;
		}
		node_list.back()->upper = right_bound;
		// out.close();
		// ofstream out4("logfh.txt",ios::app);
		// out4<<"out if"<<"\n";
		// out4.close();
	}
	else {
		if (node_list.at(0)->left_bro) {
			node_list.insert(node_list.begin(), node_list.at(0)->left_bro);
		}
		if (node_list.back()->right_bro) {
			node_list.push_back(node_list.back()->right_bro);
		}
		else {
			node_list.back()->upper =  node_list.back()->upper * 2;
			if(node_list.back()->upper >= pow(2, 60)) node_list.back()->upper = pow(2, 60);
		}
		Recode(node_list);
	}
	// ofstream out2("logfh.txt",ios::app);
	// out2<<"out recode"<<"\n";
	// out2.close();

}

long long LeafNode::Encode(int pos) {
	long long left = this->lower;
	long long right = this->upper;
	
	if (pos > 0) {
		left = this->encoding.at(pos - 1);
	}
	if (pos < this->encoding.size() - 1) {
		right = this->encoding.at(pos + 1);
	}
	if (floor(right - left) < 2) {
		std::vector<LeafNode*> node_list;
		node_list.push_back(this);
		Recode(node_list);
		// out<<pos<<" & "<<this->encoding.at(pos)<<" & "<<left<<" & "<<right<<"\n";
		// out.close();
		return 0;
	}
	else {
		unsigned long long re = right;
		long long frag = (right - left)/2; 
		re = re - frag;
		this->encoding.at(pos) = re;
		// out<<pos<<" "<<this->encoding.at(pos)<<" "<<left<<" "<<right<<"\n";
		// out.close();
		return this->encoding.at(pos);
	}
	
}

void LeafNode::rebalance() {
	LeafNode* new_node = new LeafNode();
	int middle = floor(this->cipher.size()*0.5);
	while (middle > 0) {
		new_node->cipher.insert(new_node->cipher.begin(), this->cipher.back());
		new_node->encoding.insert(new_node->encoding.begin(), this->encoding.back());
		this->encoding.pop_back();
		this->cipher.pop_back();
		middle--;
	}
	new_node->lower = new_node->encoding.at(0);
	new_node->upper = this->upper;
	this->upper =  new_node->encoding.at(0);
	if (this->right_bro) {
		this->right_bro->left_bro = new_node;
	}
	new_node->right_bro = this->right_bro;
	this->right_bro = new_node;
	new_node->left_bro = this;
	if (!this->parent) {
		InternalNode *new_root = new InternalNode();
		new_root->insert_node(0, this);
		new_root->insert_node(1, new_node);
		root = new_root;
	}
	else {
		((InternalNode*)this->parent)->child_num.at(this->parent_index) = this->cipher.size();
		if (this->cipher.size() >= M) {
			printf("error\n");
		}
		((InternalNode*)this->parent)->insert_node(this->parent_index + 1, new_node);
	}

}

long long LeafNode::insert(int pos, string cipher) {
	this->cipher.insert(this->cipher.begin() + pos, cipher);
	this->encoding.insert(this->encoding.begin() + pos, -1);
	long long cd = this->Encode(pos);
	if (this->cipher.size() >= M) {
		this->rebalance();
	}
	return cd;

}

long long LeafNode::search(int pos) {
	return this->encoding.at(pos);
}

void InternalNode::traverse() {
    for (int i = 0; i < this->child.size(); i++) {
        this->child.at(i)->traverse();
    }
}
void LeafNode::traverse() {
    ofstream out("test.txt",ios::app);
	out<<"********************"<<"\n";
	out.setf(ios::fixed);
    for (int i = 0; i < this->encoding.size(); i++) {
        out<<setprecision(15)<<this->lower<<"   "<<setprecision(15)<<this->encoding.at(i)<<" "<<setprecision(15)<<this->upper<<"\n";
    }
	out<<"********************"<<"\n";
    out.close();
}


void root_initial() {
	root = new LeafNode();
	((LeafNode*)root)->lower = 0;
	((LeafNode*)root)->upper = pow(2, 62);
	update.clear();
};

long long upper_bound() {
	return end_update;
}

long long lower_bound() {
	return start_update;
}

long long get_update(string cipher) {
	if (update.count(cipher) > 0) {
		return update[cipher];
	}
	return 0;
}

