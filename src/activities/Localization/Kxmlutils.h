/*
 * Kxmlutils.h
 *
 *  Created on: Aug 6, 2009
 *      Author: trs
 */

#ifndef KXMLUTILS_H_
#define KXMLUTILS_H_
#include <iostream>
#include <string>
#include <sstream>

#include "tools/XMLConfig.h" //Used for proper inclusion of tinyxml
#include "KLocalization.h"

#define DEBUGMXML

string convertInt(int number) {
	stringstream ss;//create a stringstream
	ss << number;//add number to the stream
	return ss.str();//return a string with the contents of the stream
}

const unsigned int NUM_INDENTS_PER_SPACE = 2;

const char * getIndent(unsigned int numIndents) {
	static const char * pINDENT = "                                      + ";
	static const unsigned int LENGTH = strlen(pINDENT);

	if (numIndents > LENGTH)
		numIndents = LENGTH;

	return &pINDENT[LENGTH - numIndents];
}
// same as getIndent but no "+" at the end
const char * getIndentAlt(unsigned int numIndents) {
	static const char * pINDENT = "                                        ";
	static const unsigned int LENGTH = strlen(pINDENT);
	unsigned int n = numIndents * NUM_INDENTS_PER_SPACE;
	if (n > LENGTH)
		n = LENGTH;

	return &pINDENT[LENGTH - n];
}
int dump_attribs_to_randVar(TiXmlElement* pElement, unsigned int indent, randvar & ret) {
	if (!pElement)
		return 0;

	TiXmlAttribute* pAttrib = pElement->FirstAttribute();
	int i = 0;
	//int ival;
	double dval;
	const char* pIndent = getIndent(indent);
	//printf("\n");
	string name;
	while (pAttrib) {
		name = "";
		name += (pAttrib->Name());
		if (name.compare("Val") == 0) {
#ifdef DEBUGMXML
			printf("%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
#endif
			if (pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS) {
				ret.val = dval;
#ifdef DEBUGMXML
				printf(" d=%f", dval);
				printf("\n");
#endif
				i++;
				pAttrib = pAttrib->Next();
			}
		} else if (name.compare("Emean") == 0) {
#ifdef DEBUGMXML
			printf("%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
#endif
			if (pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS) {
				ret.Emean = dval;
				//cout <<  atof(pAttrib->Value()) << en;
#ifdef DEBUGMXML
				printf(" d=%f", dval);
				printf("\n");
#endif
				i++;
				pAttrib = pAttrib->Next();
			}
		} else if (name.compare("EDev") == 0) {
#ifdef DEBUGMXML
			printf("%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
#endif
			if (pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS) {
				ret.Edev = dval;
#ifdef DEBUGMXML
				printf(" d=%f", dval);
				printf("\n");
#endif
				i++;
				pAttrib = pAttrib->Next();
			}
		} else {
#ifdef DEBUGMXML
			printf("WarningUknown Parameter ! %s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());
#endif
			i++;

		}
	}
	return i;
}

int dump_attribs_to_stdout(TiXmlElement* pElement, unsigned int indent) {
	if (!pElement)
		return 0;

	TiXmlAttribute* pAttrib = pElement->FirstAttribute();
	int i = 0;
	int ival;
	double dval;
	const char* pIndent = getIndent(indent);
	printf("\n");
	while (pAttrib) {
		printf("%s%s: value=[%s]", pIndent, pAttrib->Name(), pAttrib->Value());

		if (pAttrib->QueryIntValue(&ival) == TIXML_SUCCESS)
			printf(" int=%d", ival);
		if (pAttrib->QueryDoubleValue(&dval) == TIXML_SUCCESS)
			printf(" d=%1.1f", dval);
		printf("\n");
		i++;
		pAttrib = pAttrib->Next();
	}
	return i;
}

void dump_to_stdout(TiXmlNode * pParent, unsigned int indent = 0) {
	if (!pParent)
		return;

	TiXmlText *pText;
	int t = pParent->Type();
	printf("%s", getIndent(indent));
	int num;

	randvar test;

	switch (t) {
	case TiXmlNode::DOCUMENT:
		printf("Document");
		break;

	case TiXmlNode::ELEMENT:
		printf("Element [%s]", pParent->Value());
		num = dump_attribs_to_stdout(pParent->ToElement(), indent + 1);
		//dump_attribs_to_randVar(pParent->ToElement(), indent + 1, test);
		switch (num) {
		case 0:
			printf(" (No attributes)");
			break;
		case 1:
			printf("%s1 attribute", getIndentAlt(indent));
			break;
		default:
			printf("%s%d attributes", getIndentAlt(indent), num);
			break;
		}
		break;

	case TiXmlNode::COMMENT:
		printf("Comment: \"%s\"", pParent->Value());
		break;

	case TiXmlNode::UNKNOWN:
		printf("Unknown");
		break;

	case TiXmlNode::TEXT:
		pText = pParent->ToText();
		printf("Text: [%s]", pText->Value());
		break;

	case TiXmlNode::DECLARATION:
		printf("Declaration");
		break;
	default:
		break;
	}
	printf("\n");

	TiXmlNode * pChild;

	for (pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
		dump_to_stdout(pChild, indent + 2);
	}
}
#endif /* KXMLUTILS_H_ */
