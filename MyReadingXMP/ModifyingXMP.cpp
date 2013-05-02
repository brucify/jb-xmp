//
//  ModifyingXMP.cpp
//  MyReadingXMP
//
//  Created by Bruce Yinhe on 26/03/2013.
//  Copyright (c) 2013 Bruce Yinhe. All rights reserved.
//

#include <iostream>
#include <fstream>

#include <string>

#include "Global.h"
#include "ModifyingXMP.h"

using namespace std;

/**
 * Display some property values to the console
 *
 * meta - a pointer to the XMP object that will have the properties read
 */
void displayPropertyValues(SXMPMeta * meta)
{
	// Read a simple property
	string simpleValue;  //Stores the value for the property
	meta->GetProperty(kXMP_NS_XMP, "CreatorTool", &simpleValue, 0);
	cout << "meta:CreatorTool = " << simpleValue << endl;
    
	// Get the first and second element in the dc:creator array
	string elementValue;
	meta->GetArrayItem(kXMP_NS_DC, "creator", 1, &elementValue, 0);
	if(elementValue != "")
	{
		cout << "dc:creator[1] = " << elementValue << endl;
		meta->GetArrayItem(kXMP_NS_DC, "creator", 2, &elementValue, 0);
		cout << "dc:creator[2] = " << elementValue << endl;
	}
    
	// Get the the entire dc:subject array
	string propValue;
	int arrSize = meta->CountArrayItems(kXMP_NS_DC, "subject");
	for(int i = 1; i <= arrSize;i++)
	{
		meta->GetArrayItem(kXMP_NS_DC, "subject", i, &propValue, 0);
		cout << "dc:subject[" << i << "] = " << propValue << endl;
	}
    
	// Get the dc:title for English and French
	string itemValue;
	string actualLang;
	meta->GetLocalizedText(kXMP_NS_DC, "title", "en", "en-US", 0, &itemValue, 0);
	cout << "dc:title in English = " << itemValue << endl;
    
	meta->GetLocalizedText(kXMP_NS_DC, "title", "fr", "fr-FR", 0, &itemValue, 0);
	cout << "dc:title in French = " << itemValue << endl;
    
	// Get dc:MetadataDate
	XMP_DateTime myDate;
	if(meta->GetProperty_Date(kXMP_NS_XMP, "MetadataDate", &myDate, 0))
	{
		// Convert the date struct into a convenient string and display it
		string myDateStr;
		SXMPUtils::ConvertFromDate(myDate, &myDateStr);
		cout << "meta:MetadataDate = " << myDateStr << endl;
	}
    
	cout << "----------------------------------------" << endl;
}


bool readXMPFile(char* target)
{
//    string output;
//    string line;
//    ifstream myfile ("/Users/bruce/Desktop/test.xmp");
//    if (myfile.is_open())
//    {
//        while ( myfile.good() )
//        {
//            getline (myfile,line);
//            output += line;
//        }
//        
//        target = output.c_str();
//        return true;
//        myfile.close();
//    }
//    
//    else return false;
    ifstream is;
    is.open("/Users/bruce/Desktop/test.xmp");

    if (is.is_open()){
        is.seekg (0, is.end);
        int length = is.tellg();
        is.seekg (0, is.beg);
        
        target = new char [length];
    
        int i = 0;
    
        while (!is.eof()){
            is.read(target + i, 1);
            i++;
        }
        
        return true;
    }
    else
        return false;
}


/**
 * Creates an XMP object from an RDF string.  The string is used to
 * to simulate creating and XMP object from multiple input buffers.
 * The last call to ParseFromBuffer has no kXMP_ParseMoreBuffers options,
 * thereby indicating this is the last input buffer.
 */
SXMPMeta createXMPFromRDF()
{
//	const char * rdf =
//    "<rdf:RDF xmlns:rdf='http://www.w3.org/1999/02/22-rdf-syntax-ns#'>"
//    "<rdf:Description rdf:about='' xmlns:dc='http://purl.org/dc/elements/1.1/'>"
//    "<dc:subject>"
//    "<rdf:Bag>"
//    "<rdf:li>asdasdasdsad</rdf:li>"
//    "<rdf:li>ShahahahhhahaaahDK</rdf:li>"
//    "<rdf:li>Samaaaaaaaaple</rdf:li>"
//    "</rdf:Bag>"
//    "</dc:subject>"
//    "<dc:format>image/tiff</dc:format>"
//    "</rdf:Description>"
//    "</rdf:RDF>";
    
    char* rdf;
    readXMPFile(rdf);
    
    
	SXMPMeta meta;
	// Loop over the rdf string and create the XMP object
	// 10 characters at a time
	int i;
	for (i = 0; i < (long)strlen(rdf) - 10; i += 10 )
	{
		meta.ParseFromBuffer ( &rdf[i], 10, kXMP_ParseMoreBuffers );
	}
	
	// The last call has no kXMP_ParseMoreBuffers options, signifying
	// this is the last input buffer
	meta.ParseFromBuffer ( &rdf[i], (XMP_StringLen) strlen(rdf) - i );
	return meta;
    
}
