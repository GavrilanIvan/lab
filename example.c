#include <stdio.h>
#include <derivative.h>
char tof_factor = 0;	/*если переменная tof_factor = 0, то открываются 1-ый и 4-ый транзисторы, в противном случае – 2-			ой и 3-й*/
void kbi_setup(void);
void adc_setup(void);
void ports_setup(void);
void timer_setup(void);
void read_adc(unsigned char channel);
void current_level(void);
interrupt 5 void TPM_ISR(void);
interrupt 7 void TPM_ISR(void);
interrupt 1 void KBI_ISR(void);
 
void main(void){
	SOPT1_COPE = 0;	//отключение сторожевого таймера
	ports_setup();
	kbi_setup();
	adc_setup();
	timer_setup();
	EnableInterrupts;
	for( ; ; ){
		current_level(0x02);
	}
}

void timer_setup(void){
	TPMSC = 0x54;	/*разрешение прерываний по переполнению таймера, работа таймера в режиме односторонней ШИМ, 				тактирование таймера от шины микроконтроллера*/
	TPMMOD = 160; 		//максимальное значение счета таймера
	TPMC1SC = 0x50; 	/*разрешение прерывания по выходному сравнению*/
}

interrupt 7 void TPM_ISR(void){	//обработка прерывания по переполнению
	TPMSC_TOF = 0;			//сброс флага события прерывания
	if (tof_factor){			//если tof_factor = 0
		PTBD = 0x0A;		//открыть 1-ый и 4-ый транзисторы
		++tof_factor;
	}
	else{
		PTBD = 0x06;		//иначе открыть 2-ой и 3-й транзисторы
		tof_factor = 0;
	}
}
interrupt 7 void TPM_ISR(void){	//обработка прерывания по сравнению
	TPMC1SC_CH1F = 0;		//сброс флага события
	PTBD = 0x00;
}
void adc_setup(void){
	DCTL1 = 0x03;	/*запрет на прерывания по окончании преобразования, выбор 1-го канала преобразования*/
	ADCCFG = 0x03;	/*8-ми битный режим преобразования, тактирование АЦП от шины микроконтроллера*/
}
void read_adc(unsigned char channel){
	ADCSC1 = channel;		//выбор канала преобразования
	while(!ADCSC1_COCO){	//ожидание окончания преобразования
	}					//результат хранится в ячейке ADCRL
}
void current_level(void){	//вычисление нового сигнала управления
	char level_value += (read_adc(0x02) – 128);
	TPMMODH = 0x00;
	TPMMODL = level_value;	//задание нового уровня сравнения
}
void ports_setup(void){
	PTADD = 0x00;	/*настройка всех возможных портов PTAD.. на ввод*/
	PTBDD = 0xFF;	/*настройка всех возможных портов PTBD.. на вывод*/
}
void kbi_setup(void){
	KBISC = 0x03;	/*разрешение прерываний модулю KBI, работа входов в статическом режиме*/
	KBIPE = 0x09;	//выбор линии входа запроса на прервывание(PTAD3)
	KBIGS = 0x00;	/*KBI реагирует на передний фронт и высокий уровень сигнала*/
}
interrupt 1 void KBI_ISR(void){	//обработка прерываний KBI
	KBISC_KBACK = 1;		//сброс флага события
	PTBD = 0x00;			//запирание транзисторов
	DisableInterrupts;		//запрет на обработку прерываний
}
