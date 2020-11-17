#include "maps_YUV422Converter.h" // Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSYUV422Converter)
MAPS_INPUT("input", MAPS::FilterIplImage, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSYUV422Converter)
MAPS_OUTPUT("output", MAPS::MAPSImage, NULL, NULL, 0)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSYUV422Converter)
//
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSYUV422Converter)
//MAPS_ACTION("aName",MAPSYUV422Converter::ActionName)
MAPS_END_ACTIONS_DEFINITION

MAPS_COMPONENT_DEFINITION(MAPSYUV422Converter, "YUV422Converter", "1.0", 128,
						  MAPS::Threaded | MAPS::Sequential, MAPS::Threaded,
						  -1, // Nb of inputs
						  -1, // Nb of outputs
						  -1, // Nb of properties
						  -1) // Nb of actions

void MAPSYUV422Converter::Birth()
{
	Output(0).AllocOutputBufferMAPSImage(1280 * 960 * 2, 1280, 960, MAPS_IMAGECODING_UYVY, MAPS_IMAGETYPE_COLOR);
}

void MAPSYUV422Converter::Core()
{
	try
	{
		MAPSIOElt *ioEltIn = StartReading(Input(0));
		if (NULL == ioEltIn)
			return;

		IplImage &imageIn = ioEltIn->IplImage();

		MAPSIOElt *ioEltOut = StartWriting(Output(0));
		MAPSImage &imageOut = ioEltOut->MAPSImage();
		
		RGB888_BGRtoYUV422_UYVY(imageIn, imageOut, (imageIn.width * imageIn.height)); // BGR to YUV with image Resolution

		imageOut->Timestamp() = ioEltIn->Timestamp();
		StopWriting(imageOut);
	}
	catch (const std::exception &e)
	{
		Error(e.what());
	}
}
void MAPSYUV422Converter::Death()
{
}

void MAPSYUV422Converter::RGB888_BGRtoYUV422_UYVY(const IplImage &imageIn, MAPSImage &imageOut,
												  unsigned int u32ImageSize)
{
#define SATURATE(x) (((x) > 255) ? (255) : (((x) < 0) ? (0) : (x)))

	unsigned char *ptr_u8In_BGR = (unsigned char *)imageIn.imageData; //IPL to uchar
	unsigned char *ptr_u8Out_YUV = (unsigned char *)imageOut.imageData;

	double RData0, RData1;
	double GData0, GData1;
	double BData0, BData1;

	double YData0, YData1;
	double UData, VData;
	unsigned long u32loopCounter;

	u32ImageSize = (u32ImageSize * 2); //image size.

	/*Loop mapping RGB to YUV image.*/
	for (u32loopCounter = 0; u32loopCounter < u32ImageSize; u32loopCounter += 4)
	{
		BData0 = (*ptr_u8In_BGR);
		ptr_u8In_BGR++;
		GData0 = (*ptr_u8In_BGR);
		ptr_u8In_BGR++;
		RData0 = (*ptr_u8In_BGR);
		ptr_u8In_BGR++;

		BData1 = (*ptr_u8In_BGR);
		ptr_u8In_BGR++;
		GData1 = (*ptr_u8In_BGR);
		ptr_u8In_BGR++;
		RData1 = (*ptr_u8In_BGR);
		ptr_u8In_BGR++;

		YData0 = RData0 * 0.2990000 + GData0 * 0.5870000 + BData0 * 0.1140000;
		YData1 = RData1 * 0.2990000 + GData1 * 0.5870000 + BData1 * 0.1140000;

		UData = ((RData0 + RData1) / 2) * -0.1471300 + ((GData0 + GData1) / 2) * -0.2888600 + ((BData0 + BData1) / 2) * 0.4360000;
		VData = ((RData0 + RData1) / 2) * 0.6150000 + ((GData0 + GData1) / 2) * -0.5149900 + ((BData0 + BData1) / 2) * -0.1000100;

		// U Data
		*(unsigned char *)ptr_u8Out_YUV = (unsigned char)SATURATE(UData + 128);
		(ptr_u8Out_YUV)++;
		// Y 0 Data
		*(unsigned char *)ptr_u8Out_YUV = (unsigned char)SATURATE(YData0);
		(ptr_u8Out_YUV)++;
		// V Data
		*(unsigned char *)ptr_u8Out_YUV = (unsigned char)SATURATE(VData + 128);
		(ptr_u8Out_YUV)++;
		// Y 1 Data
		*(unsigned char *)ptr_u8Out_YUV = (unsigned char)SATURATE(YData1);
		(ptr_u8Out_YUV)++;
	}
}
