
#ifndef _Maps_YUV422Converter_H
#define _Maps_YUV422Converter_H

#include "maps.hpp"
#include <iostream>

class MAPSYUV422Converter : public MAPSComponent
{
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSYUV422Converter)
	void RGB888_BGRtoYUV422_UYVY(const IplImage &ptr_u8BGR, MAPSImage &ptr_u8YUV,
								 unsigned int u32ImageSize);

private:
};

#endif
