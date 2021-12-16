//
//   Program to convert byte values of opt3001 to lux value
//       To Compile: gcc -o ambval ambval.cpp
//

//  CMD> ./ambval -h 0x15 -l 0x15
//  ./ambval: Hi 0x15 Lo 0x15 rv 0x1515
//  ./ambval: ma 1301 exp 0001
//  ./ambval: Calculated lux value is 26.020000.

//  CMD> ./ambval -h 15 -l 15
//  ./ambval: Hi 0x0f Lo 0x0f rv 0x0f0f
//  ./ambval: ma 3855 exp 0000
//  ./ambval: Calculated lux value is 38.550000.

#define TI_OPT3007_BUSADDRESS 0x44
#define RASPBERRY_PI

#ifdef RASPBERRY_PI
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

static char   		*myName  = NULL;
static double 		luxVal   = 0.0;
static int    		hiByte   = -1;
static int    		loByte   = -1;

//read ambient light value based on ti_opt3007_I2C_read
static uint16_t ambRead (char address, int *hi, int *lo)
{
	// I2C read operation and return uint16_t value as per datasheet
	// Data should be in first byte received from I2C denotes 15:8 and 2nd byte from I2C denotes 7:0

#ifdef RASPBERRY_PI
    char *fileName = (char*)"/dev/i2c-1";
    int fileI2C;
    uint8_t buffer[2];

    if ((fileI2C = open(fileName, O_RDWR)) < 0)
	{
        printf("ERROR:Not able to perform I2C operation\n");
		return -1;
	}

    if (ioctl(fileI2C, I2C_SLAVE, address) < 0)
	{
        printf("ERROR:Unable to set I2C address\n");
		return -1;
	}

    if (write(fileI2C, &address, 1) != 1)
	{
        printf("ERROR:Write operation to set read register address failed\n");
		return -1;
	}

    if (read(fileI2C, buffer, 2) != 2)
	{
        printf("ERROR:Read operation failed\n");
		return -1;
	}

	*lo = buffer[1];
	*hi = buffer[0];
	
    uint16_t returnValue = buffer[1] | (((uint16_t)buffer[0])<<8);

    printf("INFO:I2C Read operation Value from register 0x%02x is 0x%02x\n",
    	address,returnValue);
#endif
	return 1;
}

static void showUsage ()
{
    printf ("Usage: %s -h <High Byte> -l <Low Byte> -r [read from I2C]\n", myName);
    exit (1);
}

static int parseOptions (int argc, char *argv[])
{
	int rspec = 0;
	int opt;

    myName = argv[0];

    // put ':' in the starting of the string so that program can  
    // distinguish between '?' and ':'
    while ((opt = getopt(argc, argv, ":h:l:r")) != -1)  
    {  
        switch (opt)  
        {  
            case 'h':  
                hiByte = strtol (optarg, NULL, 0);  
                break;
            case 'l':  
                loByte = strtol (optarg, NULL, 0);  
                break;
            case ':':  
                printf("option needs a value\n");  
                break;  
            case '?':  
                printf("unknown option: %c\n", optopt);
                break;
			case 'r':				
				rspec = ambRead (TI_OPT3007_BUSADDRESS, &hiByte, &loByte);
				break;
        }
    }
	
	if (rspec == -1)
	{
		printf("OPERATION FAILED");
		exit(0);
	}
	
	return rspec;
}

double getAmb (int hi, int lo)
{
    uint16_t readValue = (hi << 8) | (lo & 0xff);
    printf ("%s: Hi 0x%02x Lo 0x%02x rv 0x%04x\n", 
                  myName, hi, lo, readValue);

    unsigned int mantissa = readValue & 0x0fff ;
    unsigned int exp      = readValue >> 12;
    printf ("%s: ma %04d exp %04d\n", myName, mantissa, exp);

    double val = 10e-3 * (((uint16_t) 1)<<exp) * mantissa ;
    return val;
}

int main (int argc, char *argv[])
{
    parseOptions (argc, argv);
    if ((hiByte == -1) || (loByte == -1))
    {
        showUsage ();
        return -1;
    }
    printf ("%s: Calculated lux value is %f.\n",
    		myName, getAmb(hiByte, loByte));
}
