/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : ZclCallback.c
 * Description        : Cluster output source file for Matter.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019-2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

#include "AppTask.h"

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;

void MatterPostAttributeChangeCallback(const chip::app::ConcreteAttributePath &attributePath, uint8_t type, uint16_t size, uint8_t *value) {
    chip::ClusterId clusterId = attributePath.mClusterId;
    chip::AttributeId attributeId = attributePath.mAttributeId;

    // That's here that the values have to be listened to
}
