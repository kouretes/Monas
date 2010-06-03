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
/**
  TopicTree is a templated class for keeping track of the subscriptions to topics.
*/
template<class Key, class Data>
class TopicTree
{
public:
	TopicTree(){

		root = 0;

	}
	virtual ~TopicTree(){
	  if(root != 0)
	    delete root;
	  
	
	}
	bool add_topic(const Key& k)
	{
		if(root == 0)
		{
			root = new TopicTreeNode<Key,Data>(k,0);
			return true;
		}
		TopicTreeNode<Key,Data>* node = find_topic(k);
		if(node == 0 )
		{
		  root->add_child(new TopicTreeNode<Key,Data>(k,root));
		  return true;
		}
		
		return false;
	}
	
	bool delete_topic(const Key & k)
	{
	  TopicTreeNode<Key,Data>* node = find_topic(k);
	  if(node != 0)
	  {
	    TopicTreeNode<Key,Data>* parent = node->get_parent();
	    parent->remove_child(node->get_key());
	    delete node;
	    return true;
	  }
	  
	  return false;
	  
	}
	
	bool subscribe(const Key& topic,Data* subscriber, int where){
		TopicTreeNode<Key, Data>* node = find_topic(topic);
		if(node != 0)
		{
			node->add_data(subscriber,where);
			return true;
		}
		return false;
	}

	bool add_topic_under(const Key& parent, Key k )
	{
		TopicTreeNode<Key, Data>* node = find_topic(k);
		TopicTreeNode<Key, Data>* parent_node;
		if (node == 0)
		{
			parent_node = find_topic(parent);
			if(parent_node == 0)
				return false;
			parent_node->add_child(new TopicTreeNode<Key,Data>(k,parent_node));
			return true;

		}
		return false;
		
	}

	bool topic_exists(const Key & topic )
	{
	  if(find_topic(topic) == 0)
	    return false;
	  return true;
	
	}
	bool topic_exists_under(const Key& parent,const Key& topic)
	{
	  TopicTreeNode<Key, Data>* node = find_topic(topic);
	  if(node != 0)
	    if(node->get_parent()->get_key() == parent)
	      return true;
	  return false;
	  
	
	}
	TopicTreeNode<Key,Data>* find_topic( const Key& k)
	{
		if(root != 0)
		{
			return find_topic_rec(root,k);
		}
		return 0;

	}
	TopicTreeNode<Key,Data>* find_topic_rec(TopicTreeNode<Key,Data>* cur, const Key& k )
	{
		TopicTreeNode<Key,Data>* result;
		if(cur->get_key() == k)
			return cur;
		else
		{
			//Using std::_List_iterator type because compiler complains otherwise
			std::_List_iterator< TopicTreeNode<Key, Data>* > iter = cur->get_children().begin();

			while(iter != cur->get_children().end())
			{
				result = find_topic_rec( *iter, k);
				if(result != 0)
					return result;
				iter++;
			}
		}
		return 0;
	}
	
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
		result->insert(result->end(),topic_node->get_data(0).begin(),topic_node->get_data(0).end());
		result->insert(result->end(),topic_node->get_data(1).begin(),topic_node->get_data(1).end());
		result->insert(result->end(),topic_node->get_data(2).begin(),topic_node->get_data(2).end());
		above(topic_node->get_parent(),result);
		below(topic_node,result);

		return result;

	}
      bool unsubscribe(const Key& k, Data* old_dt)
      {
	cout << "Topic Tree unsubscribe" << endl;
	TopicTreeNode<Key,Data>* topic_node =  find_topic(k);
	if(topic_node != 0)
	{
	  return topic_node->remove_from(old_dt);
	}
	return false;
      }
      void find_keys_below(TopicTreeNode<Key,Data>* node, std::list<Key>* key_list)
      {
	std::list< TopicTreeNode<Key,Data> >children_list  = node->get_children();
	for(typename std::list<TopicTreeNode<Key,Data> >::iterator it = children_list.begin(); it != children_list.end();it++ )
	{
	  key_list->push_back((*it)->get_key());
	  find_keys_below((*it),key_list);
	}
      
      }
      list<Key>* all_topics_below(const Key& key)
      {
	
	std::list<Key>* result = 0;
	TopicTreeNode<Key,Data>* topic_node = find_topic(key);
	if(topic_node == 0)
	  return result;
	else
	{
	  result = new std::list<Key>();
	  find_keys_below(topic_node,result);
	  return result;
	}
      }
      
      list<Key>* all_above_below(const Key& key)
      {
	std::list<Key>* result = 0;
	TopicTreeNode<Key,Data>* topic_node = find_topic(key);
	if(topic_node == 0)
	  return result;
	else
	{
	  result = new std::list<Key>();
	  TopicTreeNode<Key,Data>* cur = topic_node->get_parent();
	  while(cur != 0)
	  {
	    result->push_back(cur->getKey());
	    cur = cur->get_parent();
	  }
	  
	  return result;
	}
      }      
      
private:
	TopicTreeNode<Key,Data>* root;
};

#endif /* TOPICTREE_H_ */
