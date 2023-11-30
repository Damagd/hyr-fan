#ifndef EXTRA_H_INCLUDED
#define EXTRA_H_INCLUDED

#define N 19
#define M 19

typedef struct
{
    void *imagen;
    int imagentam;
}TSprite;

typedef struct
{
    int x;
    int y;
    int tiempot;
}TTiempo;

typedef struct
{
    int i;
    int j;
    int k;
    int m;
}TLimites;

typedef struct
{
    TLimites limites;
    int mapaaux[57][57];
}TMapa;

typedef struct
{
    int x_inicial;//coordenadas donde comenzara a dibujarse el boton
    int y_inicial;
    int x_final;//coordenadas donde terminara el boton
    int y_final;
    int color=0;//Esta variable solo sirve para guardar el color que se esta utilizando, para el visualizador de color
    int band=-1;//Esta variable identificada a cada uno de los botones
    char *texto[5]={"Jugar","Scores","Ayuda","Atras","Salir"};//El identificador band, decide tambien que texto va a imprimir para cada boton
}TBoton;

typedef struct nodo
{
    struct nodo *arriba;
    struct nodo *abajo;
    struct nodo *izq;
    struct nodo *der;
    struct nodoesp *espada;
    struct nodoj *jugador;
    struct nodoenemigo *enemigo;
    int x;
    int y;
    int obj_id;
}TNodoMalla;

typedef struct
{
    struct nodo *Cabprim;
    struct nodo *CabSec;
}TCabeza;

typedef struct nodoesp
{
    TSprite sprite[4];
    TNodoMalla *pos;
    int band;
    struct nodoj *jugador;
}TEspada;

typedef struct
{
    int x;
    int y;
    int dir2;
    TSprite sprite[2];
    TNodoMalla *pos;
    struct nodoj *jugador;
}THada;

typedef struct nodoj
{
    int x;
    int y;
    TNodoMalla *pos;
    TSprite sprite[7][2];
    int dir;
    int dir2;//caminando
    int vidas;
    int llaves;
    int puntos;
    char nombre[15];
    TLimites cam;
    TEspada *espada;
    THada *navi;
    TTiempo tiempoat;
}TJugador;

typedef struct nodoenemigo
{
    int x;
    int y;
    TNodoMalla *pos;
    struct nodoenemigo *sig;
    struct nodoenemigo *ant;
    int dir;
    int dir2;
    int muerto;
    int id;
}TEnemigo;

typedef struct
{
    TSprite sprite[7][2];
    TEnemigo *CabPrim;
}TCabezaEnemigo;

void inicializaBotones(TBoton *btns)
{
    settextstyle(2, HORIZ_DIR, 10);

    int i;
    for(i=0;i<5;i++)//Ciclo para acomodar los botones en medio de la pantalla
    {
        (btns+i)->band=i;
        (btns+i)->x_inicial=(608/2)-100;
        (btns+i)->y_inicial=((500/2)-20)+(70*i);

        (btns+i)->x_final=(608/2)+100;
        (btns+i)->y_final=((btns+i)->y_inicial)+60;

        if(i==3)
        {
            (btns+i)->y_inicial=((500/2)-20)+(80*i);
            (btns+i)->y_final=((btns+i)->y_inicial)+65;
        }
    }
}

void dibuja_boton(TBoton btn)
{
    rectangle(btn.x_inicial,btn.y_inicial,btn.x_final,btn.y_final);

    if(btn.band>=0)
    {
        outtextxy(btn.x_inicial+5,btn.y_inicial+5,btn.texto[btn.band]);
    }
}

int click_boton(TBoton *botones,int ratonx,int ratony)
{
    int i=0;

    for(;i<5;i++)
    {
        if(( ratonx>(*(botones+i)).x_inicial  &&  ratony>(*(botones+i)).y_inicial) && ( ratonx<(*(botones+i)).x_final  &&  ratony<(*(botones+i)).y_final) )
        {
            //Estas lineas hacen la funcion de cambiar el color del boton por un momento, para que el usuario sepa que si detecto su interacción
            setfillstyle(1,RGB(100,100,100));
            floodfill(ratonx,ratony,WHITE);
            delay(100);
            setfillstyle(1,0);
            floodfill(ratonx,ratony,WHITE);

            //Regresa el valor con el que se identifica el boton
            return ((*(botones+i)).band);
        }
    }
    return -1;
}

TNodoMalla *creaNodo()
{
    TNodoMalla *nuevo;
    if(nuevo)
    {
        nuevo=(TNodoMalla*)malloc(sizeof(TNodoMalla));
        nuevo->arriba=nuevo->abajo=nuevo->izq=nuevo->der=NULL;
        nuevo->x=nuevo->y=nuevo->obj_id=0;
        nuevo->espada=NULL;
        nuevo->enemigo=NULL;
        nuevo->jugador=NULL;
    }

    return nuevo;
}

TJugador *CreaJugador(TCabeza Cab,char nom[])
{
    TJugador *nuevo;
    nuevo=(TJugador *)malloc(sizeof(TJugador));
    nuevo->espada=(TEspada *)malloc(sizeof(TEspada));
    nuevo->navi=(THada *)malloc(sizeof(THada));
    nuevo->espada->band=0;
    nuevo->espada->pos=NULL;
    nuevo->espada->jugador=nuevo->navi->jugador=nuevo;
    nuevo->navi->dir2=0;
    nuevo->tiempoat.x=nuevo->tiempoat.y=nuevo->tiempoat.tiempot=nuevo->cam.i=nuevo->cam.j=nuevo->dir2=nuevo->dir=nuevo->llaves=nuevo->puntos=nuevo->x=nuevo->y=0;
    nuevo->pos=Cab.Cabprim;
    nuevo->vidas=5;
    nuevo->cam.k=N;
    nuevo->cam.m=M;
    strcpy(nuevo->nombre,nom);
    return nuevo;
}

TEnemigo *creaEnemigo(int info,TNodoMalla *pos)
{
    TEnemigo *nuevo;
    nuevo=(TEnemigo *)malloc(sizeof(TEnemigo));
    if(nuevo && pos)
    {
        nuevo->dir2=nuevo->dir=nuevo->muerto=0;
        nuevo->id=info;
        nuevo->sig=nuevo->ant=NULL;
        nuevo->x=pos->x;
        nuevo->y=pos->y;
        nuevo->pos=pos;
        pos->enemigo=nuevo;
    }
    return(nuevo);
}

void buscaNombreSprite(char nom[15],int contador,char Dir[30])
{
    FILE *a;
    int i;
    a=fopen(Dir,"r");

    for(i=0;i<=contador && !feof(a);i++)
    {
        fgets(nom,18,a);
    }
    fclose(a);
}

void guardaSprite(TSprite *spr,char nom[30],int tamx,int tamy)
{
    FILE *a;
    int i,j,e;
    for(i=0;nom[i]!='\0';i++);
    if(nom[i]=='\0')nom[i-1]='\0';

    a=fopen(nom,"r");

    fscanf(a,"%d",&e);
    fscanf(a,"%d",&e);

    for(i=0;i<tamx;i++)
    {
        for(j=0;j<tamy;j++)
        {
            fscanf(a,"%d",&e);
            putpixel(j,i,e);
        }
    }

    fclose(a);
    spr->imagentam=imagesize(0,0,tamx-1,tamy-1);
    spr->imagen=malloc(spr->imagentam);
    getimage(0,0,tamx-1,tamy-1,spr->imagen);
}

void Reproduce(char nombre[50])
{
    PlaySound(TEXT(nombre), NULL, SND_FILENAME | SND_ASYNC);
}

void captura_texto(int x,int y, char cadena[])
{
    char tecla, var;
    int contador=0;
    cadena[0]='\0';

    do{
        //Si no se presiona una tecla, se imprimira un guion definidamente
        do{  //párpadeo
			 setcolor(15);
			 outtextxy(x+textwidth(cadena),y,"_");
			 delay(50);
			 setcolor(0);
			 outtextxy(x+textwidth(cadena),y,"_");
			 delay(50);
			 setcolor(15);
      }while(!kbhit());

      tecla = getch();

      if(tecla==0)  // si es código extendido
         tecla=getch();
        if(tecla==8 && contador > 0)// Sí se presiono la tecla de borrar y el contador que controla la posicion de la cadena no esta en el principio
			{
				 setcolor(BLACK);
				 outtextxy(x,y,cadena);  // se borra en la pantalla
				 cadena[--contador]='\0';  //se borra ultimo caracter del arreglo
				 setcolor( WHITE );
				 outtextxy(x,y,cadena);
			}
		 else //En otro caso
			{
              if(tecla!=13)//Si la tecla no fue "enter", se extiende la cadena y se imprime
              {
				 cadena[contador++]=tecla;
				 cadena[contador]='\0';
				 outtextxy(x,y,cadena);
              }
            }
    }while(tecla!=13);//Mientras no se presione "enter"
}

void mensaje(char msj[50])
{
    settextstyle(3, HORIZ_DIR, 3);
    readimagefile("Menu/Mensaje.jpg",0,608-120,608,608);
    outtextxy(10,(608-120)+10,msj);
}

void leePuntuaciones()
{
    FILE *a;
    a=fopen("Puntos/score.txt","r");
    int i=1;
    char aux[30];
    do
    {
        fgets(aux,30,a);
        outtextxy(608/2-((textwidth(aux))/2),(textheight(aux)*i)+64,aux);
        i++;
    }while(!feof(a));

    fclose(a);
}

#endif // EXTRA_H_INCLUDED
