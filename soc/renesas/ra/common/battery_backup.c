#include <zephyr/device.h>
#include <bsp_api.h>

#define VCC_DROP_DETECTION_STABILIZATION_WAIT_TIME_US 20
#define VDETELVL_NOT_USED                             0x6
#define BATTERY_BACKUP_CONFIGURATION_NOT_USED                                                      \
	DT_ENUM_HAS_VALUE(DT_NODELABEL(battery_backup), switch_threshold, 6)

volatile uint8_t vbtbpsr_state_at_boot;

void battery_backup_init(void)
{
#if BATTERY_BACKUP_CONFIGURATION_NOT_USED
	/*  Set the BPWSWSTP bit to 1. The power supply switch is stopped */
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_OM_LPC_BATT);
	R_SYSTEM->VBTBPCR1_b.BPWSWSTP = 1;

	/* Check VBPORM flag. If VBPORM flag is 0, wait until it changes to 1 */
	while (R_SYSTEM->VBTBPSR_b.VBPORM == 0) {
	}

	vbtbpsr_state_at_boot = R_SYSTEM->VBTBPSR;
	R_SYSTEM->VBTBPSR_b.VBPORF = 0;
	R_SYSTEM->VBTBPCR2_b.VDETE = 0;
	R_SYSTEM->VBTBPCR2_b.VDETLVL = VDETELVL_NOT_USED;
	R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_OM_LPC_BATT);

	/* Set the SOSTP bit to 1 regardless of its value. Stop Sub-Clock Oscillator */
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_CGC);
	R_SYSTEM->SOSCCR_b.SOSTP = 1;
	R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_CGC);
#else
	/*  Check VBPORM bit. If VBPORM flag is 0, wait until it changes to 1 */
	while (R_SYSTEM->VBTBPSR_b.VBPORM == 0) {
	}
	vbtbpsr_state_at_boot = R_SYSTEM->VBTBPSR;
	if (R_SYSTEM->VBTBPSR_b.VBPORF == 1) {
		R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_OM_LPC_BATT);
		R_SYSTEM->VBTBPSR_b.VBPORF = 0;
		R_SYSTEM->VBTBPCR2_b.VDETLVL =
			DT_PROP(DT_NODELABEL(battery_backup), switch_threshold);
		R_BSP_SoftwareDelay(VCC_DROP_DETECTION_STABILIZATION_WAIT_TIME_US,
				    BSP_DELAY_UNITS_MICROSECONDS);
		R_SYSTEM->VBTBPCR2_b.VDETE = 1;
		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_OM_LPC_BATT);
	}
#endif /* BATTERY_BACKUP_CONFIGURATION_NOT_USED */
}
