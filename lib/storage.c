#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <eapi.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include <common.h>

#define SMC_FLASH_ALIGNMENT 4

static char NVMEM_DEVICE[285];

static int initialize_nvmem()
{
	struct dirent *de;
        DIR *dr = opendir("/sys/bus/platform/devices/adl-bmc-nvmem");

        if (dr == NULL)  // opendir returns NULL if couldn't open directory 
                return -1;
        
	memset(NVMEM_DEVICE, 0, sizeof(NVMEM_DEVICE));
        while ((de = readdir(dr)) != NULL) {
                if(strncmp(de->d_name, "nvmem", strlen("nvmem")) == 0) {
                        sprintf(NVMEM_DEVICE, "/sys/bus/nvmem/devices/%s/nvmem", de->d_name);
                        closedir(dr);
                        return 0;
                }
        }
        closedir(dr);

        return -1;
}

#define NVMEM_INIT() if(initialize_nvmem() < 0) return -1;



uint32_t EApiStorageCap(uint32_t Id, uint32_t *pStorageSize, uint32_t *pBlockLength)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	char sysfile[256];
	char buf[128];

        if(pStorageSize == NULL && pBlockLength == NULL){
                return EAPI_STATUS_INVALID_PARAMETER;
        }

	if (Id != EAPI_ID_STORAGE_STD)
		return EAPI_STATUS_UNSUPPORTED;

	sprintf(sysfile, "/sys/bus/platform/devices/adl-bmc-nvmem/capabilities/nvmemcap");

        status = read_sysfs_file(sysfile, buf, sizeof(buf));
	if (status)
		return EAPI_STATUS_READ_ERROR;
 
	char* token = strtok(buf, " ");
        if (strstr(token, "StorageSize")){
	       	token = strtok(NULL, " ");
	}

	*pStorageSize = atoi(token);

	token = strtok(NULL, " ");
        if (strstr(token, "\nBlockLength")){
	       	token = strtok(NULL, " ");
	}

	*pBlockLength = atoi(token);

	return status;
}

uint32_t EApiStorageAreaRead(uint32_t Id, uint32_t Offset, void *pBuffer, uint32_t BufLen, uint32_t  Bytecnt)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	int ret = 0;
	NVMEM_INIT();
	int fd;
       	uint32_t i;
	if (Id != EAPI_ID_STORAGE_STD)
		return EAPI_STATUS_UNSUPPORTED;

	if (pBuffer == NULL || Bytecnt == 0 || BufLen == 0)
		return EAPI_STATUS_INVALID_PARAMETER;

	if (Bytecnt > BufLen)
		return -1;
	fd = open(NVMEM_DEVICE, O_RDONLY);
	if (fd < 0)
		return -1;

	ret = read(fd, pBuffer, Offset + Bytecnt);
	for(i = Offset;  i < Offset + Bytecnt; i++)
		((char*)pBuffer)[i - Offset] = ((char*)pBuffer)[i];
	((char*)pBuffer)[i - Offset] = 0;
	if (ret)
		close(fd);
	else {
		close(fd);
		return -1;
	}
	
	return status;
}

uint32_t EApiStorageAreaWrite(uint32_t Id, unsigned short Offset, char* Buf, unsigned short Len)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	int ret;
	uint32_t pBlockLength = SMC_FLASH_ALIGNMENT;
	NVMEM_INIT();

	if (Id != EAPI_ID_STORAGE_STD)
		return EAPI_STATUS_UNSUPPORTED;

	if ((Offset % pBlockLength) != 0)
		return EAPI_STATUS_INVALID_BLOCK_ALIGNMENT;

	ret = write_sysfs_file(NVMEM_DEVICE, Buf, Len);
	if (ret)
		return EAPI_STATUS_WRITE_ERROR;

	return status;
}
