/*
 * TopicTreeNode.h
 *
 *  Created on: Dec 5, 2009
 *      Author: vagvaz
 */

#ifndef TOPICTREENODE_H_
#define TOPICTREENODE_H_
#include <list>
#include "../narukom_common.h"
using std::list;

template<class Key, class Data>
class TopicTreeNode
{
public:
    TopicTreeNode()
    {
        parent = 0;
    }
    TopicTreeNode(const Key& k, TopicTreeNode<Key, Data>* par = 0)
    {
        key = Key(k);
        parent = par;
    }

    ~TopicTreeNode()
    {
        on_topic.clear();
        above_topic.clear();
        below_topic.clear();

        for (; children.size() > 0; )
        {
            TopicTreeNode<Key, Data>* tmp = children.front();
            children.pop_front();
            delete tmp;
        }
        parent = 0;
    }
    ///Get the Data of certain key

    list<Data*>&
    get_data(int where)
    {
        switch (where)
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

    Key& get_key()
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
			if(new_child != 0)
        children.push_back(new_child);
    }
    void add_data(list<Data*>& new_data, int where)
    {
        list<Data*>& val;
        switch (where)
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

        while (iter != new_data->end())
				{
					if((*iter) != 0)
						val.insert(iter++);
				}
    }

    void add_data(std::vector<Data*>& new_data,int where)
    {
        list<Data*>& val;
        switch (where)
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

        while (iter != new_data->end())
				{
					if((*iter) != 0)
						val.insert(iter++);
				}
    }
    
    void add_data(Data* new_dat, int where)
    {
			if(new_dat == 0 )
				return;
				
        switch (where)
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

    typename std::list<Data*>::iterator find_in(list<Data*>* alist, Data* old_item)
    {
        typename std::list<Data*>::iterator it = alist->begin();
        while (it != alist->end())
            if ((*old_item) == (*((Data*)(*it))) )
                break;
            else
                it++;
        return it;
    }
    void remove_child(const Key& key)
    {
        for (typename list<TopicTreeNode<Key, Data>*>::iterator it = children.begin(); it != children.end(); it++)
        {
            if (   (*it)->get_key() == key )
            {
                children.erase(it);
                break;
            }
        }
    }


    bool remove_from(Data* old_dat)
    {
			if(old_dat == 0)
				return false;
        typename std::list<Data*>::iterator old_item_it = find_in(&on_topic,old_dat);
        if (old_item_it != on_topic.end())
        {
            on_topic.erase(old_item_it);
            return true;
        }
        old_item_it = find_in( &above_topic ,old_dat);
        if (old_item_it != above_topic.end())
        {
            above_topic.erase(old_item_it);
            return true;
        }
        old_item_it = find_in( &below_topic,old_dat);
        if (old_item_it != below_topic.end())
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
