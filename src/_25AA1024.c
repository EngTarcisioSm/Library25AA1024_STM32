/* USER CODE BEGIN Header */
/**
 * @file           : _25AA1024.c
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Author:
 * Date: 11/07/2022
 *
 * Description:
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "_25AA1024.h"

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
static SPI_HandleTypeDef *hspiCOM;
static GPIO_TypeDef *ulPortCS24aa1024;
static uint16_t usPinCS24aa1024;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void v25AA1024_EnableCS(void);
void v25AA1024_DisableCS(void);
void v25AA1024_Delay(void);
/* USER CODE END PFP */

/* Private function ----------------------------------------------------------*/
/* USER CODE BEGIN FUNCTION */

/**
 * @brief Inicializa varáveis necessárias para a comunicação com a memoria spi
 * deve ser inicializada antes do uso das demais funções
 *
 * @param xHandleSPI handle SPI
 * @param ulPortCS Port do GPIO CS da memoria
 * @param usPinCS GPIO da memoria
 *
 * tested
 */
void v25AA1024_Init(SPI_HandleTypeDef *xHandleSPI, GPIO_TypeDef *ulPortCS,
					uint16_t usPinCS)
{
	hspiCOM = xHandleSPI;
	ulPortCS24aa1024 = ulPortCS;
	usPinCS24aa1024 = usPinCS;
}

/**
 * @brief Função de uso interno para habilitar a comunicação com a memoria levando
 * o GPIO de CS a zero
 *
 * tested
 */
void v25AA1024_EnableCS(void)
{
	//	HAL_GPIO_WritePin(DO_MEM_CS_GPIO_Port, DO_MEM_CS_Pin, 0);
	HAL_GPIO_WritePin(ulPortCS24aa1024, usPinCS24aa1024, 0);
}

/**
 * @brief Função de uso interno para desabilitar a comunicação com a memoria levando
 * o GPIO de CS a nivel logico alto
 *
 * tested
 */
void v25AA1024_DisableCS(void)
{
	//	HAL_GPIO_WritePin(DO_MEM_CS_GPIO_Port, DO_MEM_CS_Pin, 1);
	HAL_GPIO_WritePin(ulPortCS24aa1024, usPinCS24aa1024, 1);
}

/**
 * @brief leitura de dados presentes na memoria
 *
 * @param ulAddressMen endereço a partir do qual será feita a leitura
 * @param usQtd quantidade de dados a serem lidos
 * @param pucBuffer ponteiro para armazenamento dos dados
 * @return xStatus25AA1024
 * _25aa1024_FAIL = falha na leitura de dados
 * _25aa1024_OK = sucesso na leitura de dados
 *
 * tested
 */
xStatus25AA1024 x25AA1024_ReadMen(uint32_t ulAddressMen, uint16_t usQtd,
								  uint8_t *pucBuffer)
{
	uint8_t ucMsgRead[4];

	v25AA1024_EnableCS();
	/*COMANDO DE LEITURA*/
	ucMsgRead[0] = 0b00000011;
	/*ORGANIZAÇÃO DO ENDEREÇO A SER LIDO*/
	ucMsgRead[1] = (ulAddressMen >> 16);
	ucMsgRead[2] = (ulAddressMen >> 8);
	ucMsgRead[3] = ulAddressMen;

	if ((HAL_SPI_Transmit(hspiCOM, &ucMsgRead, 4, _25aa1024_TIMEOUT)) != HAL_OK)
	{
		v25AA1024_DisableCS();
		return _25aa1024_FAIL;
	}

	if ((HAL_SPI_Receive(hspiCOM, pucBuffer, usQtd, _25aa1024_TIMEOUT)) != HAL_OK)
	{
		v25AA1024_DisableCS();
		return _25aa1024_FAIL;
	}

	v25AA1024_DisableCS();
	return _25aa1024_OK;
}

/**
 * @brief Escrita na memoria
 *
 * @param ulAddressMen endereço a partir do qual a escrita irá ocorrer
 * @param usQtd quantidade de dados a ser escritos (quantos bytes)
 * @param pucBuffer ponteiro de dados
 * @return xStatus25AA1024
 * _25aa1024_FAIL = falha na escrita
 * _25aa1024_OK = sucesso na escrita
 *
 * tested
 */
xStatus25AA1024 x25AA1024_WriteMen(uint32_t ulAddressMen, uint16_t usQtd,
								   uint8_t *pucBuffer)
{
	uint8_t ucMsgWrite[260];
	uint8_t ucAux;

	/*NÃO É PERMITIDO ESCRITA DE MAIS DE 256 BYTES POR VEZ*/
	if (usQtd > 256)
	{
		return _25aa1024_FAIL;
	}
	memset(ucMsgWrite, 0x00, 260);


	/*Habilita escrita na memoria */
	if( x25AA1024_EnableWriteMen() == _25aa1024_FAIL ) {
		return _25aa1024_FAIL;
	}

	v25AA1024_EnableCS();

	/*ORGANIZA FUNÇÃO DE ESCRITA NO BUFFER*/
	ucMsgWrite[0] = 0b00000010;

	/*ORGANIZA ENDEREÇO A SER ESCRITO NA FUNÇÃO*/
	ucMsgWrite[1] = (ulAddressMen >> 16);
	ucMsgWrite[2] = (ulAddressMen >> 8);
	ucMsgWrite[3] = (ulAddressMen);

	/*COPIA BUFFER PARA O BUFFER DA FUNÇÃO*/
	for (ucAux = 0; ucAux < usQtd; ++ucAux)
	{
		ucMsgWrite[4 + ucAux] = *pucBuffer;
		pucBuffer++;
	}

	/*ENVIA PACOTE PELO SPI*/
	if ((HAL_SPI_Transmit(hspiCOM, &ucMsgWrite, usQtd + 4, _25aa1024_TIMEOUT)) != HAL_OK)
	{
		v25AA1024_DisableCS();
		return _25aa1024_FAIL;
	}

	v25AA1024_DisableCS();
	return _25aa1024_OK;
}

/**
 * @brief Efetua a leitura do Registrador de Status
 *
 * @return xStatus25AA1024
 * _25aa1024_FAIL = falha no processo de leitura dos registradores
 * other = dados do registrador,
 *
 * tested
 */
xStatus25AA1024 x25AA1024_ReadStatus(void)
{
	uint8_t ucMsgRead;
	ucMsgRead = 0b00000101;

	v25AA1024_EnableCS();
	{
		if ((HAL_SPI_Transmit(hspiCOM, &ucMsgRead, 1, _25aa1024_TIMEOUT)) != HAL_OK)
		{
			v25AA1024_DisableCS();
			return _25aa1024_FAIL;
		}
		if ((HAL_SPI_Receive(hspiCOM, &ucMsgRead, 1, _25aa1024_TIMEOUT)) != HAL_OK)
		{
			v25AA1024_DisableCS();
			return _25aa1024_FAIL;
		}
		v25AA1024_DisableCS();
		return (0b01110000 | ucMsgRead);
	}
	v25AA1024_DisableCS();
}

/**
 * @brief Verifica se gravação ja foi finalizada
 *
 * @return xStatus25AA1024
 * _25aa1024_WRITE_NOT_FINISHED = escrita não finalizada
 * _25aa1024_WRITE_FINISHED = escrita finalizada
 *
 * TESTED
 */
xStatus25AA1024 x25AA1024_WriteStatus(void)
{
	xStatus25AA1024 xStatusAll;
	xStatusAll = x25AA1024_ReadStatus();
	if (xStatusAll & 0b00000001)
	{
		return _25aa1024_WRITE_NOT_FINISHED;
	}
	else
	{
		return _25aa1024_WRITE_FINISHED;
	}
}

/**
 * @brief Verifica se a escrita esta disponível na memória
 *
 * @return xStatus25AA1024
 * _25aa1024_WRITE_AVAILABLE = escrita disponivel
 * _25aa1024_WRITE_NOT_AVAILABLE = escrita não disponivel
 *
 * TESTED
 */
xStatus25AA1024 x25AA1024_WriteAvailable(void)
{
	xStatus25AA1024 xStatusAll;
	xStatusAll = x25AA1024_ReadStatus();
	if (xStatusAll & 0b00000010)
	{
		return _25aa1024_WRITE_AVAILABLE;
	}
	else
	{
		return _25aa1024_WRITE_NOT_AVAILABLE;
	}
}

/**
 * @brief verifica as regições de memoria protegidas
 *
 * @return xStatus25AA1024
 * _25aa1024_NOT_PROTECTED = sem proteção nenhuma
 * _25aa1024__PROTECTED_1_4_SUP = 1/4 superior da memoria protegida
 * _25aa1024__PROTECTED_1_2_SUP = 1/2 superior da memoria protegida
 * _25aa1024__PROTECTED_ALL = toda a memoria protegida
 * 
 * TESTED
 */
xStatus25AA1024 x25AA1024_ProtectedStatus(void)
{
	xStatus25AA1024 xStatusAll;
	xStatusAll = x25AA1024_ReadStatus();
	xStatusAll = (xStatusAll & 0b00001100);

	switch (xStatusAll)
	{
	case 0b00000000 /* Sem proteção nenhuma */:
		return _25aa1024_NOT_PROTECTED;
		break;
	case 0b00000100 /* 1/4 Superior  */:
		return _25aa1024__PROTECTED_1_4_SUP;
		break;
	case 0b00001000 /* Metade Superior */:
		return _25aa1024__PROTECTED_1_2_SUP;
		break;
	case 0b00001100 /* Toda */:
		return _25aa1024__PROTECTED_ALL;
		break;
	default /*Erro*/:
		return _25aa1024_FAIL;
		break;
	}
}

/**
 * @brief Protege regiões da memoria
 *
 * @param ucConfigProtect
 * _25aa1024_PROTECT_OFF = memoria totalmente desprotegida contra escrita.
 * _25aa1024_PROTECT_1_4_SUP = 1/4 superior protegido contra escrita.
 * _25aa1024_PROTECT_1_2_SUP = 1/2 superior protegida contra escrita.
 * _25aa1024_PROTECT_ALL = totalmente protegido contra escrita.
 * @return xStatus25AA1024
 * _25aa1024_FAIL = memoria não respondeu;
 * _25aa1024_OK = ação efetuada;
 * 
 * TESTED
 */
xStatus25AA1024 x25AA1024_ProtectMemory(uint8_t ucConfigProtect)
{
	uint8_t ucBufferWrite[2];
	ucBufferWrite[0] = 0b00000001;
	switch (ucConfigProtect)
	{
	case _25aa1024_PROTECT_OFF:
		/* code */
		ucBufferWrite[1] = 0b00000000;
		break;
	case _25aa1024_PROTECT_1_4_SUP:
		/* code */
		ucBufferWrite[1] = 0b00000100;
		break;
	case _25aa1024_PROTECT_1_2_SUP:
		/* code */
		ucBufferWrite[1] = 0b00001000;
		break;
	case _25aa1024_PROTECT_ALL:
		/* code */
		ucBufferWrite[1] = 0b00001100;
		break;

	default:
		return;
		break;
	}

	if (x25AA1024_EnableWriteMen())
	{
		v25AA1024_EnableCS();
		{
			if ((HAL_SPI_Transmit(hspiCOM, &ucBufferWrite, 2, _25aa1024_TIMEOUT)) != HAL_OK)
			{
				v25AA1024_DisableCS();
				return _25aa1024_FAIL;
			}
			else
			{
				v25AA1024_DisableCS();
				return _25aa1024_OK;
			}
		}
	}
	else
	{
		return _25aa1024_FAIL;
	}
}

/**
 * @brief Habilita a escrita na memoria
 *
 * @return xStatus25AA1024
 * _25aa1024_FAIL = memoria não respondeu;
 * _25aa1024_OK = escrita liberada
 *
 * TESTED
 */
xStatus25AA1024 x25AA1024_EnableWriteMen(void)
{
	uint8_t ucMsgWrite = 0b00000110;
	uint8_t ucAux = 0x0A;

	while(--ucAux)
	{
		v25AA1024_EnableCS();
		/*Habilita escrita na memoria */
		if ((HAL_SPI_Transmit(hspiCOM, &ucMsgWrite, 1, _25aa1024_TIMEOUT)) != HAL_OK)
		{
			v25AA1024_DisableCS();
			return _25aa1024_FAIL;
		}
		v25AA1024_DisableCS();
		v25AA1024_Delay();
		if(x25AA1024_WriteStatus() == _25aa1024_WRITE_FINISHED) {
			break;
		}
	}

	if(ucAux) {
		return _25aa1024_OK;
	} else {
		return _25aa1024_FAIL;
	}
}

/**
 * @brief Função de delay necessário para algumas operações da memoria 
 * 
 */
void v25AA1024_Delay(void){
	HAL_Delay(2);
}

/**
 * @brief Deleta pagina da memoria, composta por 512páginas, cada página contendo 256bytes
 * 
 * @param usPage número da página, iniciando na página 0 
 * @return xStatus25AA1024 
 * _25aa1024_FAIL = memoria não respondeu;
 * _25aa1024_OK = memoria recebeu o comando
 * 
 * TESTED
 */
xStatus25AA1024 x25AA1024_ErasePage(uint16_t usPage) {
	uint8_t ucBuffer[4];
	uint32_t ulCalcPage = (uint32_t)usPage * 256; 

	ucBuffer[0] = 0b01000010;
	ucBuffer[1] = (ulCalcPage >> 16);
	ucBuffer[2] = (ulCalcPage >> 8);
	ucBuffer[3] = (ulCalcPage);

	if(x25AA1024_EnableWriteMen()  == _25aa1024_FAIL) {
		return _25aa1024_FAIL;
	}
	v25AA1024_EnableCS();
	{
		if(HAL_SPI_Transmit(hspiCOM, &ucBuffer, 4, _25aa1024_TIMEOUT) != HAL_OK) {
			return _25aa1024_FAIL;
		} else {
			return _25aa1024_OK;
		}
	}
	v25AA1024_DisableCS();
}
/* USER CODE END FUNCTION */

/* Tasks FreeRTOS ------------------------------------------------------------*/
/* USER CODE BEGIN TASKS FREERTOS */

/* USER CODE END TASKS FREERTOS */

#ifdef _25AA1024DEBUG
/* DEBUG ---------------------------------------------------------------------*/
/* USER CODE DEBUG */

/* USER CODE END TASKS FREERTOS */

#endif
