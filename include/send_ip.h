// #pragma once
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <fcntl.h>

// void send()
// {
//     struct sockaddr_in p;
//     p.sin_
//     struct sockaddr_in q;
//     int z,s,t,x,n,b = sizeof(q);
//     char buf[23];
//     int fd;

//     s = socket(AF_INET,SOCK_STREAM,0);

//     bzero (&p,sizeof(p));
//     p.sin_family = AF_INET;
//     p.sin_addr.s_addr = INADDR_ANY;
//     p.sin_port = htons(8080);

//     bind (s,(struct sockaddr*)&p,sizeof(p));
//     listen(s,13);

//     while (1) {
//         t = accept (s,(struct sockaddr*)&q, &b);
//         n = read(t,buf,23);
//         buf[n] = 0;
//         printf("%s sent: ",buf);
//         fd = open (buf,O_RDONLY);
//         z = lseek(fd,0,SEEK_END);
//         lseek(fd,0,SEEK_SET);
//         x = 0;
//         do {
//             n = read (fd,buf,23);
//             write (t,buf,n);
//             x += n;
//         } while (x < z);
//         printf("%d/%d\n",x,z);
//         close(fd);
//         close(t);
//     }
// }

