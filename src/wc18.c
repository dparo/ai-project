#include<stdlib.h>
#include <unistd.h>
#include<errno.h>
#include<stdio.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */


long
hexToLong(char * str)
{
    long val=0;
    int n,k;
    for(k=0; str[k]; k++) {
        if(str[k]<='9' && str[k]>='0') n=str[k]-'0';
        if(str[k]<='Z' && str[k]>='A') n=str[k]-'A'+10;
        if(str[k]<='z' && str[k]>='a') n=str[k]-'a'+10;
        val= val *16 + n ;
    }
    return val;
}

struct sockaddr_in indirizzo;
struct header {
    char * n;
    char * v;
};
struct header h[100];

int main()
{
    long dim;
    int s,k,t,i,j;
    char *p;
    char primiduepunti, chunked = 0;
    char * status_line, *entity, *chunksize;
    int content_length;
    char request[1000];
    char response[100000];
    s = socket(AF_INET,SOCK_STREAM,0);
    if (s == -1) {
        perror("Socket Fallita");
    }
    indirizzo.sin_family  = AF_INET;
    indirizzo.sin_port = htons(80);
    ((unsigned char *)&(indirizzo.sin_addr.s_addr))[0] = 74;
    ((unsigned char *)&(indirizzo.sin_addr.s_addr))[1] = 125;
    ((unsigned char *)&(indirizzo.sin_addr.s_addr))[2] = 206;
    ((unsigned char *)&(indirizzo.sin_addr.s_addr))[3] = 94;
// google.com 216.58.213.228
// google.co.uk 74.125.206.94
    t = connect(s,(struct sockaddr *)&indirizzo, sizeof(struct sockaddr_in));
    if ( t == -1) {
        perror("Connect fallita\n");
    }
    sprintf(request,"GET /?gfe_rd=cr&dcr=0&ei=7qmvWvjcMdfEaM21i4AK HTTP/1.1\r\nHost:www.google.co.uk\r\n\r\n");
//sprintf(request,"GET / HTTP/1.1\r\nHost:www.google.com\r\n\r\n");
    write(s,request,strlen(request));

    h[0].n = response;
    status_line = h[0].n;
    h[0].v = h[0].n;
    for ( i = 0, j = 0; read(s,response+i,1); i++ ) {
        if (( i > 1) && (response[i] == '\n') && (response[i - 1] == '\r')) {
            primiduepunti = 1;
            response[i - 1] = 0;
            if(h[j].n[0] == 0) break;
            h[++j].n = response + i + 1;
        }
        if ( primiduepunti && (response[i] == ':') ){
            h[j].v = response + i + 1;
            response[i] = 0;
            primiduepunti = 0;
        }
    }

    printf("Status Line: %s\n", status_line);
    content_length = 0;
    for ( i = 1; i < j; i++ ){
        printf("%s ===> %s\n", h[i].n, h[i].v );
        if ( strcmp("Content-Length", h[i].n) == 0 ) {
            content_length = atoi(h[i].v);
        }
        else if(strcmp("Transfer-Encoding", h[i].n) == 0 && strcmp(" chunked", h[i].v) == 0) chunked = 1;
    }

    if( content_length != 0 ) {
        entity = malloc(content_length + 1);
        for ( i = 0;
              (i < content_length)
                  && (t = read(s, entity + i, content_length - i));
              i = i + t )
        { }
    } else if ( chunked ) {
        chunksize = response + i;
        for (k=0; (t=read(s, response +i+k , 1))>0;  k++) {
            if ( response[i + k]== '\n' && response[i+k-1]=='\r' ) {
                response[i + k - 1]=0;
                dim = hexToLong(chunksize);
                printf("<%s> %ld\n",chunksize,dim);
                for(j = 0;(t = read(s, response + i + k ,dim - j)) > 0 && j < dim; j += t, k += t );
                // Consume CRLF after chunksize
                t = read(s,response + i + k, 2);
                //response[i+k+t]=0;
                //printf("***%s***\n",response+i+k);

                k += 2;
                if ( dim == 0 ) { printf("Fine body"); break; }
                chunksize = response + i + k;
                k--;
            }

        }
    }
    if ( t < 0 ) {
        perror("read fallita");
        return 1;
    }
//entity[i]=0;
//printf("%s",entity);
//free(entity);


/*
  p = response;
  while (t=read(s,p,100000)){
  p = p + t;
  *p=0;
  }
*/
}
