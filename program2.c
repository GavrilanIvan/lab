#include "stdio.h"
#include "math.h"
#include "derivative.h"

unsigned char adc_value
unsigned char level_value

void timer_setup()				//Настройка таймера	
{
	TPMSC = 0x59;			*/Разрешение прерываний по переполнению 								таймера, работа таймера в режиме 									односторонней ШИМ, тактирование таймера от 						внешнего источника/*
	TPMMODH = 0x00;		
	TPMMODL = 0xFF;			*/Максимальное значение счета таймера/*
	TPMS1SC = 0x50;			*/Разрешение прерывания по выходному 								сравнению,/*
	return 0;
}

void adc_setup()				//Настройка АЦП
{
	ADCTL1 = 0x00;			//запрет на прерывания по окончании 									преобразования, выбор 0-го канала 									преобразования
	ADCCFG = 0x03;			//8-ми битный режим преобразования, 									тактирование АЦП от альтернативного 									генератора (такой же, как и для таймера)
	return 0;
}

void ports_setup()				//Настройка портов ввода-вывода
{	
	PTADD = 0x00;			//разворот портов PTAD.. на ввод
	PTBDD = 0xFF;			//разворот портов PTBD.. на вывод
	return 0;
}

void stopWD()					//Отключение сторожевого таймера
{
	*0x1802 = 0x53;
	return 0;
}

void kbi_setup()
{
	KBISC_KBIE = 1;
	KBISC_KBMMOD = 1
}

void settings()					//Настройка всех модулей
{
	stopWD();
	ports_setup();
	timer_setup();
	adc_setup();
	kbi_setup();
	return 0;
}

unsigned char read_adc_channel(unsigned char ADCSC)	//ПП чтения первого канала АЦП
{
	ADCSC1 = ADCSC;
	while (!ADCSC1_COCO)		//ожидание окончания преобразования сигнала
	{

	}
	return &ADCRL;			
}

unsigned char current_level()			//ПП вычисления уровня управления
{
	level_value += (adc_value_1 - adc_value_2);
	return level_value;	
}

interrupt 10 void KBI_ISR (void)
{
	KBISC_KBACK = 1
	PTBD = 0x00;
	DisableInterrupts;
}

interrupt 5 void TPMCH1_ISR(void)		//обработка прерывания по выходному 									сравнению
{
	TPMC1SC_CH0F = 0;			//при событии выходного сравнения выходные 								порты закрываются
	PTBD = 0x00;
}

interrupt 7 void TPM_ISR(void)		//обработка прерывания по переполнению 								счетчика	
{
	TPMSC_TOF = 0;
	if (!tof_factor)
	{
		PTBD = 0x00;
		tof_factor++;			//если tof_factor = 1, то открываются 2 и 3 								транзисторы, если 0 - то 1 и 4
	}
	else
	{
		PTBD = 0x02;
		tof_factor = 0;
	}
		
}

void main() 
{
	settings();
	EnableInterrupts;
	for(;;)
	{
		adc_value_1 = read_adc_channel(0x01);
		adc_value_2 = read_adc_channel(0x02);
		TPMC1VL = current_level();	//задание нового уровня сравнения
		TPMC1VH = 0;
	}
	return 0;
}