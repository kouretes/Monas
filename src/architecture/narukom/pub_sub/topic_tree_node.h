/*
 * TopicTreeNode.h
 *
 *  Created on: Dec 5, 2009
 *      Author: vagvaz
 */

#ifndef TOPICTREENODE_H_
#define TOPICTREENODE_H_
#include <list>
#define ON_TOPIC 0
#define ABOVE_TOPIC 1
#define BELOW_TOPIC 2
#define ALL -1
using std::list;

template<class Key, class Data>
class TopicTreeNode
{
public:
	TopicTreeNode()
	{
		parent = NULL;
	}
	TopicTreeNode(Key& k, TopicTreeNode<Key, Data>* par = NULL)
	{
		key = Key(k);
		parent = par;
	}
	TopicTreeNode(Key& k, Data& d , TopicTreeNode<Key, Data>* par = NULL)
	{
		key =Key(k);
		this->data.push_back(d);
		parent = par;

	}

	~TopicTreeNode()
	{
	}
	//list<Data>&
	list<Data*>&
	get_data(int where)
	{
		switch(where)
		{
		case ON_TOPIC:
			return  on_topic;
		case ABOVE_TOPIC:
			return  above_topic;
		case BELOW_TOPIC:
			return  below_topic;
		default:
			return  on_topic;
		}

	}

	Key&
	get_key()
	{
		return  key;
	}


	std::list< TopicTreeNode<Key,Data>* >& get_children()
	{
		return	children;
	}
	TopicTreeNode<Key, Data>* get_parent()
	{
		return parent;
	}
	void add_child(TopicTreeNode<Key,Data>* new_child)
	{
		children.push_back(new_child);
	}
	void add_data(list<Data>& new_data, int where)
	{
		list<Data*>& val;
		switch(where)
		{
		case ON_TOPIC:
			val = on_topic;
			break;
		case ABOVE_TOPIC:
			val = above_topic;
			break;
		case BELOW_TOPIC:
			val = below_topic;
			break;
		default:
			val = on_topic;
		}

		list<Data> iter = new_data->begin();

		while(iter != new_data->end())
			val.insert(iter++);
	}

	void add_data(std::vector<Data>& new_data,int where)
	{
		list<Data>& val;
		switch(where)
		{
		case ON_TOPIC:
			val = on_topic;
			break;
		case ABOVE_TOPIC:
			val = above_topic;
			break;
		case BELOW_TOPIC:
			val = below_topic;
			break;
		default:
			val = on_topic;
		}

		list<Data> iter = new_data->begin();

		while(iter != new_data->end())
			val.insert(iter++);
	}
	void add_data(Data* new_dat, int where)
	{
		switch(where)
		{
		case ON_TOPIC:
			on_topic.push_back(new_dat);
			break;
		case ABOVE_TOPIC:
			above_topic.push_back(new_dat);
			break;
		case BELOW_TOPIC:
			below_topic.push_back(new_dat);
			break;
		default:
			on_topic.push_back(new_dat);
		}

	}
	
	std::_List_iterator<Data*> find_in(list<Data*>* alist, Data* old_item)
	{
	  std::_List_iterator<Data> it = alist->begin();
	  while(it != alist->end())
	    if(*old_item == *it)
	      break;
	    else
	      it++;
	  return it;
	}
	
	bool remove_from(Data* old_dat){
	  std::_List_iterator<Data*> old_item_it = find_in(&on_topic,old_dat);
	  if(old_item_it != on_topic.end())
	  {
	    on_topic.erase(old_item_it);
	    return true;
	  }
	  old_item_it = find_in( &above_topic ,old_dat);
	  if(old_item_it != above_topic.end())
	  {
	    above_topic.erase(old_item_it);
	    return true;
	  }
	  old_item_it = find_in( &below_topic,old_dat);
	  if(old_item_it != below_topic.end())
	  {
	    below_topic.erase(old_item_it);
	    return true;
	  }
	  return false;
	 }
	
private:
	Key key;
	list<Data*> on_topic;
	list<Data*> above_topic;
	list<Data*> below_topic;
	TopicTreeNode<Key, Data>* parent;
	list<TopicTreeNode<Key, Data>*> children;
};

#endif /* TOPICTREENODE_H_ */
