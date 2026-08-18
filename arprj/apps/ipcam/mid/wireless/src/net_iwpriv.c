#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>		/* gethostbyname, getnetbyname */
#include <net/ethernet.h>	/* struct ether_addr */
#include <sys/time.h>		/* struct timeval */
#include <unistd.h>

#include <net/if_arp.h>		/* For ARPHRD_ETHER */
#include <sys/socket.h>		/* For AF_INET & struct sockaddr */
#include <netinet/in.h>         /* For struct sockaddr_in */
#include <netinet/if_ether.h>

#include <linux/types.h>		/* for "caddr_t" et al		*/
#include <sys/socket.h>			/* for "struct sockaddr" et al	*/
#ifndef CONFIG_BIT_32BITS
#include <net/if.h>
#endif
#include "net_iwpriv.h"

#if 0
static int iw_sockets_open(void)
{
    static const int families[] = {
        AF_INET, AF_IPX, AF_AX25, AF_APPLETALK
    };
    unsigned int	i;
    int		sock;

    /*
    * Now pick any (exisiting) useful socket family for generic queries
    * Note : don't open all the socket, only returns when one matches,
    * all protocols might not be valid.
    * Workaround by Jim Kaba <jkaba@sarnoff.com>
    * Note : in 99% of the case, we will just open the inet_sock.
    * The remaining 1% case are not fully correct...
    */

    /* Try all families we support */
    for(i = 0; i < sizeof(families)/sizeof(int); ++i)
    {
        /* Try to open the socket, if success returns it */
        sock = socket(families[i], SOCK_DGRAM, 0);
        if(sock >= 0)
            return sock;
    }

    return -1;
}


static inline void iw_sockets_close(int	skfd)
{
    close(skfd);
}

static inline int
iw_get_ext(int			skfd,		/* Socket to the kernel */
	   const char *		ifname,		/* Device name */
	   int			request,	/* WE ID */
	   struct iwreq *	pwrq)		/* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}

static const int priv_type_size[] = {
	0,				/* IW_PRIV_TYPE_NONE */
	1,				/* IW_PRIV_TYPE_BYTE */
	1,				/* IW_PRIV_TYPE_CHAR */
	0,				/* Not defined */
	sizeof(__u32),			/* IW_PRIV_TYPE_INT */
	sizeof(struct iw_freq),		/* IW_PRIV_TYPE_FLOAT */
	sizeof(struct sockaddr),	/* IW_PRIV_TYPE_ADDR */
	0,				/* Not defined */
};


int
iw_get_priv_size(int	args)
{
  int	num = args & IW_PRIV_SIZE_MASK;
  int	type = (args & IW_PRIV_TYPE_MASK) >> 12;

  return(num * priv_type_size[type]);
}


int
iw_get_priv_info(int		skfd,
		 const char *	ifname,
		 iwprivargs **	ppriv)
{
    struct iwreq		wrq;
    iwprivargs *		priv = NULL;	/* Not allocated yet */
    int			maxpriv = 16;	/* Minimum for compatibility WE<13 */
    iwprivargs *		newpriv;

    /* Some driver may return a very large number of ioctls. Some
    * others a very small number. We now use a dynamic allocation
    * of the array to satisfy everybody. Of course, as we don't know
    * in advance the size of the array, we try various increasing
    * sizes. Jean II */
    do
    {
        /* (Re)allocate the buffer */
        newpriv = realloc(priv, maxpriv * sizeof(priv[0]));
        if(newpriv == NULL)
        {
            fprintf(stderr, "%s: Allocation failed\n", __FUNCTION__);
            break;
        }
        priv = newpriv;

        /* Ask the driver if it's large enough */
        wrq.u.data.pointer = (caddr_t) priv;
        wrq.u.data.length = maxpriv;
        wrq.u.data.flags = 0;
        if(iw_get_ext(skfd, ifname, SIOCGIWPRIV, &wrq) >= 0)
        {
            /* Success. Pass the buffer by pointer */
            *ppriv = priv;
            /* Return the number of ioctls */
            return(wrq.u.data.length);
        }

        /* Only E2BIG means the buffer was too small, abort on other errors */
        if(errno != E2BIG)
        {
            /* Most likely "not supported". Don't barf. */
            break;
        }

        /* Failed. We probably need a bigger buffer. Check if the kernel
        * gave us any hints. */
        if(wrq.u.data.length > maxpriv)
            maxpriv = wrq.u.data.length;
        else
            maxpriv *= 2;
    }
    while(maxpriv < 1000);

    /* Cleanup */
    if(priv)
        free(priv);
    *ppriv = NULL;

    return(-1);
}

static int
set_private_cmd(int		skfd,		/* Socket */
		char *		args[],		/* Command line args */
		int		count,		/* Args count */
		char *		ifname,		/* Dev name */
		char *		cmdname,	/* Command name */
		iwprivargs *	priv,		/* Private ioctl description */
		int		priv_num, /* Number of descriptions */
		u_char	*buffer)
{
    struct iwreq	wrq;
    ///u_char	buffer[IWPRIV_GET_DATA_SIZE];	/* Only that big in v25 and later */
    int		i = 0;		/* Start with first command arg */
    int		k;		/* Index in private description table */
    int		temp;
    int		subcmd = 0;	/* sub-ioctl index */
    int		offset = 0;	/* Space for sub-ioctl index */

    /* Check if we have a token index.
    * Do it now so that sub-ioctl takes precedence, and so that we
    * don't have to bother with it later on... */
    if((count >= 1) && (sscanf(args[0], "[%i]", &temp) == 1))
    {
        subcmd = temp;
        args++;
        count--;
    }

    /* Search the correct ioctl */
    k = -1;
    while((++k < priv_num) && strcmp(priv[k].name, cmdname));

    /* If not found... */
    if(k == priv_num)
    {
        fprintf(stderr, "Invalid command : %s\n", cmdname);
        return(-1);
    }

    /* Watch out for sub-ioctls ! */
    if(priv[k].cmd < SIOCDEVPRIVATE)
    {
        int	j = -1;

        /* Find the matching *real* ioctl */
        while((++j < priv_num) && ((priv[j].name[0] != '\0') ||
        (priv[j].set_args != priv[k].set_args) ||
            (priv[j].get_args != priv[k].get_args)));

        /* If not found... */
        if(j == priv_num)
        {
            fprintf(stderr, "Invalid private ioctl definition for : %s\n",
            cmdname);
            return(-1);
        }

        /* Save sub-ioctl number */
        subcmd = priv[k].cmd;
        /* Reserve one int (simplify alignment issues) */
        offset = sizeof(__u32);
        /* Use real ioctl definition from now on */
        k = j;

#if 0
        printf("<mapping sub-ioctl %s to cmd 0x%X-%d>\n", cmdname,
        priv[k].cmd, subcmd);
#endif
    }

    /* If we have to set some data */
    if((priv[k].set_args & IW_PRIV_TYPE_MASK) &&
    (priv[k].set_args & IW_PRIV_SIZE_MASK))
    {
        switch(priv[k].set_args & IW_PRIV_TYPE_MASK)
        {

            case IW_PRIV_TYPE_BYTE:
                /* Number of args to fetch */
                wrq.u.data.length = count;
                if(wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
                    wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;

                /* Fetch args */
                for(; i < wrq.u.data.length; i++) {
                    sscanf(args[i], "%i", &temp);
                    buffer[i] = (char) temp;
                }
                break;

            case IW_PRIV_TYPE_INT:
                /* Number of args to fetch */
                wrq.u.data.length = count;
                if(wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
                wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;

                /* Fetch args */
                for(; i < wrq.u.data.length; i++) {
                    sscanf(args[i], "%i", &temp);
                    ((__s32 *) buffer)[i] = (__s32) temp;
                }
                break;

            case IW_PRIV_TYPE_CHAR:
                if(i < count)
                {
                    /* Size of the string to fetch */
                    wrq.u.data.length = strlen(args[i]) + 1;
                    if(wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
                        wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;

                    /* Fetch string */
                    memcpy(buffer, args[i], wrq.u.data.length);
                    //buffer[sizeof(buffer) - 1] = '\0';
                    buffer[IWPRIV_GET_DATA_SIZE - 1] = '\0';
                    i++;
                }
                else
                {
                    wrq.u.data.length = 1;
                    buffer[0] = '\0';
                }
                break;

            #if 0
            case IW_PRIV_TYPE_FLOAT:
                /* Number of args to fetch */
                wrq.u.data.length = count;
                if(wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
                wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;

                /* Fetch args */
                for(; i < wrq.u.data.length; i++) {
                    double		freq;
                    if(sscanf(args[i], "%lg", &(freq)) != 1)
                    {
                        printf("Invalid float [%s]...\n", args[i]);
                        return(-1);
                    }
                    if(strchr(args[i], 'G')) freq *= GIGA;
                    if(strchr(args[i], 'M')) freq *= MEGA;
                    if(strchr(args[i], 'k')) freq *= KILO;
                    sscanf(args[i], "%i", &temp);
                    iw_float2freq(freq, ((struct iw_freq *) buffer) + i);
                }
                break;

            case IW_PRIV_TYPE_ADDR:
                /* Number of args to fetch */
                wrq.u.data.length = count;
                if(wrq.u.data.length > (priv[k].set_args & IW_PRIV_SIZE_MASK))
                wrq.u.data.length = priv[k].set_args & IW_PRIV_SIZE_MASK;

                /* Fetch args */
                for(; i < wrq.u.data.length; i++) {
                    if(iw_in_addr(skfd, ifname, args[i],
                    ((struct sockaddr *) buffer) + i) < 0)
                    {
                        printf("Invalid address [%s]...\n", args[i]);
                        return(-1);
                    }
                }
                break;
            #endif
            default:
                fprintf(stderr, "Not implemented...\n");
                return(-1);
        }

        if((priv[k].set_args & IW_PRIV_SIZE_FIXED) &&
        (wrq.u.data.length != (priv[k].set_args & IW_PRIV_SIZE_MASK)))
        {
            printf("The command %s needs exactly %d argument(s)...\n",
            cmdname, priv[k].set_args & IW_PRIV_SIZE_MASK);
            return(-1);
        }
    }	/* if args to set */
    else
    {
        wrq.u.data.length = 0L;
    }

    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);

    /* Those two tests are important. They define how the driver
    * will have to handle the data */
    if((priv[k].set_args & IW_PRIV_SIZE_FIXED) &&
    ((iw_get_priv_size(priv[k].set_args) + offset) <= IFNAMSIZ))
    {
        /* First case : all SET args fit within wrq */
        if(offset)
            wrq.u.mode = subcmd;
        memcpy(wrq.u.name + offset, buffer, IFNAMSIZ - offset);
    }
    else
    {
        if((priv[k].set_args == 0) &&
        (priv[k].get_args & IW_PRIV_SIZE_FIXED) &&
        (iw_get_priv_size(priv[k].get_args) <= IFNAMSIZ))
        {
            /* Second case : no SET args, GET args fit within wrq */
            if(offset)
                wrq.u.mode = subcmd;
        }
        else
        {
            /* Third case : args won't fit in wrq, or variable number of args */
            wrq.u.data.pointer = (caddr_t) buffer;
            wrq.u.data.flags = subcmd;
        }
    }

    /* Perform the private ioctl */
    if(ioctl(skfd, priv[k].cmd, &wrq) < 0)
    {
        fprintf(stderr, "Interface doesn't accept private ioctl...\n");
        fprintf(stderr, "%s (%X): %s\n", cmdname, priv[k].cmd, strerror(errno));
        return(-1);
    }

    /* If we have to get some data */
    if((priv[k].get_args & IW_PRIV_TYPE_MASK) &&
    (priv[k].get_args & IW_PRIV_SIZE_MASK))
    {
        int	j;
        int	n = 0;		/* number of args */

        //printf("bruce %-8.16s  %s:\n", ifname, cmdname);

        /* Check where is the returned data */
        if((priv[k].get_args & IW_PRIV_SIZE_FIXED) &&
        (iw_get_priv_size(priv[k].get_args) <= IFNAMSIZ))
        {
            memcpy(buffer, wrq.u.name, IFNAMSIZ);
            n = priv[k].get_args & IW_PRIV_SIZE_MASK;
        }
        else
            n = wrq.u.data.length;

        switch(priv[k].get_args & IW_PRIV_TYPE_MASK)
        {
            case IW_PRIV_TYPE_BYTE:
            /* Display args */
            for(j = 0; j < n; j++)
                printf("%d  ", buffer[j]);
            printf("\n");
            break;

            case IW_PRIV_TYPE_INT:
                /* Display args */
                for(j = 0; j < n; j++)
                    printf("%d  ", ((__s32 *) buffer)[j]);
                printf("\n");
                break;

            case IW_PRIV_TYPE_CHAR:
                /* Display args */
                buffer[n] = '\0';
                //printf("bruce get msg from kernel:%s\n", buffer);
                break;

            #if 0
            case IW_PRIV_TYPE_FLOAT:
            {
                double		freq;
                /* Display args */
                for(j = 0; j < n; j++)
                {
                    freq = iw_freq2float(((struct iw_freq *) buffer) + j);
                    if(freq >= GIGA)
                        printf("%gG  ", freq / GIGA);
                    else
                        if(freq >= MEGA)
                            printf("%gM  ", freq / MEGA);
                        else
                            printf("%gk  ", freq / KILO);
                }
                printf("\n");
            }
            break;

            case IW_PRIV_TYPE_ADDR:
            {
                char		scratch[128];
                struct sockaddr *	hwa;
                /* Display args */
                for(j = 0; j < n; j++)
                {
                    hwa = ((struct sockaddr *) buffer) + j;
                    if(j)
                        printf("           %.*s",
                    (int) strlen(cmdname), "                ");
                    printf("%s\n", iw_saether_ntop(hwa, scratch));
                }
            }
            break;
            #endif
            default:
                fprintf(stderr, "Not yet implemented...\n");
                return(-1);
        }
    }	/* if args to set */

    return(0);
}



static inline int set_private(int		skfd,		/* Socket */
	    char *	args[],		/* Command line args */
	    int		count,		/* Args count */
	    char *	ifname,     /* Dev name */
	    u_char	*resp_data)
{
    iwprivargs *	priv;
    int		number;		/* Max of private ioctl */
    int		ret;

    /* Read the private ioctls */
    number = iw_get_priv_info(skfd, ifname, &priv);

    /* Is there any ? */
    if(number <= 0)
    {
        /* Should I skip this message ? */
        fprintf(stderr, "%-8.16s  no private ioctls.\n\n", ifname);
        if(priv)
            free(priv);
        return(-1);
    }

    /* Do it */
    ret = set_private_cmd(skfd, args + 1, count - 1, ifname, args[0],
        priv, number, resp_data);

    free(priv);
    return(ret);
}


int iwpriv_get_mac_table(unsigned char *resq_data)
{
    int skfd;     /* generic raw socket desc. */
    int goterr = 0;

    /* Create a channel to the NET kernel. */
    if((skfd = iw_sockets_open()) < 0)
    {
        perror("socket");
        return(-1);
    }

    char *argv[1] = {"get_mac_table"};
    char *ifname = "wlan0";
    //u_char resq_data[IWPRIV_GET_DATA_SIZE] = {0};
    goterr = set_private(skfd, argv + 0, 1, ifname, resq_data);

    iw_sockets_close(skfd);

    return 0;
}

int iwpriv_show_maccount()
{
    int skfd;     /* generic raw socket desc. */
    int goterr = 0;

    /* Create a channel to the NET kernel. */
    if((skfd = iw_sockets_open()) < 0)
    {
        perror("socket");
        return(-1);
    }

    char *argv[2] = {"show", "maccount"};
    char *ifname = "ra0";
    u_char resq_data[IWPRIV_GET_DATA_SIZE] = {0};
    goterr = set_private(skfd, argv, 2, ifname, resq_data);

    iw_sockets_close(skfd);

    return 0;
}



int iwpriv_main(int	argc,
     char **	argv)
{
    int skfd;     /* generic raw socket desc. */
    int goterr = 0;

    /* Create a channel to the NET kernel. */
    if((skfd = iw_sockets_open()) < 0)
    {
        perror("socket");
        return(-1);
    }

    u_char resq_data[IWPRIV_GET_DATA_SIZE] = {0};
    goterr = set_private(skfd, argv + 2, argc - 2, argv[1], resq_data);

    iw_sockets_close(skfd);

    return 0;
}

#if 0

int main(int	argc,
     char **	argv)
{

    iwpriv_main(argc, argv); //"iwpriv ra0 show maccount"
    //iwpriv_get_mac_table();
    //iwpriv_show_maccount();

    return 0;
}

#endif


#endif //the begin 0

