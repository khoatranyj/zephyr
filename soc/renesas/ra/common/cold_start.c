#include <zephyr/device.h>
#include <bsp_api.h>

volatile uint8_t rstsr0_state_at_boot;
#if defined(CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE)
extern volatile uint8_t vbtbpsr_state_at_boot;
#endif /* CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE */


#if defined(CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE)
extern void battery_backup_init(void);
#endif /* CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE */

void cold_start_init(void)
{
	/* Detect power on reset */
	rstsr0_state_at_boot = R_SYSTEM->RSTSR0;
#if defined(CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE)
	vbtbpsr_state_at_boot = R_SYSTEM->VBTBPSR;
#endif /* CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE */
	if (R_SYSTEM->RSTSR0_b.PORF == 1) {
#if defined(CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE)
		battery_backup_init();
#endif /* CONFIG_RENESAS_RA_BATTERY_BACKUP_MANUAL_CONFIGURE */
	}
}
