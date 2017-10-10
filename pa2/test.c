#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main() {
	
	int file = open("/dev/simple_char_driver", O_RDWR);
	char userInput;
	int userBytes;

	while (1){
	    printf("\n\nPress r to read from device\n"
	    "Press w to write to the device\n"
	    "Press s to seek into the device\n"
	    "Press e to exit from the device\n"
	    "Press anything else to keep reading or writing from the device\n"
	    "Enter command: ");
	    userInput = getchar();
	    switch (userInput){
		case 'r':
		case 'R':
		{
		    printf("Enter the number of bytes you want to read: ");

		    scanf("%d", &userBytes);
		    if (userBytes < 0)
		    {
			printf("Enter positive numbers.");
			while (getchar() != '\n');
			break;
		    }
		    char *bufferSize = (char*)malloc(userBytes);
		    read(file, bufferSize, userBytes);
		    printf("Data read from the device: %s\n", bufferSize);
		    while (getchar() != '\n');
		    break;
		}
		case 'w':
		case 'W':
		{
		    char userData[1024];
		    
		    printf("Enter data you want to write to the device: ");
		    scanf("%s", userData);
		    userBytes = sizeof(userData);
		    write(file, userData, userBytes);
		    while (getchar() != '\n');
		    break;
		}
		case 's':
		case 'S':
		{
		    int whence;
		    int offset;
		    printf("Enter whence value: \n");
		    scanf("%d", &whence);
		    printf("Enter offset value: \n");
		    scanf("%d", &offset);
		    lseek(file, offset, whence);
		    while (getchar() != '\n');
		    break;
		}
		case 'e':
		case 'E':
		{
		    printf("Goodbye!\n");
		    return 0;
		}
		default:
		    while (getchar() != '\n');
	    }
	}
	close("/dev/simple_char_driver");
	return 0;
}