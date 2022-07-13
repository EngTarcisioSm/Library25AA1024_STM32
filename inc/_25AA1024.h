/* USER CODE BEGIN Header */
/**
 * @file           : _25AA1024.h
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Author: Eng. Tarcísio Souza de Melo
 * Date: 11/07/2022
 *
 * Description:
 * Biblioteca de uso liberado sem a remoção do nome do autor
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ___25AA1024_H
#define ___25AA1024_H

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef uint8_t xStatus25AA1024;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define _25AA1024DEBUG
#define _25aa1024_TIMEOUT 100

#define _25aa1024_FAIL                  0x00
#define _25aa1024_OK                    0x01
#define _25aa1024_WRITE_FINISHED        0x02
#define _25aa1024_WRITE_NOT_FINISHED    0x03
#define _25aa1024_WRITE_AVAILABLE       0x04
#define _25aa1024_WRITE_NOT_AVAILABLE   0x05
#define _25aa1024_NOT_PROTECTED         0x06
#define _25aa1024__PROTECTED_1_4_SUP    0x07
#define _25aa1024__PROTECTED_1_2_SUP    0x08
#define _25aa1024__PROTECTED_ALL        0x09

#define _25aa1024_PROTECT_1_4_SUP       0x01
#define _25aa1024_PROTECT_1_2_SUP       0x02
#define _25aa1024_PROTECT_ALL           0x03
#define _25aa1024_PROTECT_OFF           0x00
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void v25AA1024_Init(SPI_HandleTypeDef *xHandleSPI, GPIO_TypeDef *ulPortCS, uint16_t usPinCS);

xStatus25AA1024 x25AA1024_ReadMen(uint32_t ulAddressMen, uint16_t usQtd, uint8_t *pucBuffer);

xStatus25AA1024 x25AA1024_WriteMen(uint32_t ulAddressMen, uint16_t usQtd, uint8_t *pucBuffer);

xStatus25AA1024 x25AA1024_ReadStatus(void);

xStatus25AA1024 x25AA1024_WriteStatus(void);

xStatus25AA1024 x25AA1024_WriteAvailable(void);

xStatus25AA1024 x25AA1024_ProtectedStatus(void);

xStatus25AA1024 x25AA1024_ProtectMemory(uint8_t ucConfigProtect);

xStatus25AA1024 x25AA1024_EnableWriteMen(void);

xStatus25AA1024 x25AA1024_ErasePage(uint16_t usPage);
/* USER CODE END PFP */


#endif /* ___25AA1024_H */

/*******************************END OF FILE************************************/
