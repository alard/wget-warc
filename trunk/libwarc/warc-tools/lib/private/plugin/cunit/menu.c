#include <stdio.h>
#include <wmisc.h>

WPUBLIC int menu (void)
{
    int choix = 0;
   
    while (choix < 1 || choix > 4)
    {
        printf(" Running Tests :\n");
        printf("1 : Interactive Console Mode  \n");
        printf("2 : Basic Mode  \n");
        printf("3 : Automated Mode\n");  
        printf("Select a number ? ");
        scanf("%d", & choix);
    }   

    if (choix==2)
      {
		choix=0;
		while (choix < 21 || choix > 23)
          {
            printf("\n Running Tests in mode basic :\n\n");
            printf("21 : Failures and run summary are printed . \n");
            printf("22 : Maximum output of run details. \n");
            printf("23 : No output is printed except error messages.\n");  
            printf("Select a number ? ");
            scanf("%d", & choix);
          }  
      }

    if (choix==4)
      choix = 21;


    return choix;
}   
