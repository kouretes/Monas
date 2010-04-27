/*
 * TopicTree.h
 *
 *  Created on: Dec 6, 2009
 *      Author: vagvaz
 */

#ifndef TOPICTREE_H_
#define TOPICTREE_H_
#include "topic_tree_node.h"
#include <utility>
#include <iostream>
using std::cout;
using std::endl;  
template<class Key, class Data>
class TopicTree
{
public:
	TopicTree(){

		root = NULL;

	}
	virtual ~TopicTree(){}
	void add_topic(Key k)
	{
		if(root == NULL)
		{
			root = new TopicTreeNode<Key,Data>(k,NULL);
			return;
		}
		TopicTreeNode<Key,Data>* node = find_topic(k);
		if(node == NULL )
			root->add_child(new TopicTreeNode<Key,Data>(k,root));

	}
	bool subscribe(const Key& topic,Data* subscriber, int type){
		TopicTreeNode<Key, Data>* node = find_topic(topic);
		if(node != NULL)
		{
			node->add_data(subscriber,type);
			return true;
		}
		return false;
	}

	bool add_topic_under(Key parent, Key k )
	{
		TopicTreeNode<Key, Data>* node = find_topic(k);
		TopicTreeNode<Key, Data>* parent_node;
		if (node == NULL)
		{
			parent_node = find_topic(parent);
			if(parent_node == NULL)
				return false;
			parent_node->add_child(new TopicTreeNode<Key,Data>(k,parent_node));
			return true;

		}
		else
			return true;

			return false;
	}


	TopicTreeNode<Key,Data>* find_topic( const Key& k)
	{
		if(root != NULL)
		{
			return find_topic_rec(root,k);
		}
		return NULL;

	}
	TopicTreeNode<Key,Data>* find_topic_rec(TopicTreeNode<Key,Data>* cur, const Key& k )
	{
		TopicTreeNode<Key,Data>* result;
		if(cur->get_key() == k)
			return cur;
		else
		{

			std::_List_iterator< TopicTreeNode<Key, Data>* > iter = cur->get_children().begin();

			while(iter != cur->get_children().end())
			{

				//TopicTreeNode<Key,Data>* iter_ptr = *iter; //to avoid compiler complaning...
				result = find_topic_rec( *iter, k);
				if(result != NULL)
					return result;
				iter++;
			}
		}
		return NULL;
	}
	//list<Data>
	void below(TopicTreeNode<Key,Data>* target_node,list<Data*>* result)
	  {
		if(target_node->get_children().size() > 0)
		  {
			  for( std::_List_iterator< TopicTreeNode<Key, Data>* > child_it = target_node->get_children().begin();
					child_it != target_node->get_children().end(); child_it++)
			 {
				  result->insert(result->end(),(*child_it)->get_data(ABOVE_TOPIC).begin(),(*child_it)->get_data(ABOVE_TOPIC).end());
				  below(*child_it,result);
			  }

		  }
	  }
	  void above(TopicTreeNode<Key,Data>* target_node,list<Data*>* result)
	  {
		  if(target_node != NULL)
		  {
			  result->insert(result->end(),target_node->get_data(BELOW_TOPIC).begin(),target_node->get_data(BELOW_TOPIC).end());
			  above(target_node->get_parent(),result);
		  }

	  }
	list<Data*>* message_arrived(const Key& k)
	{
		TopicTreeNode<Key,Data>* topic_node =  find_topic(k);
		if(topic_node == NULL)
			return NULL;
		list<Data*>* result = new list<Data*>();
		//list<Data*>& ret =  topic_node->get_data(1); // just for cleaner code
		result->insert(result->end(),topic_node->get_data(0).begin(),topic_node->get_data(0).end());
	//	cout << "size is on "  << result->size() << endl;
		result->insert(result->end(),topic_node->get_data(1).begin(),topic_node->get_data(1).end());
	//	cout << "size is  above"  << result->size() << endl;
		result->insert(result->end(),topic_node->get_data(2).begin(),topic_node->get_data(2).end());
	//	cout << "size is below"  << result->size() << endl;
		above(topic_node->get_parent(),result);
	//	cout << "size is  checked aboved"  << result->size() << endl;
		below(topic_node,result);
	//	cout << "size is  checked belowed"  << result->size() << endl;
		//result->insert(result->begin(), ret.begin(),ret.end());
		//ret = topic_node->get_data(2);
		//result->insert(result->begin(), ret.begin(),ret.end());
		return result;

	}
      bool unsubscribe(Key& k, Data* )
      {
	cout << "Topic Tree unsubscribe" << endl;
    return true; //FIXME cbm
      }
private:
	TopicTreeNode<Key,Data>* root;
};

#endif /* TOPICTREE_H_ */
