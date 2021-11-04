#ifndef __COMPORT_H__
#define __COMPORT_H__
#include <cstdio>
#include <stdio.h>
#include <string>
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#define TRUE 1
#define FALSE -1

class comport
{
private:
    int speed_arr[15] = {
        B115200,
        B38400,
        B19200,
        B9600,
        B4800,
        B2400,
        B1200,
        B300,
        B38400,
        B19200,
        B9600,
        B4800,
        B2400,
        B1200,
        B300,
    };

    int name_arr[15] = {
        115200,
        38400,
        19200,
        9600,
        4800,
        2400,
        1200,
        300,
        38400,
        19200,
        9600,
        4800,
        2400,
        1200,
        300,
    };

    void set_speed(int fd, int speed)
    {
        int i;
        int status;

        struct termios Opt;
        tcgetattr(fd, &Opt);

        for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
        {
            if (speed == name_arr[i])
            {
                tcflush(fd, TCIOFLUSH);
                cfsetispeed(&Opt, speed_arr[i]);
                cfsetospeed(&Opt, speed_arr[i]);
                status = tcsetattr(fd, TCSANOW, &Opt);
                if (status != 0)
                    perror("tcsetattr fd1");

                return;
            }
            tcflush(fd, TCIOFLUSH);
        }
    }

    int set_Parity(int fd, int databits, int stopbits, int parity)
    {
        struct termios options;
        if (tcgetattr(fd, &options) != 0)
        {
            perror("SetupSerial 1");
            return (FALSE);
        }
        options.c_cflag &= ~CSIZE;
        switch (databits)
        {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return (FALSE);
        }

        switch (parity)
        {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB; /* Clear parity enable */
            options.c_iflag &= ~INPCK;  /* Enable parity checking */
            options.c_iflag &= ~(ICRNL | IGNCR);
            options.c_lflag &= ~(ICANON | ECHO);
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK; /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB; /* Enable parity */
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK; /* Disnable parity checking */
            break;

        case 'S':
        case 's': /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;

        default:
            fprintf(stderr, "Unsupported parity\n");
            return (FALSE);
        }

        switch (stopbits)
        {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits\n");
            return (FALSE);
        }

        /* Set input parity option */

        if (parity != 'n')
            options.c_iflag |= INPCK;
        options.c_cc[VTIME] = 150; // 15 seconds
        options.c_cc[VMIN] = 0;

        tcflush(fd, TCIFLUSH); /* Update the options and do it NOW */

        if (tcsetattr(fd, TCSANOW, &options) != 0)
        {
            perror("SetupSerial 3");
            return (FALSE);
        }
        return (TRUE);
    }
    int file;

public:
    comport(const char *__file, int baudrate, int databits, int parity, int stopbits);
    ~comport();

    int comread(uint8_t *read_buffer, uint32_t size);
    int comwrite(uint8_t *write_buffer, uint32_t size);
};

comport::comport(const char *__file, int baudrate, int databits, int parity, int stopbits)
{
    file = open(__file, O_RDWR | O_NOCTTY | O_SYNC);

    set_speed(file, baudrate);
    set_Parity(file, databits, stopbits, parity);

    // Check for errors
    if (file < 0)
    {
        printf("Error %i from open %s \n", errno, __file);
    }
}

comport::~comport()
{
    close(file);
}

int comport::comread(uint8_t *read_buffer, uint32_t size)
{
    return read(file, read_buffer, size);
}

int comport::comwrite(uint8_t *write_buffer, uint32_t size)
{
    return write(file, write_buffer, size);
}

#endif