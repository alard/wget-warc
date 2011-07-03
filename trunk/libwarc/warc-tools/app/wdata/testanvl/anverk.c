#include <stdio.h>

int main()
{
FILE * f = fopen ("./app/wdata/anvl-ctlkey.anvl","w+");
char i;

fwrite ("ke",2,1,f);
i = 1;
fwrite (&i,1,1,f);
fwrite ("y1",2,1,f);
fclose (f);
return 0;
}
