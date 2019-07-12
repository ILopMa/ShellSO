# ShellSO
Shell de la asignatura de sistemas operativos (UMA)

Con los ejercicios  obligatorios del examen de junio 2019

Para hacer llamadas a señales (por ejemplo SIGHUP):
           - Teclea ps en la Shell.
           - Copia el número de PID de Shell.
           - Escribe kill nºPID -SIGHUP.

Modificaciones obligatorias [total: 5 ptos]:

        Añadir la cadena "SIGHUP recibido.\n" al fichero 'hup.txt' si el shell recibe una señal SIGHUP (es la señal número 1). [1 pto]
            Pista:
                FILE *pf;
                pf=fopen("hup.txt","a"); // abre un fichero en modo 'append'
                fprintf( pf, "SIGHUP recibido.\n"); //escribe en el fichero
                fclose( pf );


        Crea un comando interno llamado 'currjob' que imprima el PID y el comando del trabajo actual, entendiendo como éste al que se refieren los comandos fg y bg sin argumentos (primero o último de la lista dependiendo de tu implementación). [2 ptos]


        Crea un comando interno llamado 'deljob' que elimine de la lista de jobs el comando actual (idem del caso anterior). Solo quitar de la lista, el proceso continuará normalmente, de manera que el comando no aparecerá cuando se listen a través del comando jobs. [2 ptos]
        
       
