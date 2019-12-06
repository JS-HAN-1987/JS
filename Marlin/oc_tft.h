//
//  oc_tft.h
//
//  OpenCreators Firmware TFT LCD module header
//
//  2015. 3. 4.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//

typedef enum {
	TFTTYPE_S6D0154,
	TFTTYPE_ILI9325,
	TFTTYPE_HX8347G,
	TFTTYPE_C505,
	TFTTYPE_ILI9341
} tft_type_e;

extern tft_type_e tft_type;
