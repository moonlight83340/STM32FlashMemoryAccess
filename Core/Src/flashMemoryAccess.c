/*
 * flashMemoryAccess.c
 *
 *  Created on: Sep 29, 2023
 *      Author: perrotg
 */

#include "stm32f4xx_hal.h"
#include "flashMemoryAccess.h"

HAL_StatusTypeDef init_flash_memory(){
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t FirstSector = 0, NbOfSectors = 0;
	uint32_t Address = 0, SECTORError = 0;
	__IO uint32_t data32 = 0 , MemoryProgramStatus = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	/* Erase the user Flash area
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	/* Get the 1st sector to erase */
	FirstSector = GetSector(FLASH_USER_START_ADDR);
	/* Get the number of sector to erase from 1st sector*/
	NbOfSectors = GetSector(FLASH_USER_END_ADDR) - FirstSector + 1;
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK){
		/*
		  Error occurred while sector erase.
		  User can add here some code to deal with this error.
		  SECTORError will contain the faulty sector and then to know the code error on this sector,
		  user can call function 'HAL_FLASH_GetError()'
		*/
		return HAL_ERROR;
	}
	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	 you have to make sure that these data are rewritten before they are accessed during code
	 execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	 DCRST and ICRST bits in the FLASH_CR register. */
	__HAL_FLASH_DATA_CACHE_DISABLE();
	__HAL_FLASH_INSTRUCTION_CACHE_DISABLE();

	__HAL_FLASH_DATA_CACHE_RESET();
	__HAL_FLASH_INSTRUCTION_CACHE_RESET();

	__HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
	__HAL_FLASH_DATA_CACHE_ENABLE();
	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();
	return HAL_OK;
}
/**
  * @brief  write value to an address
  * @param  address the address to write
  * @param  data the data to write
  * @retval The status
  */
uint8_t save_data(uint32_t address,uint32_t data){
	HAL_StatusTypeDef status = HAL_ERROR;
    HAL_FLASH_Unlock();

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
			| FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	FLASH_Erase_Sector(GetSector(address), VOLTAGE_RANGE_3);

	HAL_Delay(50);
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,address,(uint64_t)data);
	HAL_Delay(50);
	HAL_FLASH_Lock();
	return status;
}

/**
  * @brief  read value from an address
  * @param  address the address to read
  * @retval The value readed
  */
uint32_t read_data(uint32_t address){

	__IO uint32_t read_data = *(__IO uint32_t *)address;
	return (uint32_t)read_data;
}

FlashWriteStatus flashWriteData(uint32_t startPageAddress, uint32_t *data,
		uint16_t numberOfWords) {
    if (numberOfWords == 0) {
        return FLASH_WRITE_ERROR_INVALID_COUNT;
    }

    // Verify that startPageAddress is a valid flash address
    if (!isValidFlashAddress(startPageAddress)) {
        return FLASH_WRITE_ERROR_INVALID_ADDRESS;
    }

    HAL_FLASH_Unlock();

    // Erase the flash page
    uint32_t FirstSector = 0, NbOfSectors = 0;
	/* Get the 1st sector to erase */
	FirstSector = GetSector(startPageAddress);
	/* Get the number of sector to erase from 1st sector*/
	NbOfSectors = GetSector(startPageAddress + numberOfWords
			* FLASH_WORD_SIZE)- FirstSector + 1;
	/* Fill EraseInit structure*/
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

    uint32_t sectorError;
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &sectorError) != HAL_OK) {
        HAL_FLASH_Lock();
        return FLASH_WRITE_ERROR_ERASE;
    }

    // Program the flash page word by word
    uint32_t* flashPtr = (uint32_t*)startPageAddress;
    for (int i = 0; i < numberOfWords; i++) {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)flashPtr, data[i]) != HAL_OK) {
            HAL_FLASH_Lock();
            return FLASH_WRITE_ERROR_PROGRAM;
        }
        flashPtr++;
    }

    HAL_FLASH_Lock();

    return FLASH_WRITE_SUCCESS;
}

FlashReadStatus flashReadData(uint32_t startPageAddress, uint32_t *destinationBuffer,
		uint16_t numberOfWords) {
    if (numberOfWords == 0) {
        return FLASH_READ_ERROR_INVALID_COUNT;
    }

    while (numberOfWords > 0) {
        // Vérifiez l'adresse de la page flash
        if (!isValidFlashAddress(startPageAddress)) {
            return FLASH_READ_ERROR_INVALID_ADDRESS;
        }

        *destinationBuffer = *(__IO uint32_t *)startPageAddress;
        startPageAddress += FLASH_WORD_SIZE;
        destinationBuffer++;
        numberOfWords--;
    }

    return FLASH_READ_SUCCESS;
}

int isValidFlashAddress(uint32_t address) {
	return (address >= FLASH_USER_START_ADDR && address <= FLASH_USER_END_ADDR);
}

/**
  * @brief  Gets the sector of a given address
  * @param  None
  * @retval The sector of a given address
  */
uint32_t GetSector(uint32_t Address){
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0)){
    sector = FLASH_SECTOR_0;
  }else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1)) {
    sector = FLASH_SECTOR_1;
  }else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2)) {
    sector = FLASH_SECTOR_2;
  }else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3)) {
    sector = FLASH_SECTOR_3;
  }else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4)) {
    sector = FLASH_SECTOR_4;
  }else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5)) {
    sector = FLASH_SECTOR_5;
  }else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6)) {
    sector = FLASH_SECTOR_6;
  }else {/* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7) */
    sector = FLASH_SECTOR_7;
  }
  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */uint32_t GetSectorSize(uint32_t Sector){
  uint32_t sectorsize = 0x00;

  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1)
		  || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3)) {
    sectorsize = 16 * 1024;
  }else if(Sector == FLASH_SECTOR_4) {
    sectorsize = 64 * 1024;
  }else {
    sectorsize = 128 * 1024;
  }
  return sectorsize;
}
