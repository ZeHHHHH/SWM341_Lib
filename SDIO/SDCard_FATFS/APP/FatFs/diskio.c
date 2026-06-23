/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include <string.h>
#include "SWM341.h"

#define per_sector_bytes       ( 512 ) //512 bytes per sector
#define block_sector_nums      ( 8 )   //block 4KB
#define byte_to_sector(__size) ((__size) >> 9) 
#define sector_to_byte(__size) ((__size) << 9)

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

static DSTATUS disk_stat[FF_VOLUMES];

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    if (pdrv >= sizeof(disk_stat) / sizeof(disk_stat[0])) {
        return STA_NODISK;
    }
    return disk_stat[pdrv];
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();

		// translate the reslut code here
	
		disk_stat[pdrv] |= STA_NOINIT;

		break;

	case DEV_MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here
	
		PORT_Init(PORTM, PIN2, PORTM_PIN2_SD_CLK, 0);
		PORT_Init(PORTM, PIN4, PORTM_PIN4_SD_CMD, 1);
		PORT_Init(PORTM, PIN5, PORTM_PIN5_SD_D0,  1);
		PORT_Init(PORTM, PIN6, PORTM_PIN6_SD_D1,  1);
		PORT_Init(PORTN, PIN0, PORTN_PIN0_SD_D2,  1);
		PORT_Init(PORTN, PIN1, PORTN_PIN1_SD_D3,  1);

		if(SD_RES_OK == SDIO_Init(50000000)) //SD card support Max 50MHz, hight speed need used SDIO_DMABlock Read/Write
		{
			disk_stat[pdrv] = 0;
		}
		else
		{
            disk_stat[pdrv] |= STA_NOINIT;
		}

		break;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here
	
		disk_stat[pdrv] |= STA_NOINIT;

		break;
	}
	
	return disk_status(pdrv);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);

		// translate the reslut code here
	
		res = RES_PARERR;

		return res;

	case DEV_MMC :
    {
        uint32_t result = SD_RES_ERR;
        uint32_t block_buff[block_sector_nums][per_sector_bytes / sizeof(uint32_t)]; /* require align 32bit */
        
        if (0 == ((uint32_t)buff & (4 - 1))) {
            result = SDIO_DMABlockRead(sector, count, (uint32_t *)buff); /* dirty */
            if (SD_RES_OK != result) {
                return RES_ERROR;
            }
        } else {
            for (uint32_t sector_cnt = (block_sector_nums < count) ? block_sector_nums : count; 
                count > 0; 
                sector_cnt = (block_sector_nums < count) ? block_sector_nums : count) 
            {
                result = SDIO_DMABlockRead(sector, sector_cnt, (uint32_t *)&block_buff[0]);
                if (SD_RES_OK != result) {
                    return RES_ERROR;
                }
                memcpy(buff, &block_buff[0], sector_to_byte(sector_cnt));
                buff += sector_to_byte(sector_cnt);
                sector += sector_cnt;
                count -= sector_cnt;
            }
        }
		return RES_OK;
    }
    
	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here
	
		res = RES_PARERR;

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);

		// translate the reslut code here
	
		res = RES_PARERR;

		return res;

	case DEV_MMC :
	{
		uint32_t result = SD_RES_ERR;
        uint32_t block_buff[block_sector_nums][per_sector_bytes / sizeof(uint32_t)]; /* require align 32bit */
        
        if (0 == ((uint32_t)buff & (4 - 1))) {
            result = SDIO_DMABlockWrite(sector, count, (uint32_t *)buff); /* dirty */
            if (SD_RES_OK != result) {
                return RES_ERROR;
            }
        } else {
            for (uint32_t sector_cnt = (block_sector_nums < count) ? block_sector_nums : count; 
                count > 0; 
                sector_cnt = (block_sector_nums < count) ? block_sector_nums : count) 
            {
                memcpy(&block_buff[0], buff, sector_to_byte(sector_cnt));
                result = SDIO_DMABlockWrite(sector, sector_cnt, (uint32_t *)&block_buff[0]);
                if (SD_RES_OK != result) {
                    return RES_ERROR;
                }
                buff += sector_to_byte(sector_cnt);
                sector += sector_cnt;
                count -= sector_cnt;
            }
        }
        return RES_OK;
	}

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here
	
		res = RES_PARERR;

		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch (pdrv) {
#if !FF_FS_READONLY /* Complete pending write process (needed at FF_FS_READONLY == 0) */
    case CTRL_SYNC:
        switch (pdrv) {
            case DEV_RAM :
                //FlashDiskFlush();
            break;
            case DEV_MMC :
                //FlashDiskFlush();
            break;
            case DEV_USB :
                //FlashDiskFlush();
            break;
            default:
                return RES_PARERR;
        }
        return RES_OK;
#endif
#if FF_USE_MKFS /* Get media and erase block size (needed at FF_USE_MKFS == 1) */
    case GET_SECTOR_COUNT: //number of available sectors (DWORD)
        switch (pdrv) {
            case DEV_RAM :
                //*((DWORD *)buff) = ;
                break;
            case DEV_MMC :
                *(DWORD *)buff = byte_to_sector(SD_cardInfo.CardCapacity);
                break;
            case DEV_USB :
                //*((DWORD *)buff) = ;
                break;
            default:
                return RES_PARERR;
        }
        return RES_OK;
    case GET_BLOCK_SIZE: //erase block size in unit of sector (DWORD)
        switch (pdrv) {
            case DEV_RAM :
                //*((DWORD *)buff) = block_sector_nums;
                break;
            case DEV_MMC :
                *(DWORD *)buff = byte_to_sector(SD_cardInfo.CardBlockSize);
            break;
            case DEV_USB :
                //*((DWORD *)buff) = block_sector_nums;
                break;
            default:
                return RES_PARERR;
        }
        return RES_OK;
#endif
#if FF_MAX_SS != FF_MIN_SS /* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
    case GET_SECTOR_SIZE: // Get R/W sector size (WORD)
        switch (pdrv) {
            case DEV_RAM :
                *((WORD *)buff) = per_sector_bytes;
            break;
            case DEV_MMC :
                *(WORD *)buff = per_sector_bytes;
            break;
            case DEV_USB :
                *((WORD *)buff) = per_sector_bytes;
            break;
            default:
                return RES_PARERR;
        }
        return RES_OK;
#endif
#if FF_USE_TRIM /* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */
    case CTRL_TRIM:
        switch (pdrv) {
            case DEV_RAM :
            break;
            case DEV_MMC :
            break;
            case DEV_USB :
            break;
            default:
                return RES_PARERR;
        }
        return RES_OK;
#endif
        default:
            break;
    }
	return RES_PARERR;
}
