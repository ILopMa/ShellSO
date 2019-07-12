/**
UNIX Shell Project
Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores ‐ UMA
Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.
To compile and run the program:
$ gcc Shell_project.c job_control.c -o Shell
$ ./Shell
(then type ^D to exit program)
**/

#include<string.h>
#include "job_control.h" /* remember to compile with module job_control.c */
//Colores
#define ROJO "\x1b[31;1;1m"			
#define NEGRO "\x1b[0m"
#define VERDE "\x1b[32;1;1m"
#define AZUL "\x1b[34;1;1m"
#define CIAN "\x1b[36;1;1m"
#define MARRON "\x1b[33;1;1m"
#define PURPURA "\x1b[35;1;1m"
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

void manejador2(int p){
	FILE *pf;
        pf=fopen("hup.txt","a"); // abre un fichero en modo 'append'
        fprintf( pf, "SIGHUP recibido.\n"); //escribe en el fichero
        fclose( pf );
}




// -‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐--‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐
// MAIN
// -‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐--‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐-‐‐

job *lista;
job *hist;

void manejador(int p)		//funcion manejador
{
	int i, status, info, pid_wait;  //inicializamos las variables que vamos a usar
	enum status status_res;		//estados


	block_SIGCHLD(); //bloqueo para no modificar la lista

	for(i=1; i<=list_size(lista); i++){ //recorrido de la lista


		job *jb;	//puntero al job que vamos a coger con get_item_bypos
		jb=get_item_bypos(lista, i); //cogemos el trabajo que esta en la posicion i
		if (jb == NULL) { //controlamos que el trabajo es null
			printf("El trabajo es null\n"); //si es null decimos que continue con el siguiente comando
			 continue;
		 }

		//las funciones waitpid son funciones que se utilizan para esperar hasta que los procesos cambien de estado
		//por eso se pone el & ya que vamos a modificar el estado
		//
		pid_wait = waitpid(jb->pgid, &status, WUNTRACED|WNOHANG|WCONTINUED); //waitpid no bloqueante,esperamos cambio estado
		//con las constantes WUNTRCED,WHOHANG,WCONTINUED
		//< -1   significa esperar a que cualquier proceso hijo cuyo ID del proceso es igual
        //      al valor absoluto de pid.
		//
        //-1     significa  que  espera  por  cualquier  proceso  hijo;  este  es  el  mismo
        //      comportamiento que tiene wait.

        //0    significa  que  espera  por cualquier proceso hijo cuyo ID es igual al del
        //      proceso llamante.

       //> 0    significa que espera por el proceso hijo cuyo ID es igual al valor de pid.


		if(pid_wait == jb->pgid){ //si el pid_wait tiene mi id significa que me ha ocurrido un cambio de los notificados anteriormente

			//Hay que poner 3 IF (hay 3 estados), solo nos da 1 (el de matar), los otros 2 los tenemos que implementar en la Tarea 4

			// la funcion analyze_status analiza el entero devuelto por status y devuelve la causa de la

			//terminacion de una tarea (como enumerado status) y la info asociada a dicha terminacion (info)


			//Esta función analiza el entero status devuelto por la llamada waitpid y devuelve la causa de
			//terminación de una tarea (como un enumerado status) y escriben la información adicional asociada a
			//dicha terminación en info.
			status_res=analyze_status(status, &info); //Devuelve estado de terminacion de la tarea

			printf(VERDE"El estado es: %s\n",status_strings[status_res]); //imprimo el analyze_status

			// Actuar segun los posibles casos reportados por status
			// Al menos hay que considerar EXITED, SIGNALED, y SUSPENDED
			// En este ejemplo sólo se consideran los dos primeros

			if((status_res==SIGNALED) || (status_res==EXITED)){ //si el estado es uno de estos dos

				printf(VERDE"proceso eliminado\n");
				delete_job(lista, jb); //eliminamos el trabajo de la lista

			}else if (status_res == SUSPENDED){ //si esta suspendido
/////////////////////////////////////////////////////////////////////////////////////
				killpg(jb->pgid,SIGSTOP);  //esto realiza las acciones con el control+z
				////////////////////////////////////////////////////////////////////////////////
				jb->state=STOPPED; //las paramos



			}else if(status_res == CONTINUED){ //si es continued la mandamos a background
				jb->state=BACKGROUND; //la mandamos a segundo plano
				////////////////////////////////////////////////////////////////////////////
				killpg(jb->pgid,SIGCONT);  //esto realiza las acciones con el control+z
				printf("Proceso en background, %s, pid %d\n",jb->command,jb->pgid);
				/////////////////////////////////////////////////////////////////////////////
			}

			print_job_list(lista); //por ultimo imprimimos la lista

		} // if
	} // for
	unblock_SIGCHLD(); //debloqueamos las señales
	fflush(stdout);
	return;
}



int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */

	int background; /* equals 1 for commands followed by char '&' */

	char *args[MAX_LINE/2]; /* command line (of 256) has max of 128 arguments */
	// probably useful variables:

	int pid_fork, pid_wait; /* pid for created and waited process */

	int status; /* status returned by wait */

	enum status status_res; /* status processed by analyze_status() */

	enum status status3;

	int info; /* info processed by analyze_status() */

	int pid_shell=getpid();



	ignore_terminal_signals();
	//Estas funciones se utilizan para desactivar o activar las señales relacionadas
	//con el terminal (SIGINT SIGQUIT SIGTSTP SIGTTIN SIGTTOU) . El Shell debe ignorar
	//estas señales, pero el comando creado con fork() debe restaurar su comportamiento por defecto.

	signal(SIGCHLD,manejador);//instalacion del manejador
	signal(SIGHUP,manejador2);

	lista=new_list("listajobs");//inicializamos la lista
	hist=new_list("historial");
	printf(PURPURA"SHELL SISTEMAS OPERATIVOS\n"); //Printf de bienvenida
	fflush(stdout);

	while (1) /* Program terminates normally inside get_command() after ^D is typed*/
	{
		printf(AZUL"COMANDO‐‐>"); /* print prompt */
		fflush(stdout);

		/* get next command */

		get_command(inputBuffer, MAX_LINE, args, &background); //lee el comando



		if(args[0]==NULL){ //si es null simplemente pide el siguiente comando
			continue; /* do nothing if empty command */

		}

		if(strcmp(args[0],"historial")==0) {  //si es historial imprimo la lista del historial
			int n=0;
			job * aux=hist;
			while(aux->next!= NULL)
			{
				printf(CIAN" [%d] ",n);		//numero de la lista del historial
				printf(CIAN"%s\n",aux->command);	//comando almacenado de la lista
				n++;
				aux=aux->next;
			}

				if(args[1] != NULL){
					int pos=atoi(args[1]);
					job* njob = get_item_bypos(hist,pos);
					job* copy =new_job(getpid(),njob->command,FOREGROUND);
					if(njob==NULL){
						printf(ROJO"Error, no existe el job\n");
					}else{
						strcpy(args[0],copy->command);
						args[1]=NULL;
					}
				}
		}




		if(strcmp(args[0],"currjob")==0){ //si solo es cd
			job * aux=lista->next;
			printf(VERDE"Proceso en background, %s, pid %d\n",aux->command,aux->pgid);
			continue;

		}
		
		
		if(strcmp(args[0],"deljob")==0){ //si solo es cd
			job * aux=lista->next;
			delete_job(lista,aux);
			continue;

		}





		if(strcmp(args[0],"cd")==0 && args[1]==NULL){ //si solo es cd
			add_job(hist,new_job(getpid(),args[0],FOREGROUND)); //funcion que añade a la lista historial aparece al principio de todos los strcmp
			chdir(getenv("HOME")); //cambia el directorio a home(predeterminado)
			continue;

		}
		if(strcmp(args[0],"cd")==0) { //cd con argumentos
			add_job(hist,new_job(getpid(),args[0],FOREGROUND));
			if(chdir(args[1])==-1){//pero el segundo argumento no se lee, envia mensaje de error
				printf(ROJO"[ERROR, DIRECTORIO NO ENCONTRADO]\n");
			}else{ //sino va a dicha direccion
				chdir(args[1]);
			}
			continue;

		}
		if(strcmp(args[0],"jobs")==0) {  //imprimo listajobs
			add_job(hist,new_job(getpid(),args[0],FOREGROUND));
			print_job_list(lista);
			continue;
		}





		if(strcmp(args[0],"bg")==0) { //bg
			int pos;

			if(args[1]==NULL){ //y no te devuelve ningun argumento mas
				pos=1;		//coges el primero
			}else{
				pos=atoi(args[1]); //Conversion string a int
			}
			job *njob = get_item_bypos(lista,pos); //cojo la tarea de la posicion pos
			if(njob==NULL){ //si es null imprimo el error
				printf(ROJO"error,no existe el job\n");
			}else{
				if(njob->state == STOPPED){ //si el job esta en estado stopped
					njob->state = BACKGROUND; //lo ponemos a background
					killpg(njob->pgid,SIGCONT);  //esto realiza las acciones con el control+z
					printf(VERDE"Proceso en background, %s, pid %d\n",njob->command,njob->pgid);
				}
				continue;
			}
		}else if(strcmp(args[0],"fg")==0) { //si el comando es foreground(fg) igual que el background(bg)
			int pos;

			if(args[1] == NULL){
				pos = 1;
			}else{
				pos = atoi(args[1]);
			}
			job* njob = get_item_bypos(lista, pos);
				if(njob==NULL){
					printf(ROJO"Error, no existe el job\n");
				}else{
					if(njob->state == STOPPED || njob->state == BACKGROUND){ //si es stopped o background
						set_terminal(njob->pgid);//le doy el terminal
						njob->state = FOREGROUND;//ponemos en estado foreground
						killpg(njob->pgid, SIGCONT);
						printf(VERDE"Proceso en Foreground: %s ,pid: %d\n",njob->command,njob->pgid);
						waitpid(njob->pgid,&status,WUNTRACED);//hago un pid hasta que este suspendido
						status3=analyze_status(status,&info); //analizo el estado
						set_terminal(getpid()); //cojo el terminal
						if(status3==SUSPENDED){ //compruebo si esta suspendido
							njob->state=STOPPED; //si lo esta lo pongo es stopped
							printf(VERDE"Proceso suspendido\n");
						}else{ //sino, lo elimino de la lista
							printf(VERDE"pid: %d, command: %s, %s, info: %i \n",njob->pgid,njob->command,status_strings[status3],info);
							delete_job(lista,njob);
						}
					}

				}
				continue;




		}else{
			/* the steps are:
				(1) fork a child process using fork()
				(2) the child process will invoke execvp()
				(3) if background == 0, the parent will wait, otherwise continue
				(4) Shell shows a status message for processed command
				(5) loop returns to get_commnad() function
			*/
			pid_fork = fork(); // (1) fork a child process using fork()


			//Este switch permite crear un nuevo grupo de procesos para el proceso recién creado. Debe realizarlo
			//el Shell después de hacer fork().
			new_process_group(pid_fork);

			switch (pid_fork) {  //comprobamos el pid_fork
			case -1: 					 // si es -1 lanza un error
				perror("fork");
				break;
			case 0: 					// si es un 0 es un hijo
				pid_fork = getpid(); //recoge el pid del proceso
				new_process_group(pid_fork);
				add_job(hist,new_job(getpid(),args[0],FOREGROUND));
				if (!background) {  //si no esta en background
					set_terminal(pid_fork); //le da a pid_fork el terminal
				}
				restore_terminal_signals(); //desactiva o activa las señales relacionadas con el terminal
				execvp(args[0],args);		//ejecuta el primer comando
				perror("execvp");
				exit(-1);
				break;
			default: 			// si no, es padre
				new_process_group(pid_fork);
				if (!background) { //si no esta en background
					add_job(hist,new_job(getpid(),args[0],FOREGROUND));
					set_terminal(pid_fork); //le da al hijo el terminal
					pid_wait = waitpid(-pid_fork,&status,WUNTRACED);  //
					status_res=analyze_status(status,&info);
					set_terminal(getpid()); //coge el terminal
					if (status_res!=SUSPENDED) {
						printf(VERDE"Foreground pid: %d, command: %s, %s, info: %i \n",pid_wait,args[0],status_strings[status_res],info); //FG
					} else {
						printf(VERDE"Stopped pid: %d, command: %s\n",pid_wait,args[0]);		//Stopped
					job * j = new_job(pid_fork,args[0],STOPPED);
					block_SIGCHLD();
					add_job(lista,j);
					unblock_SIGCHLD();
					}
				} else {
					printf(VERDE"Background job running... pid: %i, command: %s\n",pid_fork,args[0]);	//bg
					job * j = new_job(pid_fork,args[0],BACKGROUND);
					add_job(hist,j);
					block_SIGCHLD();
					add_job(lista,j);
					unblock_SIGCHLD();
				}
				break;
			} // switch fork
		} // else if chain
	} // end while
	}
