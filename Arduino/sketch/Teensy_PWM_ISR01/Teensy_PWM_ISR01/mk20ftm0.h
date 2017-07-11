#define FTM_QDCTRL_QUADEN            (1 << 0)  // Bit 0:  Quadrature Decoder Mode Enable
#define FTM_PWMLOAD_LDOK_MASK        (1 << 9)  // Bit 9: Load Enable
                                               // Bits 10-31: Reserved
#define FTM_CnSC_CHF		(1 << 7)
#define FTM_CnSC_CHIE		(1 << 6)
#define FTM_CnSC_MSB		(1 << 5)
#define FTM_CnSC_MSA		(1 << 4)
#define FTM_CnSC_ELSB		(1 << 3)
#define FTM_CnSC_ELSA		(1 << 2)
#define FTM_CnSC_DMA		(1 << 0)

// Function for Linked Channels 
#define FTM_COMBINE_COMBINE0 (1 << 0)  // Bit 0: Combine Channels for n = 0 
#define FTM_COMBINE_COMP0 (1 << 1)     // Bit 1: Complement of Channel (n) for n = 0 
#define FTM_COMBINE_DECAPEN0 (1 << 2)  // Bit 2: Dual Edge Capture Mode Enable for n = 0 
#define FTM_COMBINE_DECAP0 (1 << 3)    // Bit 3: Dual Edge Capture Mode Captures for n = 0 
#define FTM_COMBINE_DTEN0 (1 << 4)     // Bit 4: Deadtime Enable for n = 0 
#define FTM_COMBINE_SYNCEN0 (1 << 5)   // Bit 5: Synchronization Enable for n = 0 
#define FTM_COMBINE_FAULTEN0 (1 << 6)  // Bit 6: Fault Control Enable for n = 0 
                                       // Bit 7: Reserved 
#define FTM_COMBINE_COMBINE1 (1 << 8)  // Bit 8: Combine Channels for n = 2 
#define FTM_COMBINE_COMP1 (1 << 9)     // Bit 9: Complement of Channel (n) for n = 2 
#define FTM_COMBINE_DECAPEN1 (1 << 10) // Bit 10: Dual Edge Capture Mode Enable for n = 2 
#define FTM_COMBINE_DECAP1 (1 << 11)   // Bit 11: Dual Edge Capture Mode Captures for n = 2 
#define FTM_COMBINE_DTEN1 (1 << 12)    // Bit 12: Deadtime Enable for n = 2 
#define FTM_COMBINE_SYNCEN1 (1 << 13)  // Bit 13: Synchronization Enable for n = 2 
#define FTM_COMBINE_FAULTEN1 (1 << 14) // Bit 14: Fault Control Enable for n = 2 
                                       // Bit 15: Reserved 
                                       // Bit 15: Reserved 
#define FTM_COMBINE_COMBINE2 (1 << 16) // Bit 16: Combine Channels for n = 4 
#define FTM_COMBINE_COMP2 (1 << 17)    // Bit 17: Complement of Channel (n) for n = 4 
#define FTM_COMBINE_DECAPEN2 (1 << 18) // Bit 18: Dual Edge Capture Mode Enable for n = 4 
#define FTM_COMBINE_DECAP2 (1 << 19)   // Bit 19: Dual Edge Capture Mode Captures for n = 4 
#define FTM_COMBINE_DTEN2 (1 << 20)    // Bit 20: Deadtime Enable for n = 4 
#define FTM_COMBINE_SYNCEN2 (1 << 21)  // Bit 21: Synchronization Enable for n = 4 
#define FTM_COMBINE_FAULTEN2 (1 << 22) // Bit 22: Fault Control Enable for n = 4 
                                       // Bit 23: Reserved 
#define FTM_COMBINE_COMBINE3 (1 << 24) // Bit 24: Combine Channels for n = 6 
#define FTM_COMBINE_COMP3 (1 << 25)    // Bit 25: Complement of Channel (n) for n = 6 
#define FTM_COMBINE_DECAPEN3 (1 << 26) // Bit 26: Dual Edge Capture Mode Enable for n = 6 
#define FTM_COMBINE_DECAP3 (1 << 27)   // Bit 27: Dual Edge Capture Mode Captures for n = 6 
#define FTM_COMBINE_DTEN3 (1 << 28)    // Bit 28: Deadtime Enable for n = 6 
#define FTM_COMBINE_SYNCEN3 (1 << 29)  // Bit 29: Synchronization Enable for n = 6 
#define FTM_COMBINE_FAULTEN3 (1 << 30) // Bit 30: Fault Control Enable for n = 6 
                                       // Bit 31: Reserved 
/* Deadtime Insertion Control */
#define FTM_DEADTIME_DTVAL_SHIFT (0) /* Bits 0-5: Deadtime Value */
#define FTM_DEADTIME_DTVAL (63 << FTM_DEADTIME_DTVAL_SHIFT)
#define FTM_DEADTIME_DTPS_SHIFT (6) /* Bits 6-7: Deadtime Prescaler Value */
#define FTM_DEADTIME_DTPS_MASK (3 << FTM_DEADTIME_DTPS_SHIFT)
# define FTM_DEADTIME_DTPS_DIV1 (0 << FTM_DEADTIME_DTPS_SHIFT)
# define FTM_DEADTIME_DTPS_DIV4 (2 << FTM_DEADTIME_DTPS_SHIFT)
# define FTM_DEADTIME_DTPS_DIV16 (3 << FTM_DEADTIME_DTPS_SHIFT) 

/* Register Bit Definitions */
/* Status and Control */
#define FTM_SC_PS_SHIFT (0) 			// Bits 0-2: Prescale Factor Selection
#define FTM_SC_PS_MASK (7 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_1 (0 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_2 (1 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_4 (2 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_8 (3 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_16 (4 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_32 (5 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_64 (6 << FTM_SC_PS_SHIFT)
#define FTM_SC_PS_128 (7 << FTM_SC_PS_SHIFT)
#define FTM_SC_CLKS_SHIFT (3) 			    // Bits 3-4: Clock Source Selection
#define FTM_SC_CLKS_MASK (3 << FTM_SC_CLKS_SHIFT)
#define FTM_SC_CLKS_NONE (0 << FTM_SC_CLKS_SHIFT)   // No clock selected
#define FTM_SC_CLKS_SYSCLK (1 << FTM_SC_CLKS_SHIFT) // System clock
#define FTM_SC_CLKS_FIXED (2 << FTM_SC_CLKS_SHIFT)  // Fixed frequency clock
#define FTM_SC_CLKS_EXTCLK (3 << FTM_SC_CLKS_SHIFT) // External clock
#define FTM_SC_CPWMS (1 << 5) 			    // Bit 5: Center-aligned PWM Select
#define FTM_SC_TOIE (1 << 6) 			    // Bit 6: Timer Overflow Interrupt Enable
#define FTM_SC_TOF (1 << 7) 			    // Bit 7: Timer Overflow Flag

/* FTM Software Output Control */
#define FTM_SWOCTRL_CH7OC(n) (1 << (n)) // Bits 0-7: Channel (n) Software Output Control Enable
#define FTM_SWOCTRL_CH0OC (1 << 0) // Bit 0: Channel 0 Software Output Control Enable
#define FTM_SWOCTRL_CH1OC (1 << 1) // Bit 1: Channel 1 Software Output Control Enable
#define FTM_SWOCTRL_CH2OC (1 << 2) // Bit 2: Channel 2 Software Output Control Enable
#define FTM_SWOCTRL_CH3OC (1 << 3) // Bit 3: Channel 3 Software Output Control Enable
#define FTM_SWOCTRL_CH4OC (1 << 4) // Bit 4: Channel 4 Software Output Control Enable
#define FTM_SWOCTRL_CH5OC (1 << 5) // Bit 5: Channel 5 Software Output Control Enable
#define FTM_SWOCTRL_CH6OC (1 << 6) // Bit 6: Channel 6 Software Output Control Enable
#define FTM_SWOCTRL_CH7OC (1 << 7) // Bit 7: Channel 7 Software Output Control Enable
#define FTM_SWOCTRL_CHOCV(n) (1 << ((n)+8)) // Bits 8-15: Channel (n) Software Output Control Value
#define FTM_SWOCTRL_CH0OCV (1 << 8) // Bit 8: Channel 0 Software Output Control Value
#define FTM_SWOCTRL_CH1OCV (1 << 9) // Bit 9: Channel 1 Software Output Control Value
#define FTM_SWOCTRL_CH2OCV (1 << 10) // Bit 10: Channel 2 Software Output Control Value
#define FTM_SWOCTRL_CH3OCV (1 << 11) // Bit 11: Channel 3 Software Output Control Value
#define FTM_SWOCTRL_CH4OCV (1 << 12) // Bit 12: Channel 4 Software Output Control Value
#define FTM_SWOCTRL_CH5OCV (1 << 13) // Bit 13: Channel 5 Software Output Control Value
#define FTM_SWOCTRL_CH6OCV (1 << 14) // Bit 14: Channel 6 Software Output Control Value
#define FTM_SWOCTRL_CH7OCV (1 << 15) // Bit 15: Channel 7 Software Output Control Value
                                     // Bits 16-31: Reserved											// Bits 8-31: Reserved 
