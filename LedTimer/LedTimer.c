/*
   Investigar o "escorregamento" do tempo de interrupção causado pelo atraso
   na recarga do timer.

   O timer0 é recarregado pela própria rotina de atendimento da interrupção.
   Há um atraso entre o momento da interrupção e o momento da recarga, com o
   consequente desvio do período de interrupção.

   O timer1 é programado em modo de recarga automática, evitando o problema.

   Os bits pares da porta P1 "acompanham" o timer1.
   Os bits impares de P1 "acompanham" o timer0.
   Assim o "escorregamento" fica evidente.
 */

#include <stdbool.h>
#include <8051.h>

volatile unsigned long int clocktime0;
volatile unsigned long int clocktime1;
volatile _Bool clockupdate0;
volatile _Bool clockupdate1;

/* reload = clock_f(MHz)*1000/12*period(ms)
          = 12MHz*1000/12*1ms
          = 1000 */
#define T0RELOAD 1000
#define TH0RLD   (65536 - T0RELOAD) / 256
#define TL0RLD   (65536 - T0RELOAD) % 256

/* ciclos de máquina com o timer0 parado na isr do timer0
   verificar o assembler gerado pelo compilador e contar os ciclos
 */
#if 0
/* 12T CPUs */
#define T0SLIP   19
#else
/* 1T CPUs */
#define T0SLIP   1
#endif

/* reload = clock_f(MHz)*1000/12*period(ms)
          = 12MHz*1000/12*0.25ms
          = 250 */
#define T1RELOAD 250
#define TH1RLD   (256 - T1RELOAD)

void clockinc0(void) __interrupt(1)
{

#if 1
  union
  {
    struct
    {
      unsigned char bytelow;
      unsigned char bytehigh;
    } bytes;
    unsigned int word;
  } now;

  /* Correção do "escorregamento"
     Parar timer0, ler valor, calcular novo valor, recarregar e partir
     CUIDADO!
     Se entrar uma interrupção durante esse trecho o cálculo pode ficar errado
   */
  TR0 = 0;
  now.bytes.bytelow = TL0;
  now.bytes.bytehigh = TH0;
  now.word += 65536 - T0RELOAD + T0SLIP;
  TL0 = now.bytes.bytelow;
  TH0 = now.bytes.bytehigh;
  TR0 = 1;
#else
  TH0 = TH0RLD;
  TL0 = TL0RLD;
#endif

  clocktime0++;
  clockupdate0 = true;
}

void clockinc1(void) __interrupt(3)
{
  clocktime1++;
  clockupdate1 = true;
}

unsigned long int clock0(void)
{
  unsigned long int ctmp;
  do
    {
      clockupdate0 = false;
      ctmp = clocktime0;
    } while(clockupdate0);

  return(ctmp);
}

unsigned long int clock1(void)
{
  unsigned long int ctmp;
  do
    {
      clockupdate1 = false;
      ctmp = clocktime1;
    } while(clockupdate1);

  return(ctmp/4);
}

void main(void)
{

  union
  {
    struct
    {
      unsigned char bit0:1;
      unsigned char bit1:1;
      unsigned char bit2:1;
      unsigned char bit3:1;
      unsigned char bit4:1;
      unsigned char bit5:1;
      unsigned char bit6:1;
      unsigned char bit7:1;
    } bits;
    unsigned char byte;
  } a, b;

  TH0 = TH0RLD;
  TL0 = TL0RLD;

  TH1 = TH1RLD;
  TL1 = TH1RLD;

  TMOD = 0x21;              /* timer0 16bits, timer1 8bits auto-reload */
  IE = 0x8a;                /* Habilita interrupção timer0 e timer 1*/
  TCON = 0x50;              /* Start timer0 e timer1*/

  for(;;)
    {
      // CUIDADO! overflow dos contadores pode trazer surpresas...
      a.byte = ~(clock0() / 1000);
      b.byte = ~(clock1() / 1000);

      // gera código pouco eficiente...
      P0_0 = b.bits.bit0;
      P0_1 = a.bits.bit0;
      P0_2 = b.bits.bit1;
      P0_3 = a.bits.bit1;
      P0_4 = b.bits.bit2;
      P0_5 = a.bits.bit2;
      P0_6 = b.bits.bit3;
      P0_7 = a.bits.bit3;
    }

}
