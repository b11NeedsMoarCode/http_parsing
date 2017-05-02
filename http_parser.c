#include <string.h>
#include <stdio.h>

#define nb_token 56
#define requetetest "GET / HTTP/1.1\r\nHost: www.site.com\r\nConnection: Close\r\nContent-type: application/x-www-form-urlencoded\r\nContent-Length: 33\r\n\r\n"
#define ISTCHAR(X) ('0'<=X && X <='9'||'A'<=X && X <='z'||X=='!'||X=='#'||X=='$'||X=='%'||X=='&'||X=='\''||X=='*'||X=='+'||X=='-'||X=='.'||X=='^'||X=='_'||X=='`'||X=='|'||X=='~')
#define ISPCHAR(X) ('0'<=X && X <='9'||'A'<=X && X <='z'||X=='-'||X=='.'||X=='_'||X=='~'||X=='!'||X=='$'||X=='&'||X=='\'' ||X=='(' || X==')'||X=='*'||X=='/'||X=='+'||X==','||X==';'||X=='='||X=='@')
#define ISDIGIT(X) ('0'<=X && X <='9')

char liste_token[][24]={"HTTP-message","start-line","request-line","method","request-target",
"origin-form","absolute-form","authority-form","asterik-form","absolute-path","segment",
"query","absolute-URI","scheme","hier-part","authority","userinfo","host","IPv4adress","reg-name",
"port","HTTP-version","HTTP-name","Content-Length-header","Transfer-Encoding-header","cookie-header",
"Host-header","Referer-header","User-Agent-header","Accept-Encoding-header","Connection-header",
"Content-Length","transfer-Encoding","transer-extension","cookie-string","cookie-pair","cookie-name"
"cookie-value","Host","Uri-host","Referer","absolute-URI","Partial-URI","relative-part","User-Agent"
"product","comment","media-range","Accept-params","Connection","connection-option","Accept-Encoding"
"codings","weight"}; //liste de tous les tokens

typedef struct{
  int taille;
  char* debut;
}champ;

champ info_token[nb_token]; //structure ou tous les couples pointeurs/tailles sont stockes pour chaque token

int search_header(char* token);
int parser(char* buf, unsigned int len,char *search,void (*callback)());
void getField(char* s,int len);

void clear (void){  //inutile, juste pour vider le buffer pour un scanf
  while ( getchar() != '\n' );
}

void recopie(char* debut, int taille, char* copie){ //recopie taille lettre de la chaine debut dans la chaine copie
  int i;
  for (i=0;i<taille;i++){
    copie[i]=debut[i];
  }
  copie[taille]='\0';
}

int main(){ // ici on fait les tests
  parser(requetetest,strlen(requetetest),"HTTP-name",getField);
  /*char testrecopiage[1000];
  for(int k=0;k<nb_token;k++){
    recopie(info_token[k].debut,info_token[k].taille,testrecopiage);
    printf("%s\n",testrecopiage);
    testrecopiage[0]='\0';
  }*/
  printf("%d\n",info_token[search_header("HTTP-name")].taille);
  char a[10];
  scanf("%s",&a);
  clear();
  return 0;
}

int parser(char* buf, unsigned int len,char* search,void (*callback)()){
  info_token[search_header("HTTP-message")].taille=len; // on rentre deja tous le message dans la structure
  info_token[search_header("HTTP-message")].debut=buf;
  info_token[search_header("start-line")].debut=buf; // on rentre le pointeur sur la 1ere ligne dans la structure
  int i=0; // parcours chaque lettre
  int j=search_header("method"); // parcours chaque element
  int erreur=0; // indique si il y a une erreur ( 0 pas d'erreur, 1 erreur)
  int indice_erreur=0; // indique l'indice ou il y a une erreur
  while(buf[i]!='\n'&&erreur==0){
    // on realise la verification syntaxique sur le token method
    if(j==search_header("method")){
      int taille=0; // on initialise la taille et on rentre le pointeur sur le 1ere caractere du token
      info_token[j].debut=&buf[i];
      while(ISTCHAR(buf[i])){ //tant que ce qu'on lit et du type TCHAR defini dans la grammaire abnf, on continue
        taille++;
        i++;
      }
      if(buf[i]==' '){ // on verifie le singlespace obligatoire
        info_token[j].taille=taille; // on stock la taille dans la structure
        j=search_header("request-target"); // si tout est bon on passe au token request-target
        i++;
      }
      else{ // cas d'erreur, et on fait le choix de renvoyer l'indice ou il y a une erreur ( on pourrait faire un tableau d'incice d'erreur et tout renvoyer)
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
    }
    // meme idée mais sur le token request-target
    if(j==search_header("request-target")){
      int taille=0;
      info_token[j].debut=&buf[i];
      if(buf[i]=='/'){ // verification du / en debut obligatoire
        i++;
        taille++;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
      while(ISPCHAR(buf[i])){ // caractere de type PCHAR
        taille++;
        i++;
      }
      if(buf[i]==' '){
        info_token[j].taille=taille;
        j=search_header("HTTP-version"); // on passe au token suivant
        i++;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
    }
    if(j==search_header("HTTP-version")){
      int taille=8; // ce token a une taille fixe
      info_token[j].debut=&buf[i];
      if(buf[i]=='H'&&buf[i+1]=='T'&&buf[i+2]=='T'&&buf[i+3]=='P'){
        i=i+4;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
      if(buf[i]=='/'){
        i++;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
      if(ISDIGIT(buf[i])){
        i++;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
      if(buf[i]=='.'){
        i++;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
      if(ISDIGIT(buf[i])){
        i++;
      }
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
      if(buf[i]=='\r'){
        info_token[j].taille=taille;
        i++;
      } // verification du bout de ligne
      else{
        erreur=1;
        if (indice_erreur==0){
          indice_erreur=i;
        }
      }
    }
    if(buf[i]!='\n'){
      erreur=1;
      if (indice_erreur==0){
        indice_erreur=i;
      }
    }
  }
  if(erreur==0){
    j=search_header("start-line"); //on stock finalement la taille de la 1ere ligne
    info_token[1].taille=i-1;
  }
  else{
    printf("Erreur dans la requete a l'indice : %d", indice_erreur);
    return indice_erreur;
  }
  //ici faire le traitement des autres lignes
  if(erreur==0){
    int a=search_header("request-line"); // on met à jour tous les tokens maintenant qu'on connait le reste
    int b=search_header("start-line");
    info_token[a].debut=info_token[b].debut;
    info_token[a].taille=info_token[b].taille;
    int indice=search_header(search); // on trouve l'indice du token recherche en parametre
    if(indice!=-1){ // si on le trouve, on appelle la fonction de callback passe en parametre
      if(info_token[indice].taille!=0){
        char testrecopiage[1000];
        callback(info_token[indice].debut,info_token[indice].taille);
      }
      else{
        printf("Ce token n'est pas supporte ou absent de la requete.\n");
      }
    }
    else{
      printf("Ce token n'est pas valide :%s\n",search); // sinon le token est pas valide
    }
    return -1;
  }
}

int search_header(char* token){ // parcour dans le tableau des noms
  char testrecopiage[1000];
  for(int i=0;i<nb_token;i++){
    if(strcmp(token,liste_token[i])==0){
      return i; // renvoi l'indice
    }
  }
  return -1; // pas trouve
}

void getField(char* s,int len){ // une fonction de callback ( celle des profs)
  printf("Le parseur a trouve %.*s\n",len,s);
}
