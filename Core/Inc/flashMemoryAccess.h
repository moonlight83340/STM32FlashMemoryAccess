/*
 * flashMemoryAccess.h
 *
 *  Created on: Sep 29, 2023
 *      Author: perrotg
 */

#ifndef INC_FLASHMEMORYACCESS_H_
#define INC_FLASHMEMORYACCESS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes */

#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_2   /* Start @ of user Flash area */
/* End @ of user Flash area : sector start address + sector size -1 */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_7  +  GetSectorSize(ADDR_FLASH_SECTOR_7) -1

#define FLASH_WORD_SIZE 4

typedef enum {
    FLASH_READ_SUCCESS,
    FLASH_READ_ERROR_INVALID_ADDRESS,
    FLASH_READ_ERROR_INVALID_COUNT,
} FlashReadStatus;

typedef enum {
    FLASH_WRITE_SUCCESS,
    FLASH_WRITE_ERROR_INVALID_ADDRESS,
    FLASH_WRITE_ERROR_INVALID_COUNT,
    FLASH_WRITE_ERROR_ERASE,
    FLASH_WRITE_ERROR_PROGRAM,
} FlashWriteStatus;

HAL_StatusTypeDef init_flash_memory();

uint8_t save_data(uint32_t address,uint32_t data);
uint32_t read_data(uint32_t address);

FlashWriteStatus flashWriteData(uint32_t startPageAddress, uint32_t *data,
		uint16_t numberOfWords);
FlashReadStatus flashReadData(uint32_t startPageAddress, uint32_t *destinationBuffer,
		uint16_t numberOfWords);

int isValidFlashAddress(uint32_t address);
uint32_t GetSector(uint32_t Address);
uint32_t GetSectorSize(uint32_t Sector);

#ifdef __cplusplus
}
#endif

#endif /* INC_FLASHMEMORYACCESS_H_ */
