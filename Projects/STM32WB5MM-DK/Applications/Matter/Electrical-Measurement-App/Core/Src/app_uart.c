#include "app_uart.h"

#include "stm32wbxx_hal.h"
#include "stm32wbxx_ll_lpuart.h"

UART_HandleTypeDef hlpuart1         = {0};
UART_HandleTypeDef huart1           = {0};
DMA_HandleTypeDef  hdma_usart1_tx   = {0};
DMA_HandleTypeDef  hdma_lpuart1_rx  = {0};

void (*clbk_dma_uart1)(void)        = NULL;
void (*clbk_rx_lpuart1)(uint16_t)   = NULL;

static uint8_t* buff_lpuart_rx;
static uint16_t buff_lpuart_rx_size;
static volatile bool rx_requested   = false;

void HAL_UART_MspInit(UART_HandleTypeDef* huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
extern void Error_Handler(void);

bool AppUart_InitDebug(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_8;
    huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    return true;
}

bool AppUart_TransmitDmaDebug(uint8_t* data, size_t size, void (*callback)(void))
{
    clbk_dma_uart1 = callback;
    return (HAL_UART_Transmit_DMA(&huart1, data, size) == HAL_OK);
}

bool AppUart_TransmitDebug(uint8_t* data, size_t size)
{
    return (HAL_UART_Transmit_IT(&huart1, data, size) == HAL_OK);
}

bool AppUart_InitExternal(void (*rx_callback)(uint16_t))
{
    hlpuart1.Instance = LPUART1;
    hlpuart1.Init.BaudRate = 9600;
    hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
    hlpuart1.Init.StopBits = UART_STOPBITS_1;
    hlpuart1.Init.Parity = UART_PARITY_EVEN;
    hlpuart1.Init.Mode = UART_MODE_RX;
    hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV4;
    hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
    if (HAL_UART_Init(&hlpuart1) != HAL_OK)
    {
        Error_Handler();
    }

    clbk_rx_lpuart1 = rx_callback;

    return true;
}

void AppUart_NotifyIdleEventExternal(void)
{
    if (rx_requested)
    {
        rx_requested = false;

        __HAL_UART_CLEAR_FLAG(&hlpuart1, UART_CLEAR_OREF);
        // All bytes until an IDLE event or a buffer overflow will be received
        HAL_UARTEx_ReceiveToIdle_DMA(&hlpuart1, buff_lpuart_rx, buff_lpuart_rx_size);
    }
}

bool AppUart_ReceiveFrameExternal(uint8_t* buff, uint16_t buff_size)
{
    if (!rx_requested)
    {
        // Next time the UART line will be idle, a Rx will be triggered
        buff_lpuart_rx      = buff;
        buff_lpuart_rx_size = buff_size;
        rx_requested        = true;

        // Enable IDLE interrupt
        LL_LPUART_EnableIT_IDLE(hlpuart1.Instance);
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(huart->Instance==LPUART1)
    {
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
        PeriphClkInitStruct.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_LPUART1_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**LPUART1 GPIO Configuration
        PC0     ------> LPUART1_RX
        PB5     ------> LPUART1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);

        hdma_lpuart1_rx.Instance = DMA2_Channel5;
        hdma_lpuart1_rx.Init.Request = DMA_REQUEST_LPUART1_RX;
        hdma_lpuart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_lpuart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_lpuart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_lpuart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_lpuart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_lpuart1_rx.Init.Mode = DMA_NORMAL;
        hdma_lpuart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
        if (HAL_DMA_Init(&hdma_lpuart1_rx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(huart, hdmarx, hdma_lpuart1_rx);

        HAL_NVIC_SetPriority(LPUART1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(LPUART1_IRQn);
    }
    else if(huart->Instance==USART1)
    {
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PB7     ------> USART1_RX
        PB6     ------> USART1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        __HAL_RCC_DMAMUX1_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        HAL_NVIC_SetPriority(DMA2_Channel4_IRQn, 9, 0);
        HAL_NVIC_EnableIRQ(DMA2_Channel4_IRQn);

        hdma_usart1_tx.Instance = DMA2_Channel4;
        hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
        hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_LINKDMA(huart, hdmatx, hdma_usart1_tx);

        HAL_NVIC_SetPriority(USART1_IRQn, 10, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if ((huart == &hlpuart1) && (huart->RxEventType == HAL_UART_RXEVENT_TC || huart->RxEventType == HAL_UART_RXEVENT_IDLE) && (clbk_rx_lpuart1 != NULL))
    {
    	clbk_rx_lpuart1(size);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if ((huart == &huart1) && (clbk_dma_uart1 != NULL))
	{
		clbk_dma_uart1();
	}
}
