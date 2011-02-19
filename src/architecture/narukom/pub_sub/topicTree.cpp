#include "topicTree.h"



TopicTree::TopicTree(std::string const& root): stringRegistry()
{
	topictree[0].parentid=0;
	topictree[0].children.insert(registerNew(root));

}

std::set<std::size_t>  TopicTree::iterateTopics(std::string const& topic , int where) const
{
	std::set<std::size_t>  res;
	size_t topicId=getId(topic);
	//cout<<"Check 0"<<endl;
	if(topicId==0)
		return res;
	//cout<<"Check 1"<<endl;
	std::map<std::size_t,topicdata >::const_iterator tit=topictree.find(topicId);
	if(tit==topictree.end())
			return res;
	//cout<<"Check 2"<<endl;
	res.insert(topicId);
	if(where==ABOVE_TOPIC||where==ALL)
	{

		do
		{
			tit=topictree.find((*tit).second.parentid);
			if(tit==topictree.end())
				break;
			res.insert((*tit).first);
		}
		while((*tit).first!=(*tit).second.parentid);
	}

	if(where==BELOW_TOPIC||where==ALL)
		iterateTopicsBelow(res,topicId);
  return res;
}


void TopicTree::addTopic(std::string const& what,std::string const& under)
{
	size_t underid=getId(under);
	size_t newid=getId(what);
	if(newid!=0)
		return;
	newid=registerNew(what);
	topictree[underid].children.insert(newid);
	topictree[newid].parentid=underid;
};
void TopicTree::iterateTopicsBelow(std::set<std::size_t>  &res,std::size_t const topicid) const
{
	std::map<std::size_t,topicdata >::const_iterator tit=topictree.find(topicid);
	if(tit==topictree.end())
			return ;
	res.insert(topicid);
	std::set<std::size_t>::const_iterator cit=(*tit).second.children.begin();
	for(;cit!=(*tit).second.children.end();++cit)//Recursive calls
		iterateTopicsBelow(res,*cit);

}


