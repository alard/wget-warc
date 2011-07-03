#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ()
{FILE *f;
int a;
char *s,c;
f = fopen("./app/wdata/tcommanvl.warc","w+");
s= "#leédhcjefhcudfhvfdjkhvfeuihrvuiefchichduichiuiuqca";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s = "IP-Address: 207.241.224.241";
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
s= "#leédhcjefhceuihvuiefrchixxxxxxxxchduichiuiuqca";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s= "#Other comment";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s= "#THird comment";
fwrite (s,strlen(s),1,f);
a = 0x0D;
fwrite(&a,1,1,f);
a = 0x0A;
fwrite(&a,1,1,f);
s = "Related-Record-ID: uuid:f569983a-ef8c-4e62-b347-295b227c3e51";
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
