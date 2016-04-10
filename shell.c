#include <sys/wait.h>			//waitpid()
#include <sys/types.h>
#include <unistd.h>			//chdir();fork();pid_t;pipe()
#include <stdlib.h>			//malloc();free();execvp();exit();EXIT_FAILURE
#include <stdio.h>
#include <string.h>			//strtok()
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

char **split_input(char *line,char *str);
char home[1000],*last,*line,line2[1000],line1[1000],xa[1000],xb[1000],*xc,st[100];		//stores home directory path
int background,n,count=0;
int redirect_output,redirect_input,append,npipes;
int p1[1000],condition;
int sigf = 0;
//int gl;
#define gl getpid()
//int sp;
pid_t p;
int pflag,g;
//printf("blah");
//backgorund processes linked list
struct node{
	int id;
	char com[100];
//	int cont;
//	int state;
	struct node * next;
}*head=NULL,*temp,*end=NULL;
void sh1()			//signal handler for ctrl+c
{
//	sigf = 1;
	printf("\n");
//	exit(0);
	condition = 1;
	return;
//	return 1;
}

void sh2()			//signal handler for ctrl+z
{
	printf("\n");
//	printf("%d\n",p);
//	return 1;
//	if(pflag != 1)
//	count++;
//	sigf = 1;
//	setpgid(p,p);
//	kill(p,SIGTSTP);
//	printf("\n[%d]+	stopped		[%d] %s\n",count,p,st);
//	tcsetpgrp(0,gl);
//	kill(p,SIGTSTP);
//	setpgid(p,p);
	condition = 1;
	return;
}

void checkbp()
{

      signal (SIGINT, SIG_DFL);
      signal (SIGTSTP, SIG_DFL);
      signal (SIGCHLD, SIG_DFL);
//	char * tcom;
	int status;
	pid_t pid1;
//	printf("i'm going in\n");
	pid1 = waitpid(-1,&status,WNOHANG);   //finds pids of process tht have been terminated
	while(pid1 > 0)
	{
		int i = 0;
		temp = head;
		struct node * temp1;
		if(head != end)
			temp1 = head;
		while(temp != NULL)
		{
//			printf("delete\n");
			if(temp -> id == pid1)
			{

				if(WIFSIGNALED(status))
					printf("%s with pid %ld terminated due to signal %d\n", temp -> com, (long)pid1, WTERMSIG(status));
				else
					printf("%s with pid %ld terminated normally\n", temp -> com, (long)pid1);
//				tcom = temp -> com;
				if(head == temp)  //remove that process from list of background processes
				{
					if(head == end)
					{
						head = NULL;
						end = NULL;
					}
					else
						head = head -> next;
				}
				else
				{
					temp1 -> next = temp -> next;
				}
				count--;
			}
			temp = temp -> next;
			if(i >= 1)
				temp1 = temp1 -> next;
			i++;
		}

/*		if(WIFSIGNALED(status))
			printf("%s with pid %ld terminated due to signal %d\n", tcom, (long)pid1, WTERMSIG(status));
		else
			printf("%s with pid %ld terminated normally\n", tcom, (long)pid1);
*/
      		pid1 = waitpid(-1, &status, WNOHANG);
	}

      signal (SIGINT, sh1);
      
      signal (SIGTSTP, sh2);
//      signal(SIGCHLD, SIG_IGN);
//	printf("exiting\n");
//	tcsetpgrp(0,gl);
}
//change directory implementation
int cd(char **args)
{
	if(args[1] == NULL)		//cd
		chdir(home);
	else if(strcmp(args[1],"~")==0) //cd ~
		chdir(home);
	else if (chdir(args[1]) != 0)
		    printf("No such file or directory\n");
//	printf("hehe");
  return 1;
}
// exit shell implementation
int ex() {
  return 0;
}

int jobs()
{
	checkbp();   //removes all background processes that have been terminated
	int i=1;
	temp = head;
	while(temp != NULL)
	{
//		puts(temp->com);
//		if(temp -> state == 1)
			printf("[%d] %s [%d]\n",i,temp->com,temp->id);
		temp = temp->next;
		i++;
//		if(i == 6)
//			break;
	}
	return 1;
}

int kjob(char **args)
{
	int i,flag=0;
	int a=0,b=0,c;
	for(i=0;i<strlen(args[1]);i++)
		a=(a*10+((int)args[1][i])-48);
	for(i=0;i<strlen(args[2]);i++)
		b=(b*10+((int)args[2][i])-48);
//	printf("%d\n%d\n",a,b);
	i = 1;
	temp = head;
	while(temp != NULL)
	{
		if(a == i)
		{
			flag = 1;  //job number is found in the list of jobs running
			c = temp -> id;
//			if(b == SIGSTOP)
//				temp -> cont = 1;
		}
		temp = temp -> next;
		i++;
	}
	if(flag == 1)
	{
		kill(c,b);   //send the signal
		if( errno == EINVAL )
			printf("Invalid signal number\n");
	}
	else
	{
		if(flag != 1)
			printf("Invalid job number\n");
/*		if(b>=NSIG || b<=0)
			printf("Invalid signal number\n");*/
	}
	return 1;
}

int fg(char **args)
{
//	signal(SIGCHLD,SIG_DFL);
	int i,flag=0;
	int a=0,b=0,c;
	if(args[1] == NULL)
	{
		printf("No job number specified\n");
		return 1;
	}
	for(i=0;i<strlen(args[1]);i++)
		a=(a*10+((int)args[1][i])-48);

	i = 1;
	temp = head;
	struct node * temp1;
	while(temp != NULL)   //check if given job exists
	{
		if(a == i)
		{
			flag = 1;
			c = temp -> id;
//			printf("%d\n",c);
//			cont = temp -> cont;
			temp1 = temp;
		}
		temp = temp -> next;
		i++;
	}

	if(flag != 1)
		printf("Invalid job number\n");
	else
	{
		printf("[%d] ",temp1 -> id);
		puts(temp1 -> com);
		int status;
		pid_t wpid;
//		int x = tcgetpgrp(0);
//		printf("%d\n",x);
//		printf("%d\n%d\n",tcgetpgrp(0),c);
//		kill(x,SIGSTOP);
//		tcsetpgrp(0,c);
//		if(cont)
		kill(c,SIGCONT);  //brings the job to the foreground
//		printf("haha\n");
//		p = c;
//		pflag = 0;
//      		signal (SIGCHLD, SIG_DFL);
//		getchar();
		do
		{
//			printf("hehe%d\n",sigf);
/*			  if(sigf == 1)
			  {
//				  pflag = 1;
//				  printf("haha\n");
//				  sigf = 0;
//				  kill(c,SIGSTOP);
//				  setpgid(pid,pid);
//				  kill(0,SIGCONT);
//				  kill(pid,SIGSTOP);
//				  return 1;
				  break;
			  }*/
      			wpid = waitpid(c, &status, WUNTRACED);			//parent process waits for child process to finish executing the command
		}while(!WIFSIGNALED(status) && !WIFEXITED(status) && !sigf);
/*		if(sigf == 1)
		{
			pflag = 1;
			sigf = 0;
		}*/
//		if(pflag != 1)
//		{
//			printf("fg delete\n");
			temp = head;
			if(head != end)
				temp1 = head->next;
		while(temp != NULL)
		{
//			printf("delete\n");
			if(temp -> id == c)
			{
				if(head == temp)
				{
					if(head == end)
					{
						head = NULL;
						end = NULL;
					}
					else
						head = head -> next;
				}
				else
				{
					temp1 -> next = temp -> next;
				}
				count--;
			}
			temp = temp -> next;
			if(temp != NULL)
				temp1 = temp1 -> next;
		}
//		}
//		tcsetpgrp(0,gl);
//		main();
//		printf("hehe\n");
//		kill(x,SIGCONT);
//		printf("%d\n",tcgetpgrp(0));
//		printf("yolo\n");
	}
	return 1;
}

int overkill()
{
	temp = head;
	while(temp != NULL)
	{
		kill(temp -> id, 9);
		temp = temp -> next;
	}
	return 1;
}

// System commands.
int sys_com(char **args,char **args2)
{
  char **pargs;
  pid_t pid, wpid, pid1;
  int status,i,j;
//  printf("before fork");
//	printf("out\n");
  if (npipes > 0)				//pipe implementation
  {
/*	int fd[2*npipes];
	for(i=0;i<npipes;i++)
		pipe(fd+2*i);	*/
	int f,f1;
	int fd[2],fd1[2];
//	puts(xa);
	if (redirect_input == 1)
		pargs = split_input(xb,"|<>");
	else
		pargs = split_input(xa,"|<>");
//	puts(xa);
	char ** temp;
	temp = args;
//	for(i=0;pargs[i]!=NULL;i++)
//		puts(pargs[i]);
	for(i=0;i<=npipes;i++)
	{
//		printf("i = %d\n",i);
		args = split_input(pargs[i]," \t\r\n\a");
//		printf("--\n");
		if(i % 2 == 1)
			pipe(fd);
		else
			pipe(fd1);
//		printf("haha\n");
		pid = fork();
		if (pid == 0)
 		{
//			printf("yo\n");
			if (i == 0)
			{
				if(redirect_input == 1)
				{
					args2 = args;
					args = temp;
//					puts(args2[0]);
//					puts(args[0]);
					f = open(args2[0],O_RDONLY|O_EXCL,0600);
					if(f == -1)
					{
						printf("File/Directory doesn't exist\n");
						exit(0);
					}
					else
					{
						dup2(f,0);
						close(f);
					}
				}
				dup2(fd1[1],1);
			}
			else if(i == npipes)
			{
//				printf("yyyy\n");
				if(redirect_output == 1)
				{
//					printf("yo\n");
					if(append == 2)
						f1 = open(last,O_WRONLY|O_APPEND|O_CREAT,0600);
					else
					{
						off_t off = 0;
						truncate(last,off);
//						puts(last);
						f1 = open(last,O_WRONLY|O_CREAT,0600);
					}
					dup2(f1,1);
					close(f1);
				}
				if(i % 2 == 0)
					dup2(fd[0],0);
				else
					dup2(fd1[0],0);
			}
			else
			{
				if(i % 2 == 1)
				{
					dup2(fd1[0],0);
					dup2(fd[1],1);
				}
				else
				{
					dup2(fd[0],0);
					dup2(fd1[1],1);
				}
			}
/*			for(j = 0; j < 2*npipes; j++)
				close(fd[i]);*/
//		printf("---\n");
    		if (strcmp(args[0],"echo") == 0 && args[1][0] == '$')   //echo: environment variables
    		{
//	    printf("blah");
	    		if(strcmp(args[1],"$HOME")==0)
		    		printf("%s\n",home);
	    		else
	    		{
		    		char s[200];
	    			for(i=0;args[1][i]!='\0';i++)
		    			s[i]=args[1][i+1];
	    			char *x=getenv(s);
	    			if(x!=NULL)
//		    printf("yes");
		    			printf("%s\n",x);
	    		}
//	    printf("blah1");
    		}
    		else if (execvp(args[0],args) == -1)
			  printf("Error in command\n");
//	printf("hi\n");
    		exit(EXIT_FAILURE);
  	}
  	else if (pid == -1)
    		printf("Error in forking\n");
// closing file descriptors
	if (i == 0)
		close(fd1[1]);
	else if ( i == npipes )
	{
		if(i % 2 == 0)
			close(fd[0]);
		else
			close(fd1[0]);
	}
	else
	{
		if(i % 2 == 1)
		{
			close(fd[1]);
			close(fd1[0]);
		}
		else
		{
			close(fd[0]);
			close(fd1[1]);
		}
	}

	waitpid(pid,NULL,0);
//  	else
//  	{

//	  	for(j = 0; j<=npipes ; j++)
//         	{

//			if(!background)
//			{
/*		  		do {
      					wpid = waitpid(pid, &status, WUNTRACED);			//parent process waits for child process to finish executing the command
    			} while (!WIFEXITED(status) && !WIFSIGNALED(status));*/	//waits till a process is either exited or killed
//	  	  	}
//  	}

	}
}
else				//input/output redirection
{
  pid = fork();					//creates duplicate copy(child process) of original prog(parent process);
//  int pipes[2*npipes];
//  printf("out");
//  printf("%s\n",pid);
  if (pid == 0) 				//Child process
  {
	if(redirect_output == 1)
	{
		int f;
		if(append == 2)
			f = open(last,O_APPEND|O_WRONLY|O_CREAT,0600);		//permissions?
		else
		{
			off_t off = 0;
			truncate(last,off);
			f = open(last,O_WRONLY|O_CREAT,0600);
		}
		dup2(f,1);
		close(f);
	}

	if(redirect_input == 1)
	{
		int f1 = open(args2[0],O_RDONLY|O_EXCL,0600);    // file doesnt exist?
//			printf("hehe\n");
		if(f1 == -1)
		{
			printf("File/Directory doesnt exist\n");
			exit(0);
		}
		else
		{
			dup2(f1,0);
			close(f1);
		}
	}
//	  printf("in");
//	  printf("%s\n",args[0]);
//	  printf("%c\n",args[1][0]);
	if(background == 1)
	{
//		printf("in\n");
		setpgid(0,0);
	}

    if (strcmp(args[0],"echo") == 0 && args[1][0] == '$')   //echo: environment variables
    {
//	    printf("blah");
	    if(strcmp(args[1],"$HOME")==0)
		    printf("%s\n",home);
	    else
	    {
		    char s[200];
	    for(i=0;args[1][i]!='\0';i++)
		    s[i]=args[1][i+1];
	    char *x=getenv(s);
	    if(x!=NULL)
//		    printf("yes");
		    printf("%s\n",x);
	    }
//	    printf("blah1");
    }
    else if (execvp(args[0],args) == -1)
		  printf("Error in command\n");
//	printf("hi\n");
    exit(EXIT_FAILURE);
  }
  else if (pid == -1)
    	printf("Error in forking\n");
  else 							//forking successful
  {
	  int i,flag;
//	  printf("haha\n");
//	  printf("%d\n",pid);
//	  count++;
//	  p[count] = pid;
	  p = pid;
	  strcpy(st, args[0]);
//	  printf("%d\n",p);
//	  if(background)
//	  	printf("[%d] %d\n",count,p);
		if(!background)
		{
		  do {
/*			  if(sigf == 1)
			  {
				  sigf = 0;
				  temp = head;
				  while(temp != NULL)
				  {
					  if(temp -> id == p)
					  {
						  temp -> state = 0;
						  return 1;
					  }
				  }
//				  printf("haha\n");
				  temp = (struct node *)malloc(sizeof(struct node *));
				  temp -> id = pid;
				  temp -> state = 0;
				  strcpy(temp -> com,st);
				  temp -> next = NULL;
				  if(head == NULL)
				  	head = temp;
				  else
					  end -> next = temp;
				  end = temp;
//				  kill(pid,SIGTSTP);
//				  setpgid(pid,pid);
//				  kill(0,SIGCONT);
//				  kill(pid,SIGSTOP);
//				  return 1;
				  break;*/
//			  }
//			  printf("lala\n");
      				wpid = waitpid(pid, &status, WUNTRACED);			//parent process waits for child process to finish executing the command
    		} while (!WIFEXITED(status) && !WIFSIGNALED(status));	//waits till a process is either exited or killed
//		  printf("hehe\n");
  		}
   }
}
  	return 1;
}

//Execute built-in commands or launch program.
int exec_command(char **args,char **args2)
{
//	printf("fefe");
  int i;
//	printf("bef");
  if(!npipes)
  {
	  if(strcmp(args[0],"cd")==0)
	  	cd(args);
  	else if(strcmp(args[0],"quit")==0)
	  ex(args);
//	printf("before f");
	  else if(strcmp(args[0],"jobs")==0)
		  jobs();
	  else if(strcmp(args[0],"kjob")==0)
		  kjob(args);
	  else if(strcmp(args[0],"fg")==0)
		  fg(args);
	  else if(strcmp(args[0],"overkill")==0)
		  overkill();
  	else
  		return sys_com(args,args2);
  }
  else
	  return sys_com(args,args2);
}

//split input into tokens using strtok(acc to delimiters)
char **split_input(char *line,char *str)
{
	n=0;
//	printf("hello");
	int i=0;
  char **tokens = malloc(100 * sizeof(char*)),*token;

  token = strtok(line, str);
  while (token != NULL) {
    tokens[i]=token;
    n++;
    i++;
//	  printf("hah");
/*    if(pflag == 0)
    {
	pflag = 1;
    	last = token;
    }*/
    token = strtok(NULL, str);
  }
  tokens[i] = NULL;
  return tokens;
}

/*displays prompt; takes user input and executes commands
  using calls to other functions. */

void shell_loop()
{
	//signal handling
      signal (SIGINT, sh1);
      signal (SIGTSTP, sh2);
      signal (SIGCHLD,SIG_IGN);

//	tcsetpgrp(0,gl);
//	printf("hi");
//char *line;
//	sp = getpid();
  char **args,g[2000],**args1,**args2;
//  char **pargs;
  char *inp_string=malloc(2000*sizeof(char));
  int i,k,index;
//  count = -1;
  do {
//	  sigf = 0;
	  int flag=0;
    char hname[1000],path[1000];
    gethostname(hname,sizeof(hname));
    char *user = getenv("USER");
    getcwd(path,1000);

    if (strlen(home) <=strlen(path))
    {
 	   for(i=0;home[i]!='\0';i++)
    	   {
	   	if(path[i]!=home[i])
	    	{
			flag=1;
			break;
	    	}
	   }
    	   if (flag ==0)
           {
	   	g[0]='~';
	    	for(k=i;path[k]!='\0';k++)
		    g[k-i+1] = path[k];
	    	g[k-i+1]='\0';
           }
    	   strcpy(path,g);
    }

    checkbp();
//	  if(background)
//	  	printf("[%d] %d\n",count,p);
    printf("<%s@%s:%s>",user,hname,path);
//	tcsetpgrp(0,gl);
	char c;
	i=0;
	int f = 0;
	while((c=getchar())!='\n')
	{
//		if(sigf == 1)
//			break;
		f = 1;
		inp_string[i]=c;
		i++;
	}
	inp_string[i]='\0';
//	if(sigf == 1)
//		continue;
	if(f != 1)
	{
		condition = 1;
		continue;
	}
//		inp_string
    args1 = split_input(inp_string,";");

//    printf("%s\n",args1[0]);

/*    for(i=0;args1[i]!='\0';i++)
	    puts(args1[i]);*/
//    printf("haha\n");
    int y;
    flag=0;
    for(index=0;args1[index]!='\0';index++)
    {
	    y=0;
	    for(i=0;args1[index][i]!='\0';i++)
	    {
		    if(!isspace(args1[index][i]))
			{	
				y=1;
				break;
			}
	    }
	    if(y == 0)
	    {
	    	    index++;
	    	    continue;
	    }
	redirect_output = 0;
	redirect_input = 0;
	background=0;
	    flag=1;
//	    pflag=0;
    line = args1[index];
    if(line[0] == '>' || line[strlen(line)-1] == '>' || line[0] == '<' || line[strlen(line)-1] == '<' || line[0] == '&')
    {
	    printf("Error in command\n");
	    condition = 1;
	    continue;
    }
//    puts(line);
//    char line1[1000];
    int flag1=0;
    flag = 0;
    append=0;
    npipes=0;
    int j=-1;
    for(i=0;i<strlen(line);i++)
    {
	line1[i] = line[i];
	if(flag1 == 1)
	{
		line2[i-k] = line[i];
	}
	if (line1[i] == '>')
	{
		j = i;
		append++;
		redirect_output = 1;
	}
	if (line1[i] == '<')
	{
		flag1=1;
		k = i+1;
		redirect_input = 1;
	}
	if(line1[i] == '|')
	{
		npipes++;
	}
	if(line1[i] == '&' && i < strlen(line)-1)
		flag = 1;

    }
    line1[i]='\0';
    if(flag == 1)
    {
	    printf("Error in command\n");
	    condition = 1;
	    continue;
    }
    int in;
    if(flag1 == 1)
    {
	    line2[i-k] = '\0';
	    if(j != -1 && npipes > 0)
	    {
		    xc = line1+j+1;
//		    puts(xc);
		    line2[j-k] = '\0';
		    args = split_input(xc," \t\r\a\n");
		    last = args[0];
	    }
	    for(in=0;line[in]!='<';in++)
		    line1[in]=line[in];
	    line1[in] = '\0';
	    strcpy(line,line1);
	    for(in=0;line2[in]!='\0';in++)
		    xb[in] = line2[in];
	    xb[in] = '\0';
    }
//    puts(line2);
	    if(line[i-1] == '&')
    	    {
		background = 1;
		line[i-1]='\0';
		line1[i-1]='\0';
//		count++;
    	    }
    for(i=0;line[i]!='\0';i++)
	    xa[i] = line[i];
    xa[i] = '\0';
//    printf("--\n");
//    puts(xa);
//    printf("--\n");
/*    if (npipes > 0)
    	pargs = split_input(line,"|");*/
    	    args = split_input(line," \t\r\n\a<>");
//    puts(args[0]);
    if(strlen(line1)>4 && strcmp(args[0],"cd")==0)
    {
//	    printf("1\n");
//	    if (line1[strlen(args[0])+1] == '~' && line1[strlen(args[0])+2] == '/')
	    if(args[1][0] == '~' && args[1][1] == '/')
	    {
//		    printf("2\n");
		    strcpy(line1,args[0]);
		    strcat(line1," ");
		    strcat(line1,home);
		    for(i=0;args[1][i]!='\0';i++)
			    line1[strlen(args[0])+strlen(home)+1+i] = args[1][i+1];
		    args = split_input(line1," \t\r\n\a<>");
	    }
    }

    if(redirect_input == 1)
	    args2 = split_input(line2," \t\r\n\a<>");

    if(redirect_output == 1)
    {
	    if(redirect_input == 1)
	    {
		    if(npipes == 0)
		    	last = args2[n-1];
		    args2[n-1] = NULL;
	    }
	    else
	    {
		last = args[n-1];
		args[n-1] = NULL;
	    }
    }
//    printf("ajja\n");
//    printf("%d\n",gl);
//    if (strcmp(args[0],"jobs")==0)
//	    printf("%d\n",background);
//    if(npipes > 0)
//	    line2[j] = '\0';
/*    if(redirect_input == 1)
    {
	    args2 = split_input(line2," \t\r\n\a");
//	    puts(args2[0]);

    }*/
/*    if (args[(sizeof(**args)/sizeof(*args))-1][0] == '&')
    {
	background=1;
	args[(sizeof(**args)/sizeof(*args))-1] = NULL;
    }*/
/*    for(i=0;args[i]!='\0';i++)
	    puts(args[i]);*/
    condition = exec_command(args,args2);	//condition=0 => exit shell_loop()
/*    for(i=1;i<NSIG;i++)
    {
   	    printf("--\n");
	printf("%s\n",sys_signame[i]);
	printf("--\n");
    }
    */
    //add job to background processes linked list
    if(background)				
    {
	    	count++;
		temp = (struct node *)malloc(sizeof(struct node *));
		temp -> id = p;
//		printf("%d\n",temp->id);
		strcpy(temp -> com,st);
//		temp -> state = 1;
//		temp -> com[i] = NULL;
//		puts(temp->com);
//		temp -> cont = 0;
		temp -> next = NULL;
		if(head == NULL)
			head = temp;
		else
			end -> next = temp;
		end = temp;
		printf("[%d] %d\n",count,temp -> id);
//		free(temp);
    }
    if(condition == 0)
	    break;
    }
  } while (condition);
  free(inp_string);
  free(args1);
}

int main()
{
//	printf("--\n");
//       	printf("%d\n",gl);
//	printf("--\n");
//	tcsetpgrp(0,gl);
//	int x = tcgetpgrp(0);
//	 printf("___\n%d\n___\n",x);
/*      signal (SIGINT, SIG_IGN);
      signal (SIGQUIT, SIG_IGN);
      signal (SIGTSTP, SIG_IGN);
      signal (SIGTTIN, SIG_IGN);
      signal (SIGTTOU, SIG_IGN);
      signal (SIGCHLD, SIG_IGN);*/
//  sp = getpid();
  getcwd(home,1000);
  shell_loop();
  return 0;
}
