//
//  oc_encoder.h
//
//  Encoder module header
//
//  2015. 4. 15.
//  Created by James Kang
//  Copyright (c) 2014-2015 OpenCreators, Inc. All rights reserved.
//

#ifndef __OC_ENCODER_H__
#define __OC_ENCODER_H__

void encoder_init(void);
void encoder_update(void);
void encoder_process(void);
bool encoder_released(void);

#endif // __OC_ENCODER_H__

