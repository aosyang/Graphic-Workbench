/********************************************************************************
/Name:		TGA.cpp																*
/Header:	tga.h																*
/Purpose:	Load Compressed and Uncompressed TGA files							*
/Functions:	LoadTGA(Texture * texture, char * filename)							*
/			LoadCompressedTGA(Texture * texture, char * filename, FILE * fTGA)	*
/			LoadUncompressedTGA(Texture * texture, char * filename, FILE * fTGA)*	
/*******************************************************************************/
#include "TGA.h"

#include <memory>
#include <cstdio>
#include <malloc.h>
#include <cstring>

/********************************************************************************
/name :		LoadTGA(Texture * texture, char * filename)							*
/function:  Open and test the file to make sure it is a valid TGA file			*	
/parems:	texture, pointer to a Texture structure								*
/			filename, string pointing to file to open							*
/********************************************************************************/

typedef struct
{
	GW_UINT8 Header[12];									// TGA File Header
} TGAHeader;


typedef struct
{
	GW_UINT8		header[6];								// First 6 Useful Bytes From The Header
	GW_UINT32		bytesPerPixel;							// Holds Number Of Bytes Per Pixel Used In The TGA File
	GW_UINT32		imageSize;								// Used To Store The Image Size When Setting Aside Ram
	GW_UINT32		temp;									// Temporary Variable
	GW_UINT32		format;	
	GW_UINT32		Height;									//Height of Image
	GW_UINT32		Width;									//Width ofImage
	GW_UINT32		Bpp;									// Bits Per Pixel
} TGA;

TGAHeader tgaheader;									// TGA header
TGA tga;												// TGA image data

GW_UINT8 uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
GW_UINT8 cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header
bool LoadUncompressedTGA(GW_IMAGE* image, const char* filename, FILE* fTGA);	// Load an Uncompressed file
bool LoadCompressedTGA(GW_IMAGE* image, const char* filename, FILE* fTGA);		// Load a Compressed file

bool LoadTGAImage(GW_IMAGE* image, const char* filename)				// Load a TGA file
{
	FILE * fTGA;												// File pointer to texture file
	fTGA = fopen(filename, "rb");								// Open file for reading

	if(fTGA == NULL)											// If it didn't open....
	{
		return false;														// Exit function
	}

	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)					// Attempt to read 12 byte header from file
	{
		if(fTGA != NULL)													// Check to seeiffile is still open
		{
			fclose(fTGA);													// If it is, close it
		}
		return false;														// Exit function
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)				// See if header matches the predefined header of 
	{																		// an Uncompressed TGA image
		LoadUncompressedTGA(image, filename, fTGA);						// If so, jump to Uncompressed TGA loading code
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		// See if header matches the predefined header of
	{																		// an RLE compressed TGA image
		LoadCompressedTGA(image, filename, fTGA);							// If so, jump to Compressed TGA loading code
	}
	else																	// If header matches neither type
	{
		fclose(fTGA);
		return false;																// Exit function
	}
	return true;															// All went well, continue on
}

bool LoadUncompressedTGA( GW_IMAGE* image, const char* filename, FILE* fTGA )
{																			// TGA Loading code nehe.gamedev.net)
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Read TGA header
	{										
		if(fTGA != NULL)													// if file is still open
		{
			fclose(fTGA);													// Close it
		}
		return false;														// Return failular
	}	

	image->width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	image->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	image->bpp	= tga.header[4];										// Determine the bits per pixel
	tga.Width		= image->width;										// Copy width into local structure						
	tga.Height		= image->height;										// Copy height into local structure
	tga.Bpp			= image->bpp;											// Copy BPP into local structure

	if((image->width <= 0) || (image->height <= 0) || ((image->bpp != 24) && (image->bpp !=32)))	// Make sure all information is valid
	{
		if(fTGA != NULL)													// Check if file is still open
		{
			fclose(fTGA);													// If so, close it
		}
		return false;														// Return failed
	}

	if(image->bpp == 24)													// If the BPP of the image is 24...
		image->format	= GW_PIXFMT_RGB;									// Set Image type to GL_RGB
	else																	// Else if its 32 BPP
		image->format	= GW_PIXFMT_RGBA;									// Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute the total amout ofmemory needed to store data
	image->imageData	= (GW_UINT8 *)malloc(tga.imageSize);					// Allocate that much memory

	if(image->imageData == NULL)											// If no space was allocated
	{
		fclose(fTGA);														// Close the file
		return false;														// Return failed
	}

	if(fread(image->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)	// Attempt to read image data
	{
		if(image->imageData != NULL)										// If imagedata has data in it
		{
			free(image->imageData);										// Delete data from memory
		}
		fclose(fTGA);														// Close file
		return false;														// Return failed
	}

	// Byte Swapping Optimized By Steve Thomas
	for(GW_UINT32 cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		image->imageData[cswap] ^= image->imageData[cswap+2] ^=
			image->imageData[cswap] ^= image->imageData[cswap+2];
	}

	fclose(fTGA);															// Close file
	return true;															// Return success
}

bool LoadCompressedTGA( GW_IMAGE* image, const char* filename, FILE* fTGA )
{ 
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)					// Attempt to read header
	{
		if(fTGA != NULL)													// If file is open
		{
			fclose(fTGA);													// Close it
		}
		return false;														// Return failed
	}

	image->width  = tga.header[1] * 256 + tga.header[0];					// Determine The TGA Width	(highbyte*256+lowbyte)
	image->height = tga.header[3] * 256 + tga.header[2];					// Determine The TGA Height	(highbyte*256+lowbyte)
	image->bpp	= tga.header[4];										// Determine Bits Per Pixel
	tga.Width		= image->width;										// Copy width to local structure
	tga.Height		= image->height;										// Copy width to local structure
	tga.Bpp			= image->bpp;											// Copy width to local structure

	if((image->width <= 0) || (image->height <= 0) || ((image->bpp != 24) && (image->bpp !=32)))	//Make sure all texture info is ok
	{
		if(fTGA != NULL)													// Check if file is open
		{
			fclose(fTGA);													// Ifit is, close it
		}
		return false;														// Return failed
	}

	if(image->bpp == 24)													// If the BPP of the image is 24...
		image->format	= GW_PIXFMT_RGB;									// Set Image type to GL_RGB
	else																	// Else if its 32 BPP
		image->format	= GW_PIXFMT_RGBA;									// Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);									// Compute BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);		// Compute amout of memory needed to store image
	image->imageData	= (GW_UINT8 *)malloc(tga.imageSize);					// Allocate that much memory

	if(image->imageData == NULL)											// If it wasnt allocated correctly..
	{
		fclose(fTGA);														// Close file
		return false;														// Return failed
	}

	GW_UINT32 pixelcount	= tga.Height * tga.Width;							// Nuber of pixels in the image
	GW_UINT32 currentpixel	= 0;												// Current pixel being read
	GW_UINT32 currentbyte	= 0;												// Current byte 
	GW_UINT8 * colorbuffer = (GW_UINT8 *)malloc(tga.bytesPerPixel);			// Storage for 1 pixel

	do
	{
		GW_UINT8 chunkheader = 0;											// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(GW_UINT8), 1, fTGA) == 0)				// Read in the 1 byte header
		{
			if(fTGA != NULL)												// If file is open
			{
				fclose(fTGA);												// Close file
			}
			if(image->imageData != NULL)									// If there is stored image data
			{
				free(image->imageData);									// Delete image data
			}
			return false;													// Return failed
		}

		if(chunkheader < 128)												// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																	// that follow the header
			chunkheader++;													// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++)		// Read RAW color values
			{
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
				{

					if(fTGA != NULL)													// See if file is open
					{
						fclose(fTGA);													// If so, close file
					}

					if(colorbuffer != NULL)												// See if colorbuffer has data in it
					{
						free(colorbuffer);												// If so, delete it
					}

					if(image->imageData != NULL)										// See if there is stored Image data
					{
						free(image->imageData);										// If so, delete it too
					}

					return false;														// Return failed
				}
				// write to memory
				image->imageData[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
				image->imageData[currentbyte + 1	] = colorbuffer[1];
				image->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												// if its a 32 bpp image
				{
					image->imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
				}

				currentbyte += tga.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}	

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(image->imageData != NULL)										// If there is Image data
					{
						free(image->imageData);										// delete it
					}

					return false;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
			{	

				if(fTGA != NULL)														// If thereis a file open
				{
					fclose(fTGA);														// Close it
				}

				if(colorbuffer != NULL)													// If there is data in the colorbuffer
				{
					free(colorbuffer);													// delete it
				}

				if(image->imageData != NULL)											// If thereis image data
				{
					free(image->imageData);											// delete it
				}

				return false;															// return failed
			}

			for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
			{																			// by the header
				image->imageData[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				image->imageData[currentbyte + 1	] = colorbuffer[1];
				image->imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												// If TGA images is 32 bpp
				{
					image->imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
				}

				currentbyte += tga.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}	

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(image->imageData != NULL)										// If there is Image data
					{
						free(image->imageData);										// delete it
					}

					return false;														// Return failed
				}
			}
		}
	}

	while(currentpixel < pixelcount);													// Loop while there are still pixels left
	fclose(fTGA);																		// Close the file
	return true;																		// return success
}

void UnloadTGAImage( GW_IMAGE* image )
{
	if (image->imageData)
	{
		free(image->imageData);
	}
}
