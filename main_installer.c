/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/** @file
 * @defgroup flashwrite_example_main main.c
 * @{
 * @ingroup flashwrite_example
 *
 * @brief This file contains the source code for a sample application using the Flash Write Application.
 *
 *
 * @image html example_board_setup_a.jpg "Use board setup A for this example."
 */

#include <stdbool.h>
#include "nrf.h"
#include "nrf_gpio.h"

/** @brief Function for erasing a page in flash.
 *
 * @param page_address Address of the first word in the page to be erased.
 */
static void flash_page_erase(uint32_t *page_address)
{
  // Turn on flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos);
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    // Erase page:
    NRF_NVMC->ERASEPAGE = (uint32_t)page_address;
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
    
    // Turn off flash erase enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}


/** @brief Function for filling a page in flash with a value.
 *
 * @param[in] address Address of the first word in the page to be filled.
 * @param[in] value Value to be written to flash.
 */
static void flash_word_write(uint32_t *address, uint32_t value)
{
    // Turn on flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos);
    
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
  
    *address = value;
  
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
  
    // Turn off flash write enable and wait until the NVMC is ready:
    NRF_NVMC->CONFIG = (NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos);
  
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
    {
        // Do nothing.
    }
}

void clone_data(uint32_t *src, uint32_t *dest, uint32_t length) {
    uint32_t pg_size = NRF_FICR->CODEPAGESIZE;
	  uint32_t *clean_address = dest; 
	  uint32_t dest_end_address = (uint32_t)dest + length; 
	
	  // clear pages
		while ((uint32_t)clean_address < dest_end_address) {
		    flash_page_erase(clean_address);
			  clean_address += pg_size;
		}
		
		// clone data	
	  uint32_t *read_address = src; 	
	  uint32_t *write_address = dest; 	
		uint32_t data;
		while ((uint32_t)write_address < dest_end_address) {
			  data = (uint32_t)*read_address++;
			  flash_word_write(write_address++, (uint32_t)data);
		}
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t *sd_dest_addr = (uint32_t *)0;
    uint32_t *bl_dest_addr = (uint32_t *)0x3C000;
    uint32_t *sd_src_addr = (uint32_t *)0x19000;
	  uint32_t sd_size = 0x1D000;
    uint32_t *bl_src_addr = (uint32_t *)0x15000;
	  uint32_t bl_size = 0x4000;

  	// copy bootloader
  	clone_data(bl_src_addr, bl_dest_addr, bl_size);
	
  	// copy softdevice
  	clone_data(sd_src_addr, sd_dest_addr, sd_size);
	
	  // set new bootloader address
		uint32_t *uicr_bl = (uint32_t *)0x10001014;
	  flash_word_write(uicr_bl, (uint32_t)bl_dest_addr);
	
	  NVIC_SystemReset();
}


/** @} */