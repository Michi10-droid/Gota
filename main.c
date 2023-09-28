/**
  ******************************************************************************
  * @file    main.c 
  * @author  MC José Miguel Morán Loza
  * @version V1.0.0
  * @date    20-September-2013
  * @brief   Main program body
  ******************************************************************************

  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"	//Archivo encabezado para el acceso a los 
												//dispositivos Periféricos del CMSIS Cortex-M4
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stdlib.h"
#include <math.h>
/** @addtogroup Template
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/

GPIO_InitTypeDef	GPIO_InitStructure;	//Inicialización de Terminales/Velocidad/Modo

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define VRES 320
#define HRES 240

#define X0 10  //Offset Referencia de los ejes
#define Y0 10 

#define NODES 20 //Define rejilla -> Creación de la rejilla
#define SIDE 10

#define STEP 1 //incrementos entre coordenadas

#define PI 3.1416
/* Private variables ---------------------------------------------------------*/
//inicializaciones 
/* Private function prototypes -----------------------------------------------*/

typedef struct { //Creación de la estructura point para referencias con respecto a x y y
	int x;
	int y;
}point;

point edge[NODES],prev;
void Delay(__IO uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< 	En este punto los ajustes del reloj del microcontrolador están 
				configurados, a través de la función SystemInit() la cual se llama
				desde el archivo startup (startup_stm32f429_439xx.s) antes de saltar
				a la aplicación principal.
				Para reconfigurar los ajustes predeterminados de la función SystemInit,
				refierase al documento system_stm32f4xx.c
  */  
  //Variables auxiliares para manejo de coordenadas
	int i, j, x, y, z;
	float xf, yf, zf, sf; 
	int px, py;
	int xoff, scale;
	
  /* Agregue el código de su aplicación aquí */ 
	/*Inicializar el LCD */
	LCD_Init();
	
	/*Configurar LCD: Configurar 2 capas w/ Blending and CLUT cargado*/
	
	/*Habilitar Capa 1*/
	LTDC_LayerCmd(LTDC_Layer1, ENABLE);
	
	/*Habilitar Capa 2*/
	LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	
	/*Recarga configuración LTDC*/
	LTDC_ReloadConfig(LTDC_IMReload);
	
	/*Habilitar el LCD*/
	LTDC_Cmd(ENABLE);
	
	/*Configuración de LCD*/
	LCD_LayerInit();
	/* Habilitación del Display del LCD*/
	LCD_DisplayOn();
	/*Set Foreground Layer*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	/*Clear the LCD*/
	LCD_Clear(LCD_COLOR_BLACK);
	/*Set the LCD Back Color*/
	LCD_SetBackColor(LCD_COLOR_BLACK);
	/*Set the LCD Text Color*/
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font12x12);
	
	//Condiciones de inicio: offset y escala
	xoff = 100;
	scale = 75;
	
	//LCD_DisplayStringLine(LCD_LINE_1, (uint8_t *) "ABCDEFGHIJ0123456789");
	//LCD_DisplayStringLine(LCD_LINE_2, (uint8_t *) "Michelle Pedraza Sal");
	
	
  while (1)
  {
		//Limpieza de la pantalla
		//Dibujado de los ejex x y x cruzando en X0,YO
		
		//LCD_DrawUniLine(X0, 10, X0, 10); //Eje z
		//LCD_DrawUniLine(X0-5, Y0, HRES-10, Y0); //Eje x
		//LCD_DrawUniLine(X0-2, Y0-2, X0+120, Y0+120); //Eje y
		LCD_DrawUniLine(X0, 10, X0, 10); //Eje z
		LCD_DrawUniLine(Y0, X0-5, Y0, HRES-10); //Eje x
		LCD_DrawUniLine(X0-2, Y0-2, X0+120, Y0+120); //Eje y
		
		
		
		//Inicializar el arreglo de los puntos previos del borde
		for(j=0; j<NODES; j++)
		{
			edge[j].x = X0+j*SIDE/2; //Ejes x y y (45° entre si)
			edge[j].y = Y0+j*SIDE/2;
		}
		
		//Dibujar la gráfica para la funcióm
		for(i=0; i<NODES; i++)  //Hasta el nodo 19
		{
			//Transformar el renago de x de 0..200 con un offset de 100
			x = i*SIDE; //eje x por 10
			xf = (6 * PI/200) * (float)(x-xoff); //definiendo x para la función
			prev.y = Y0;//Almacenando el valor previo de los puntos x y y
			prev.x = X0 + x;
			
			for (j=0; j<NODES; j++) //Hasta el nodo 19
			{
				//Transformar el rango de x de 0..200 con un offset de 100
				y = j*SIDE; //Eje x por 10
				yf = (6*PI/200) * (float)(y-100); //Definiendo y de la función
				
				//calculando la función
				sf = sqrt(xf*xf + yf*yf);
				zf = 1/(1+sf) * cos(sf); //Definiendo z de la función
				
				//Escalando la salida
				z = zf *scale; //scale = 75
				
				//Aplicando perspectiva Isométrica y el offset
				px = X0 + x + y/2; //Ejes x y y (45 entre si)
				py = Y0 + z + y/2;
				
				//Dibujar el punto
				//plot(px,py); //Dibujando el punto de la función
				//LCD_DrawUniLine(px, py, px, py); //Dibujando el punto de la función
				LCD_DrawUniLine(py, px, py, px); //Dibujando el punto de la función
				
				//Dibujar lineas de conexión para visualizarlas enla rejilla
				//LCD_DrawUniLine(px, py, prev.x, prev.y);
				//LCD_DrawUniLine(px, py, edge[j].x, edge[j].y); //Conectar el punto nuevo con los previos
				LCD_DrawUniLine(py, px, prev.y, prev.x);
				LCD_DrawUniLine(py, px, edge[j].y, edge[j].x); //Conectar el punto nuevo con los previos
				
				
				//Actualización de los puntos previos
				prev.x = px;
				prev.y = py;
				edge[j].x = px;
				edge[j].y = py;
				
			} //for j
		}//for i
	}
	//while(1);
}
    

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reportar del nombre del archivo fuente y el número de línea
  *			donde se "declara o mantiene" algún parámetro con error.
  * @param  file: Apunta al nombre del archivo fuente
  * @param  line: Número de líne que contiene el error.
  * @retval Ninguno
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* El usuario podrá agregar su propia implementación para reportar el nombre
  	 del archivo y el número de línea,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) 
	*/

  /* lazo infinito */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT 2011 Moran Technologies *****END OF FILE****/
