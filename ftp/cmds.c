#include "cmds.h"

/* ������ ��� ������ ���������� */
const char months[][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Ukn"
};


/**
 * ������ �������� ������
 */
static int str_begin_with(const char *src, char *match)
{
    while (*match) {
	/* check source */
	if (*src == 0) {
	    return -1;
	}

	if (*match != *src) {
	    return -1;
	}
	match++;
	src++;
    }
    return 0;
}


/**
 * ������� ������ FTP � �����
 */
void do_send_reply(int sock, char *fmt, ...)
{
    static char str[256];
    va_list p_args;
    int r;

    va_start(p_args, fmt);
    r = vsnprintf(str, sizeof(str), fmt, p_args);
    va_end(p_args);

    /* printf(buf); */
    if (r > 0) {
	send(sock, str, r, 0);
    }
}

/**
 * ������ ������� ����������. ������� ��� ��� ���������� CHAN FAT
 * ������� ������������ ��������� ���� ����� ��������
 * ��� �������� �����
 */
int do_full_list(char *name, int sockfd)
{
    DIR dir;
    FILINFO fno;
    FRESULT rc;
    u16 day, mon, year;		/* ���� */
    int res = -1;

/*    PRINTF("do full list of %s", name); */

    /* ���� ������ ���� � ��������� ������� */
    int len = strlen(name);
    if (len > 1 && name[len - 1] == '/') {
	name[len - 1] = 0;
    }


    do {
	/* ��������� ���������� */
	rc = f_opendir(&dir, (TCHAR *) name);
	if (rc) {
	    do_send_reply(sockfd, "500 Internal Server Error\r\n");
	    break;
	} else {

	    for (;;) {
		/* ������ ���������� */
		rc = f_readdir(&dir, &fno);
		if (rc || !fno.fname[0]) {
		    break;	/* ����� ���������� */
		}

		day = fno.fdate & 0x1f;	// ���� 
		mon = ((fno.fdate >> 5) & 0x0f) - 1;	//�����
		year = ((fno.fdate >> 9) & 0x3f) + 1980;

		/* ���� ����� �� ������ */
		if (mon > 12) {
		    mon = 0;
		    year = 2000;
		}

		/* ���� ���������� */
		if (fno.fattrib & AM_DIR) {
		    do_send_reply(sockfd, "drw-r--r-- 1 root root %9d %3s %2u %4u %s\r\n", 0, months[mon], day, year, fno.fname);
		} else {
		    /* ���� ���� */
		    do_send_reply(sockfd, "-rw-r--r-- 1 root root %9d %3s %2u %4u %s\r\n", fno.fsize, months[mon], day, year, fno.fname);
		}
		res = 0;
	    }
	}
    } while (0);
    
/*    PRINTF("...OK\r\n"); */
    
    return res;
}


/**
 * ��� ������� �������� �� ������� NLST
 */
int do_simple_list(char *name, int sockfd)
{
    FILINFO fno;
    FRESULT rc;
    DIR dir;

/*    PRINTF("do simple list of %s", name);  */
    
    /* ��������� ���������� */
    rc = f_opendir(&dir, (TCHAR *) name);
    if (rc != FR_OK) {
	do_send_reply(sockfd, "500 Internal Error\r\n");
	return -1;
    }
    
    /* ���� ������� - ���������� */
    while (1) {
	rc = f_readdir(&dir, &fno);
	if (rc || !fno.fname[0]) {
	    break;		/* ����� ���������� */
	}
	do_send_reply(sockfd, "%s\r\n", fno.fname);
    }
    
/*    PRINTF("...OK\r\n"); */
    return 0;
}

/**
 * ������� ������ �����. ���� ��� �� ������. ����������� ����� �������������
 */
int do_get_filesize(char *filename)
{
    FIL file;
    int res = -1;

    if (f_open(&file, (TCHAR*)filename, FA_READ) == FR_OK) {
	res = f_size(&file);
	f_close(&file);
    }

    return res;
}

/**
  * ���������� ����� �������� ������� �� enum
  * ��� 2 ������� ����� ��� �� ��������������,
  * ����� �� �������� ������ ���
  */
int do_parse_command(char *buf)
{
    if (str_begin_with(buf, "USER") == 0) {
	return CMD_USER;
    } else if (str_begin_with(buf, "PASS") == 0) {
	return CMD_PASS;
    } else if (str_begin_with(buf, "LIST") == 0) {
	return CMD_LIST;
    } else if (str_begin_with(buf, "NLST") == 0) {
	return CMD_NLST;
    } else if (str_begin_with(buf, "PWD") == 0 || str_begin_with(buf, "XPWD") == 0) {
	return CMD_PWD;
    } else if (str_begin_with(buf, "TYPE") == 0) {
	return CMD_TYPE;
    } else if (str_begin_with(buf, "PASV") == 0) {
	return CMD_PASV;
    } else if (str_begin_with(buf, "RETR") == 0) {
	return CMD_RETR;
    } else if (str_begin_with(buf, "STOR") == 0) {
	return CMD_STOR;
    } else if (str_begin_with(buf, "SIZE") == 0) {
	return CMD_SIZE;
    } else if (str_begin_with(buf, "MDTM") == 0) {
	return CMD_MDTM;
    } else if (str_begin_with(buf, "SYST") == 0) {
	return CMD_SYST;
    } else if (str_begin_with(buf, "CWD") == 0) {
	return CMD_CWD;
    } else if (str_begin_with(buf, "CDUP") == 0) {
	return CMD_CDUP;
    } else if (str_begin_with(buf, "PORT") == 0) {
	return CMD_PORT;
    } else if (str_begin_with(buf, "REST") == 0) {
	return CMD_REST;
    } else if (str_begin_with(buf, "MKD") == 0) {
	return CMD_MKD;
    } else if (str_begin_with(buf, "DELE") == 0) {
	return CMD_DELE;
    } else if (str_begin_with(buf, "RMD") == 0) {
	return CMD_RMD;
    } else if (str_begin_with(buf, "FEAT") == 0) {
	return CMD_FEAT;
    } else if (str_begin_with(buf, "QUIT") == 0) {
	return CMD_QUIT;
    } else {
	return CMD_UKNWN;
    }
}

/**
 * ���������� �� ������� ����, ���� �������� ������� ������� �� ���������� ����
 * ���� ��������� ������ - ���������� �� � ���������� �� ��� ����������!!!
 * �������� ������
 */
int do_step_down(char *path)
{
    int len, i;
    int res = -1;

    len = strlen(path);

    /* ������� ������ ���� */
    for (i = len - 1; i > 0; i--) {
	if (path[i] == '/') {
	    path[i] = 0;
	    res = 0;
	    break;
	}
    }

    /* ���� ����� �� ����� - ����� ���������� � ������ */
    if (res != 0) {
	path[0] = '/';
	path[1] = 0;
    }

    return 0;
}

/**
 * ������� ���������� ����, ������ ������� //
 * � ������� ��������� ��� �������� �� ���������� ���� ../..
 */
int do_full_path(struct conn *conn, char *path, char *new_path, size_t size)
{
    if (path[0] == '/') {
	strcpy(new_path, path);
    } else if (strcmp("/", conn->currentdir) == 0) {
	sprintf(new_path, "%s", path);
    } else if (path[0] == '.' && path[1] == '.') {
	/* ����� ���������� �� ������� ���� */
	PRINTF("Suspicius path\r\n");
	do_step_down(conn->currentdir);
	sprintf(new_path, "%s", conn->currentdir);
    } else {
	sprintf(new_path, "%s/%s", conn->currentdir, path);
    }

    return 0;
}
