// =================================================================================================
// Copyright 2008 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it.
// =================================================================================================

/**
 * Tutorial solution for the Walkthrough 1 in the XMP Programmers Guide, Opening files and reading XMP.
 * Demonstrates the basic use of the XMPFiles and XMPCore components, obtaining read-only XMP from a file
 * and examining it through the XMP object.
 */

#include "Global.h"

#include <iostream>
#include <fstream>

#include "ModifyingXMP.h"


using namespace std;

/**
 * Client defined callback function to dump XMP to a file.  In this case an output file stream is used
 * to write a buffer, of length bufferSize, to a text file.  This callback is called multiple
 * times during the DumpObject() operation.  See the XMP API reference for details of
 * XMP_TextOutputProc() callbacks.
 */
XMP_Status DumpXMPToFile(void * refCon, XMP_StringPtr buffer, XMP_StringLen bufferSize)
{
	XMP_Status status = 0;
	
	try
	{
		ofstream * outFile = static_cast<ofstream*>(refCon);
		(*outFile).write(buffer, bufferSize);
	}
	catch(XMP_Error & e)
	{
		cout << e.GetErrMsg() << endl;
		return -1;  // Return a bad status
	}
    
	return status;
}

/**
 *	Initializes the toolkit and attempts to open a file for reading metadata.  Initially
 * an attempt to open the file is done with a handler, if this fails then the file is opened with
 * packet scanning. Once the file is open several properties are read and displayed in the console.
 * The XMP object is then dumped to a text file and the resource file is closed.
 */
int main ( int argc, const char * argv[] )
{
//	if ( argc != 2 ) // 2 := command and 1 parameter
//	{
//		cout << "usage: ReadingXMP (filename)" << endl;
//		return 0;
//	}
	
	string filename = "/Users/bruce/Desktop/image1.jpg" /*string( argv[1] )*/;
    
	if(!SXMPMeta::Initialize())
	{
		cout << "Could not initialize toolkit!";
		return -1;
	}
	XMP_OptionBits options = 0;
    #if UNIX_ENV
    options |= kXMPFiles_ServerMode;
    #endif
	// Must initialize SXMPFiles before we use it
	if ( ! SXMPFiles::Initialize ( options ) )
	{
		cout << "Could not initialize SXMPFiles.";
		return -1;
	}
	
	try
	{
		// Options to open the file with - read only and use a file handler
		XMP_OptionBits opts = kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler;
       
        // **** register CC URI
        string ccRegistered;
        SXMPMeta::RegisterNamespace(kXMP_NS_CC, "cc", &ccRegistered);
        cout << "registeted cc ns: = " << ccRegistered  << endl;

        
		bool ok;
		SXMPFiles myFile;
		std::string status = "";
        
		// First we try and open the file
		ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
		if( ! ok )
		{
			status += "No smart handler available for " + filename + "\n";
			status += "Trying packet scanning.\n";
            
			// Now try using packet scanning
			opts = kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning;
			ok = myFile.OpenFile(filename, kXMP_UnknownFile, opts);
		}
        
        
		// If the file is open then read the metadata
		if(ok)
		{
			
            //******** READ HERE
            cout << status << endl;
			cout << filename << " is opened successfully" << endl;
			// Create the xmp object and get the xmp data
			SXMPMeta meta;
			myFile.GetXMP(&meta);

            displayPropertyValues(&meta);
            
            
            //******** WRITE HERE
            SXMPMeta meta1;
            meta1 = createXMPFromRDF();
            
            SXMPUtils::ApplyTemplate(&meta, meta1, kXMPTemplate_AddNewProperties | kXMPTemplate_ReplaceExistingProperties | kXMPTemplate_IncludeInternalProperties);

            cout << "Things to append:" << endl;
            displayPropertyValues(&meta1);
            
            cout << "After Appending Properties:" << endl;
            displayPropertyValues(&meta);
			
			// Close the SXMPFile.  The resource file is already closed if it was
			// opened as read only but this call must still be made.
			myFile.CloseFile();
		}
		else
		{
			cout << "Unable to open " << filename << endl;
		}
	}
	catch(XMP_Error & e)
	{
		cout << "ERROR: " << e.GetErrMsg() << endl;
	}
    
	// Terminate the toolkit
	SXMPFiles::Terminate();
	SXMPMeta::Terminate();
    
	return 0;
}

