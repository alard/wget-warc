#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ()
{FILE *f;
int a;
char *s,c;
f = fopen("./app/wdata/testanvl-errkey.warc","w+");
s = "IP-Address: 112.241.224.241";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s = "\tChecksum: sha1:2ZWC6JAT6KNXKD37F7MOEKXQMRY75YY4";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s = "Related-Recof569983a-ef8c-4e62-b347-295b227c3e51";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s = "Data";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
fclose (f);
}
