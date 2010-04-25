#ifndef _XML_h_
#define _XML_h_ 1

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <utility>

#include "../external/tinyxml_2-5-3/tinyxml.h"

template<class TxtType, class AttrType, class Key = std::string>
class XMLNode {

    public:

        std::string name;
        
        TxtType value;

        std::map<Key,AttrType> attrb;

        XMLNode(std::string name) : name(name), node(0) { ; }

    private:

        const TiXmlNode * node;
        
        XMLNode( std::string name, const TiXmlNode * node ) : name(name), node(node) { ; }

    friend class XML;

};

class XML {

    public:

        XML ( const std::string & filename) {
            load_successful = doc.LoadFile( filename );
            if ( ! load_successful ) 
                doc.LinkEndChild( new TiXmlDeclaration( "1.0", "", "" ) );
        }

        bool IsLoadedSuccessfully () const { return load_successful; }

        //bool SaveConfiguration () { return doc.SaveFile(); }

        template <class TxtType, class AttrType, class Key >
        std::vector<XMLNode<TxtType, AttrType, Key> > QueryElement (const std::string & ElName, XMLNode<TxtType, AttrType, Key> * pNode = NULL) const {

            std::vector<XMLNode<TxtType, AttrType, Key> > Values;

            const TiXmlNode * currentNode = (pNode == NULL)? doc.FirstChild( ElName ) : pNode->node->FirstChild( ElName);
            const TiXmlNode * searchNode = (pNode==NULL)? &doc : pNode->node;

            for ( ; currentNode != NULL ; currentNode = searchNode->IterateChildren( ElName , currentNode ) ) {

                const TiXmlElement * element = currentNode->ToElement();

                if ( element==NULL ) continue;

                XMLNode<TxtType, AttrType, Key> theNode( ElName, currentNode );                

                // Node Text
                if ( element->GetText() ) {

                    std::string elText;
                    elText.assign( element->GetText() );

                    if ( elText.size() != 0 ) {
                        std::istringstream strmElText(elText);

                        TxtType  tTypeVal;
                        strmElText>>tTypeVal;

                        theNode.value = tTypeVal;
                    }
                }

                //Node Attrb
                const TiXmlAttribute * pAttrib = element->FirstAttribute();

                while ( pAttrib ) {
                    std::istringstream strs( pAttrib->Value() ); // assumption: key is string, fix
                    AttrType theValue;
                    strs>>theValue;
                    theNode.attrb.insert( std::pair<Key,AttrType> ( pAttrib->Name(), theValue ) );
                    pAttrib = pAttrib->Next();
                }

                Values.push_back( theNode );

            } 

            return Values;
        }

    private:

        TiXmlDocument doc;

        bool load_successful;
};

#endif // _XML_h_
