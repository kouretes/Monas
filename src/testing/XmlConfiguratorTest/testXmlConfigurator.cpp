#include "XmlConfigurator.h"


int main()
{
	string filename = "test.xml";
	node root;
	root.loadAllFiles(filename);
	cout << root.getChildrenCount() << endl;
	node *temp = root.findNodeForKey("test.iter_string");
	root.print("");
	cout << temp << endl;
	cout << "1) " << temp->getText()[0] << endl;
	cout << "2) " << root.findValueForKey("test.agent.$time").size() << endl;
	cout << "3) " << root.findValueForKey("test~0.agent~0.$time")[0] << endl;
	cout << "4) " << root.findValueForKey("test~0.agent~0")[0] << endl;
	cout << "5) " << root.findValueForKey("test~0.agent~0")[1] << endl;
	//cout<< "6) " << root.findValueForKey("test~0.agent~1.module~1")[0]<<endl;
	cout << "7) " << root.findValueForKey("test~0.map_double").size() << endl;

	if(!root.updateValueForKey("test.agent.$time", "lala"))
		cout << "Ton poulo" << endl;

	cout << "8) " << root.findValueForKey("test~0.agent~0.$time")[0] << endl;
	vector<pair<string, string> > tempV;
	pair<string, string> tempP;
	tempP.first = "test.iter_string~0";
	tempP.second = "gg";
	tempV.push_back(tempP);
	tempP.first = "test.map_double~1.$TheASDF1";
	tempP.second = "bb";
	tempV.push_back(tempP);
	tempP.first = "test.agent~0=1";
	tempP.second = "qq";
	tempV.push_back(tempP);
	cout << root.burstWrite(tempV) << endl;
	/*node *temp = root.findNodeForKey("test.iter_string");
	cout << temp->getText() << endl;
	cout << root.findValueForKey("test.map_int~2.$correctTheASDF1") << endl;
	if(root.updateValueForKey("test.map_int~2.$correctTheASDF1","Kofi"))
		cout << "Updated succesfull" << endl;
	cout << root.findValueForKey("test.iter_int~2.iter~2") << endl;
	*/
	return 0;
}
