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
    
    // OBS Begin of Adobe authored 
    
	// Read a simple property
	string simpleValue;  //Stores the value for the property
	meta->GetProperty(kXMP_NS_XMP, "CreatorTool", &simpleValue, 0);
    if(simpleValue != "")
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
    // OBS end of Adobe authored
    
    // OBS START of custom cc property
    // Get cc:attributionName
    string attriNameValue;
    
    // OBS kXMP_NS_CC must be defined
	meta->GetProperty(kXMP_NS_CC, "attributionName", &attriNameValue, 0);
    
    if (attriNameValue != "")
        cout << "cc:attributionName = " << attriNameValue << endl;
    // OBS END of custom cc property
    
	cout << "----------------------------------------" << endl;
}


// read .xmp file
bool readXMPFile( string * target )
{
    string output;
    string line;
    
    // OBS hard-coded .xmp path
    ifstream myfile ("/Users/bruce/Desktop/test_sweden.xmp");
    
    if (myfile.is_open())
    {
        while ( myfile.good() )
        {
            getline (myfile,line);
            output += line;
        }
  
        *target = output;
        
        myfile.close();

        cout << "output length = " << output.length() << endl;
        
        return true;
    }
    
    else
    {
        cout << "faaalse" << endl;
        return false;
    }
    
}


/**
 * Creates an XMP object from an RDF string.  The string is used to
 * to simulate creating and XMP object from multiple input buffers.
 * The last call to ParseFromBuffer has no kXMP_ParseMoreBuffers options,
 * thereby indicating this is the last input buffer.
 */
SXMPMeta createXMPFromRDF()
{
    
    string rdf;
    
    readXMPFile(&rdf);
    
    cout << "rdf length = " << rdf.length() << endl;

	SXMPMeta meta;
	
    cout << "reading lines";

    // OBS must be char*
    const char* c_rdf = rdf.c_str();
    
    // Loop over the rdf string and create the XMP object
	// 10 characters at a time
    // note: SEE user manuel
	int i;
	for (i = 0; i < (long)strlen(c_rdf) - 10; i += 10 )
	{
		meta.ParseFromBuffer (&c_rdf[i], 10, kXMP_ParseMoreBuffers );
        cout << ".";
	}
    cout << endl;

	// The last call has no kXMP_ParseMoreBuffers options, signifying
	// this is the last input buffer
	meta.ParseFromBuffer ( &c_rdf[i], (XMP_StringLen) strlen(c_rdf) - i );
    
	return meta;
    
}

void writeRDFToFile( string * rdf, string filename )
{
    ofstream outFile;
    outFile.open( filename.c_str(), ios::out ); outFile << *rdf;
    outFile.close();
}