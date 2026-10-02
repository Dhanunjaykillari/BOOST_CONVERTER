#include "F28x_Project.h"

#define TBPRD_VALUE     5000
#define PWM_FREQ        20000.0f
#define Ts              (1.0f / PWM_FREQ)

#define ADC_VREF        3.3f
#define ADC_RESOLUTION  4095.0f

#define VDC_REF         400.0f

#define FILTER_ALPHA    0.1f

#define DUTY_MAX        0.95f
#define DUTY_MIN        0.05f

#define KP_VDC          0.001f
#define KI_VDC          0.5f

#define PI_A            (KP_VDC + KI_VDC * Ts)
#define PI_B            (-KP_VDC)

volatile Uint16 vadc = 0;

volatile float vadcVoltage = 0.0f;
volatile float vdcMeasured = 0.0f;
volatile float vdcFiltered = 0.0f;

volatile float vdcError = 0.0f;
volatile float vdcError_k_1 = 0.0f;

volatile float duty = 0.5f;
volatile float duty_k_1 = 0.5f;


void Setup_ePWM1(void);
void Setup_ADC(void);

static inline float DC_Voltage_Controller(float error);

__interrupt void adcc1ISR(void);


void main(void)
{
    InitSysCtrl();

    DINT;

    InitPieCtrl();

    IER = 0;
    IFR = 0;

    InitPieVectTable();

    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;

    PieVectTable.ADCC1_INT = &adcc1ISR;

    EDIS;

    Setup_ePWM1();
    Setup_ADC();

    PieCtrlRegs.PIEIER1.bit.INTx3 = 1;

    IER |= M_INT1;

    EPwm1Regs.CMPA.bit.CMPA = (Uint16)(duty * TBPRD_VALUE);

    EINT;
    ERTM;

    for(;;)
    {
        asm(" NOP");
    }
}


__interrupt void adcc1ISR(void)
{
    /* Read ADC */
    vadc = AdccResultRegs.ADCRESULT0;

    /* ADC count to voltage */
    vadcVoltage = ((float)vadc / ADC_RESOLUTION) * ADC_VREF;

    /* ADC voltage to DC voltage */
    vdcMeasured = vadcVoltage;

    /* Low-pass filter */
    vdcFiltered = FILTER_ALPHA * vdcMeasured + (1.0f - FILTER_ALPHA) * vdcFiltered;

    /* Voltage error */
    vdcError = VDC_REF - vdcFiltered;

    /* Voltage PI */
    duty = DC_Voltage_Controller(vdcError);

    /* Update PWM */
    EPwm1Regs.CMPA.bit.CMPA =(Uint16)(duty * TBPRD_VALUE);

    /* Clear ADC flag */
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    /* Acknowledge interrupt */
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


static inline float DC_Voltage_Controller(float error)
{
    float duty_unsat;
    float duty_sat;

    /* Incremental PI */
    duty_unsat = duty_k_1 + PI_A * error + PI_B * vdcError_k_1;

    /* Duty limit */
    duty_sat = duty_unsat;

    if(duty_sat > DUTY_MAX)
        duty_sat = DUTY_MAX;

    if(duty_sat < DUTY_MIN)
        duty_sat = DUTY_MIN;

    /* Anti-windup */
    if(duty_unsat == duty_sat)
    {
        duty_k_1 = duty_unsat;
    }

    /* Previous error */
    vdcError_k_1 = error;

    return duty_sat;
}


void Setup_ePWM1(void)
{
    EALLOW;

    /* Count-up mode */
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;

    EPwm1Regs.TBPRD = TBPRD_VALUE;
    EPwm1Regs.TBCTR = 0;

    /* Initial duty = 50% */
    EPwm1Regs.CMPA.bit.CMPA = 2500;

    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    /* PWM output */
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;

    /* PWM trigger ADC */
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;

    EDIS;
}


void Setup_ADC(void)
{
    EALLOW;

    /* ADC clock */
    AdccRegs.ADCCTL2.bit.PRESCALE = 6;

    /* ADC C, 12-bit, single-ended */
    AdcSetMode(ADC_ADCC,ADC_RESOLUTION_12BIT,ADC_SIGNALMODE_SINGLE);

    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    /* Power ON ADC */
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    EDIS;

    DELAY_US(1000);

    EALLOW;

    /* ADC C5 */
    AdccRegs.ADCSOC0CTL.bit.CHSEL = 5;

    /* Acquisition window */
    AdccRegs.ADCSOC0CTL.bit.ACQPS = 15;

    /* ePWM1 SOCA trigger */
    AdccRegs.ADCSOC0CTL.bit.TRIGSEL = 5;

    /* EOC0 generates ADCINT1 */
    AdccRegs.ADCINTSEL1N2.bit.INT1SEL = 0;

    /* Enable ADC interrupt */
    AdccRegs.ADCINTSEL1N2.bit.INT1E = 1;

    AdccRegs.ADCINTSEL1N2.bit.INT1CONT = 0;

    /* Clear ADC flag */
    AdccRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    EDIS;
}
// end of the file 


