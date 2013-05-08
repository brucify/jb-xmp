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

#include <string>

#include "ModifyingXMP.h"


using namespace std;

///**
// * Client defined callback function to dump XMP to a file.  In this case an output file stream is used
// * to write a buffer, of length bufferSize, to a text file.  This callback is called multiple
// * times during the DumpObject() operation.  See the XMP API reference for details of
// * XMP_TextOutputProc() callbacks.
// */
//XMP_Status DumpXMPToFile(void * refCon, XMP_StringPtr buffer, XMP_StringLen bufferSize)
//{
//	XMP_Status status = 0;
//	
//	try
//	{
//		ofstream * outFile = static_cast<ofstream*>(refCon);
//		(*outFile).write(buffer, bufferSize);
//	}
//	catch(XMP_Error & e)
//	{
//		cout << e.GetErrMsg() << endl;
//		return -1;  // Return a bad status
//	}
//    
//	return status;
//}

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
	
//	string filename = "/Users/bruce/Desktop/image1.jpg" /*string( argv[1] )*/;
    string filename = "/Users/bruce/Desktop/466218_3860647804331_1345311607_o.jpg" /*string( argv[1] )*/;

    
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
		//XMP_OptionBits opts = kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler;
       
        XMP_OptionBits opts = kXMPFiles_OpenForUpdate;
        
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
            
            
            string xmpBufferOld;
            meta.SerializeToBuffer( &xmpBufferOld, NULL, NULL, "", "", NULL );
            cout << "meta Old length = " << xmpBufferOld.length() << endl;

            
            //******** WRITE HERE
            SXMPMeta meta1;
            meta1 = createXMPFromRDF();

            cout << "Things to append:" << endl;
            displayPropertyValues(&meta1);
            
            
            SXMPUtils::ApplyTemplate(&meta, meta1, kXMPTemplate_AddNewProperties | kXMPTemplate_ReplaceExistingProperties | kXMPTemplate_IncludeInternalProperties);

            
            cout << "After Appending Properties:" << endl;
            displayPropertyValues(&meta);
            
            cout << "exporting updated XMP obj to .xmp file.." << endl;
            string xmpBuffer;
            meta.SerializeToBuffer( &xmpBuffer, NULL, NULL, "", "", NULL );
            cout << "meta length = " << xmpBuffer.length() << endl;
            
            writeRDFToFile(&xmpBuffer, "XMP_RDF.xmp");

			
            // Writing updated XMP back to file
            // CanPutXMP (API Doc): Use to determine if the file can probably be updated with a given set of XMP metadata. This depends on the size of the packet, the options with which the file was opened, and the capabilities of the handler for the file format. The function obtains the length of the serialized packet for the provided XMP, but does not keep it or modify it, and does not cause the file to be written when closed. This is implemented roughly as follows:
            

            
            
            
            if( myFile.CanPutXMP( meta ))
            {
                cout << "Putting back to file..." << endl;
                myFile.PutXMP( meta );
            } else {
                cout << "Cannot put back to file." << endl;
                
                XMP_FileFormat format;
                myFile.GetFileInfo(NULL, NULL, &format);
                
                XMP_OptionBits formatFlags;
                SXMPFiles::GetFormatInfo ( format, &formatFlags );
                
                cout << "formatFlags = " << formatFlags << endl;
                cout << "formatFlags & kXMPFiles_CanInjectXMP = " << (formatFlags & kXMPFiles_CanInjectXMP) << endl;
                cout << "formatFlags & kXMPFiles_CanExpand = " << (formatFlags & kXMPFiles_CanExpand) << endl;
                cout << "&& = " <<  ( (formatFlags & kXMPFiles_CanInjectXMP) && (formatFlags & kXMPFiles_CanExpand) )  << endl;
                
                XMP_PacketInfo packetInfo;
                bool hasXMP = myFile.GetXMP(0, 0, &packetInfo);
                cout << "packetInfo.length = " << packetInfo.length << endl;
            }
            
            
            
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
