#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <eapi.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <common.h>
#include "conv.h"

#define SMC_FLASH_ALIGNMENT 4

#define EAPI_STOR_LOCK       _IOWR('a', 1, unsigned long)
#define EAPI_STOR_UNLOCK       _IOWR('a', 2, unsigned long)
#define EAPI_STOR_REGION      _IOWR('a', 3, unsigned long)

struct secure {
        uint8_t Region;
        uint8_t permission;
        char passcode[32];
};
static char NVMEM_DEVICE[285];
//static char NVMEM_SEC_DEVICE[285];

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

#define NVMEM_INIT() if(initialize_nvmem() < 0) return EAPI_STATUS_UNSUPPORTED;

static int initialize_nvmem_sec()
{
        struct dirent *de;
        DIR *dr = opendir("/sys/bus/platform/devices/adl-bmc-nvmem-sec");

        if (dr == NULL)  // opendir returns NULL if couldn't open directory 
                return -1;
        memset(NVMEM_DEVICE, 0, sizeof(NVMEM_DEVICE));
        while ((de = readdir(dr)) != NULL) {
                if(strncmp(de->d_name, "nvmem-sec", strlen("nvmem-sec")) == 0) {
                       
		       	sprintf(NVMEM_DEVICE, "/sys/bus/nvmem/devices/%s/nvmem", de->d_name);
                       
		       	closedir(dr);
                        return 0;
                }
        }
        closedir(dr);

        return -1;
}

#define NVMEM_SEC_INIT() if(initialize_nvmem_sec() < 0) return EAPI_STATUS_UNSUPPORTED;

uint32_t EApiStorageCap(uint32_t Id, uint32_t *pStorageSize, uint32_t *pBlockLength)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	char sysfile[256];
	char buf[128];

        if(pStorageSize == NULL && pBlockLength == NULL){
                return EAPI_STATUS_INVALID_PARAMETER;
        }

	if(Id > 1 && Id < 0)
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

uint32_t EApiStorageAreaRead(uint32_t Id,uint32_t Region, uint32_t Offset, void *pBuffer, uint32_t BufLen, uint32_t  Bytecnt)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	int ret = 0;
	int fd;
	uint32_t BytecntTemp=Bytecnt;
        uint8_t pBufferTemp[(Bytecnt%4)?Bytecnt+4-(Bytecnt%4):Bytecnt];
	struct secure data;       
	        
	if(Region==1)
	{
		NVMEM_INIT();
	}
	else if(Region==2)
	{
		NVMEM_SEC_INIT();
	}
	else if(Region==3)
	{
		NVMEM_SEC_INIT();
	}
	else
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if(Id > 1 && Id < 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	if (pBuffer == NULL || Bytecnt == 0 || BufLen == 0)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	if (Bytecnt > BufLen)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	data.Region=Region;
	
        if((fd = open("/dev/bmc-nvmem-eapi", O_RDWR)) < 0)
	{
	
		return -1;
	}
	if(ioctl(fd, EAPI_STOR_REGION, &data ) < 0)
        {
        	close(fd);
		return EAPI_STATUS_UNSUPPORTED;
	}
	else
	close(fd);
		
	fd = open(NVMEM_DEVICE, O_RDONLY);
	
	if (fd < 0)
	{
		return EAPI_STATUS_READ_ERROR;
	}

	
	lseek(fd,Offset,SEEK_SET);
	
	if(Bytecnt % 4 != 0)
	{
		Bytecnt += 4 - (Bytecnt % 4);
	}
	pBufferTemp[sizeof(pBufferTemp)]='\0';
	
	ret = read(fd, pBufferTemp, Bytecnt);
	
	memcpy(pBuffer, pBufferTemp, BytecntTemp);
	
	if (ret > 0)
		close(fd);
	else {
		close(fd);
		return EAPI_STATUS_READ_ERROR;
	}
	
	return status;
}

uint32_t EApiStorageAreaWrite(uint32_t Id,uint32_t Region, uint32_t Offset, char* Buf, uint32_t Len)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	int ret,fd;
	struct secure data;       
	
	if(Region==1)
	{
		NVMEM_INIT();
	}
	else if(Region==2)
	{
		NVMEM_SEC_INIT();
	}
	else if(Region==3)
	{
		NVMEM_SEC_INIT();
	}
	else
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if(Id > 1 && Id < 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	if(Offset + Len > 2048)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	unsigned char *buffer;
	if((Len % 4) != 0)
	{
		int rem = 4 - (Len % 4);
		Len = Len + rem;
		buffer = calloc(Len, sizeof(char));
		if(buffer == NULL)
		{
			return EAPI_STATUS_READ_ERROR;
		}
		memcpy(buffer, Buf, Len);
		EApiStorageAreaRead(Id,Region, Offset + (Len - rem), buffer + (Len - rem), rem, rem);
	}
	else
	{
		buffer = (unsigned char*)Buf;
	}

	data.Region=Region;
	
        if((fd = open("/dev/bmc-nvmem-eapi", O_RDWR)) < 0)
	{
		return -1;
	}
	if(ioctl(fd, EAPI_STOR_REGION, &data ) < 0)
        {
        	close(fd);
		return EAPI_STATUS_UNSUPPORTED;
	}
	else
	{
		close(fd);
	}
	fd = open(NVMEM_DEVICE, O_WRONLY);
        
	if (fd < 0)
	{
                return EAPI_STATUS_WRITE_ERROR;
	}

	lseek(fd,Offset,SEEK_SET);
	ret = write(fd,buffer,Len);
	if (ret > 0)
	{
		close(fd);
	}
	else
	{
		close(fd);
		return EAPI_STATUS_WRITE_ERROR;
	}

	return status;
}

uint32_t EApiStorageHexRead(uint32_t Id,uint32_t Region, uint32_t Offset, void *pBuffer, uint32_t BufLen, uint32_t  Bytecnt)
{
        int ret = 0;
	ret = EApiStorageAreaRead(Id,Region,Offset,pBuffer,BufLen,Bytecnt);
	
	if(ret)
		return ret;
	else
		return EAPI_STATUS_SUCCESS;
}

uint32_t EApiStorageHexWrite(uint32_t Id,uint32_t Region, uint32_t Offset, char* Buf, uint32_t Len)
{
        uint32_t status = EAPI_STATUS_SUCCESS;
        int ret,i,fd;
	struct secure data;
	char *asciiString,*hex_buf;
        char result[1028];

	for(i=0;i<Len*2;i++)
	{
		if(isxdigit(Buf[i])==0)
		{
			printf("Please provide the hex data only 'A'-'F','a'-'f' and '0'-'9'\n");
			return EAPI_STATUS_UNSUPPORTED;
		}
	}
	asciiString = Buf;
	Conv_String2HexByte(asciiString,result);
	hex_buf = result;

        if(Region==1)
        {
                NVMEM_INIT();
        }
        else if(Region==2)
        {
                NVMEM_SEC_INIT();
        }
        else if(Region==3)
        {
                NVMEM_SEC_INIT();
        }
        else
        {
                return EAPI_STATUS_INVALID_PARAMETER;
        }

        if(Id > 1 && Id < 0)
        {
                return EAPI_STATUS_UNSUPPORTED;
        }
        if(Offset + Len > 2048)
        {
                return EAPI_STATUS_MORE_DATA;
        }

        unsigned char *buffer;
	
	buffer = (unsigned char*)hex_buf;

        data.Region=Region;

        if((fd = open("/dev/bmc-nvmem-eapi", O_RDWR)) < 0)
        {
                return -1;
        }
        if(ioctl(fd, EAPI_STOR_REGION, &data ) < 0)
        {
                close(fd);
                return EAPI_STATUS_UNSUPPORTED;
        }
        else
        {
                close(fd);
        }
	
	fd = open(NVMEM_DEVICE, O_WRONLY);

        if (fd < 0)
        {
                return EAPI_STATUS_WRITE_ERROR;
        }

        lseek(fd,Offset,SEEK_SET);
        ret = write(fd,buffer,Len);
        if (ret > 0)
        {
                close(fd);
        }
        else
        {
                close(fd);
                return EAPI_STATUS_WRITE_ERROR;
        }

        return status;
}

uint32_t EApiStorageAreaClear(uint32_t Id,uint32_t Region)
{
	return EAPI_STATUS_UNSUPPORTED;
}

uint32_t EApiStorageLock(uint32_t Id,uint32_t Region)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	int fd;
	struct secure data;
	if(Region!=2 && Region!=3 )
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	data.Region=Region;

        if((fd = open("/dev/bmc-nvmem-eapi", O_RDWR)) < 0)
	{
		return -1;
	}
	if(ioctl(fd, EAPI_STOR_LOCK, &data ) < 0)
        {
		return EAPI_STATUS_UNSUPPORTED;
	}

	return status;
}

uint32_t EApiStorageUnLock(uint32_t Id,uint32_t Region,uint32_t Permission, char *passcode)
{
	uint32_t status = EAPI_STATUS_SUCCESS;
	int fd;
        struct secure data;       
	if(Region!=2 && Region!=3 )
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	data.Region=Region;
	data.permission=Permission;

	memcpy(data.passcode,passcode,strlen(passcode));

        if((fd = open("/dev/bmc-nvmem-eapi", O_RDWR)) < 0)
        {
                return -1;
        }

        if(ioctl(fd, EAPI_STOR_UNLOCK,&data ) < 0)
        {
                return EAPI_STATUS_UNSUPPORTED;
        }

        return status;
}
