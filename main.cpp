#include <iostream>
#include <Windows.h>
#include <MMSystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>
#include <time.h>
#include "extra.h"

void mainMenu(char nom[15]);
void leePuntuaciones();
int mainJuego(char nom[15],int *pts);
void creaMalla(TCabeza *Cab);
void cargaSprites(TSprite sprj[7][2],TSprite spre[],TSprite sprsword[],TSprite sprenem[7][2],TSprite HUD[2],TSprite sprnavi[2]);
void cargaMapa(TCabeza Cab,TJugador **jugador,TMapa *mapaux);
void actualiza(TJugador **jugador,TCabeza *Cab,TMapa *mapaux,TEnemigo **CabEnem);
int actualizaJugador(TJugador **jugador);
void actualizaNavi(THada **navi);
void actualizaMapa(TMapa mapaux,TCabeza *Cab,TEnemigo **CabEne,int dir);
int insertaEnemigo(TEnemigo **Cab,int info,TNodoMalla *pos);
void actualizaEnemigos(TEnemigo **CabEnem,TJugador *jugador);
int eliminaEnemigo(TEnemigo *enemigo,TEnemigo **Cab);
void eventos(TCabeza Cab);
void eventosJugador(TJugador *jugador);
void eventosEnemigo(TEnemigo *enemigo);
void dibujar(TJugador *jugador,TSprite escenario[],TCabeza Cab,TCabezaEnemigo CabE,TSprite HUD[]);
void dibujaMapa(TNodoMalla *Cab,TSprite escenario[]);
void dibujaJugador(TJugador *jugador,TSprite HUD[]);
void dibujaEnemigos(TCabezaEnemigo CabE);
void finDJuego(char nom[15],int band,int pts);

void captura_texto(int x,int y, char cadena[]);
void mensaje(char msj[50]);

int main()
{
    char nombre[15];
    int bandera,puntos;
    initwindow(608,608);
    setfillstyle(10,15);

    setcolor(WHITE);

    while(true)
    {
        mainMenu(nombre);
        bandera=mainJuego(nombre,&puntos);
        finDJuego(nombre,bandera,puntos);
    }
    getch();

    return 0;
}

void mainMenu(char nom[15])
{
    Reproduce("Sonidos/Intro.wav");
    setbkcolor(BLACK);
    setcolor(WHITE);
    TBoton jugar_btn,score_btn,ayuda_btn,atras_btn,salir_btn;
    TBoton botones[5]={jugar_btn,score_btn,ayuda_btn,atras_btn,salir_btn};
    inicializaBotones(botones);
    int ratonx=0,ratony=0,opc=-1,i;

    readimagefile("Menu/Menu1.jpg",0,0,608,608);
    for(i=0;i<5;i++)
        if(i!=3)dibuja_boton(botones[i]);

    while(opc!=0)
    {
        opc=-1;
        if(ismouseclick(WM_LBUTTONDOWN))
        {
            getmouseclick(WM_LBUTTONDOWN,ratonx,ratony);//Obtiene las coordenadas
            opc=click_boton(botones,ratonx,ratony);//Checa si se dio un click en algun boton, y regresa la opcion que se escogio
        }
        switch(opc)
        {
            case 1:
                readimagefile("Menu/Menu2.jpg",0,0,608,608);
                leePuntuaciones();
                dibuja_boton(botones[3]);
                break;
            case 2:
                readimagefile("Menu/Menu3.jpg",0,0,608,608);
                dibuja_boton(botones[3]);
                break;
            case 3:
                readimagefile("Menu/Menu1.jpg",0,0,608,608);
                for(i=0;i<5;i++)
                    if(i!=3)dibuja_boton(botones[i]);
                break;
            case 4:
                exit(1);
                break;
        }

    }
    Reproduce("Sonidos/espada.wav");
    readimagefile("Menu/Menu4.jpg",0,0,608,608);
    captura_texto(100,608/2,nom);
}

int mainJuego(char nom[15],int *pts)
{
    setbkcolor(WHITE);
    TCabeza Cabeza;
    TCabezaEnemigo CabEnemigos;
    CabEnemigos.CabPrim=NULL;
    TJugador *jugador;
    Cabeza.Cabprim=Cabeza.CabSec=NULL;
    TMapa mapeo;

    TSprite escenario[9];
    TSprite HUD[2];
    int v=0,band;
    setactivepage(1);

    creaMalla(&Cabeza);
    jugador=CreaJugador(Cabeza,nom);
    cargaSprites(&jugador->sprite[0],escenario,&jugador->espada->sprite[0],&CabEnemigos.sprite[0],HUD,&jugador->navi->sprite[0]);
    cargaMapa(Cabeza,&jugador,&mapeo);
    while(jugador->vidas>0 /*|| jefe==muerto*/)
    {
        jugador->tiempoat.x=time(0);

        setactivepage(v);
        cleardevice();
        actualiza(&jugador,&Cabeza,&mapeo,&CabEnemigos.CabPrim);
        eventos(Cabeza);
        dibujar(jugador,escenario,Cabeza,CabEnemigos,HUD);
        delay(16);
        setvisualpage(v);

        v=!v;

        jugador->tiempoat.y=time(0);
        jugador->tiempoat.tiempot+=jugador->tiempoat.y-jugador->tiempoat.x;
    }
    if(jugador->vidas<=0 && jugador->llaves<5)
        band=0;
    else band=1;
    *pts=jugador->puntos;
    //destructor(Cabeza,jugador,CabEnemigos);
    return band;
}

void finDJuego(char nom[15],int band,int pts)
{
    setcolor(WHITE);
    setbkcolor(BLACK);
    FILE *a;
    char aux[50];
    char auxpuntos[5];
    sprintf(auxpuntos,": %d",pts);
    if(band==0)readimagefile("Menu/fin1.jpg",0,0,608,608);
    else
    {
        readimagefile("Menu/fin2.jpg",0,0,608,608);
        outtextxy(608/2-((textwidth(auxpuntos))/2),608/2,auxpuntos);
        strcat(nom,auxpuntos);
        strcpy(aux,nom);
        a=fopen("Puntos/score.txt","a");
        fprintf(a,"\n");
        fputs(aux,a);
        fclose(a);
    }
    getch();
    getch();

}

void eventos(TCabeza Cab)
{
    TNodoMalla *aux1=Cab.Cabprim,*aux2=Cab.Cabprim;
    int i,j;
    for(i=0;i<N && aux1;i++,aux1=aux1->abajo,aux2=aux1)
    {
        for(j=0;j<M && aux2;j++,aux2=aux2->der)
        {
            if(aux2->jugador)eventosJugador(aux2->jugador);
            else if(aux2->enemigo)eventosEnemigo(aux2->enemigo);
        }
    }
}

void eventosJugador(TJugador *jugador)
{
    if(jugador->pos->obj_id>3)jugador->pos->jugador=NULL;
    if(jugador->pos->enemigo && jugador->tiempoat.tiempot>2)
    {
        jugador->tiempoat.tiempot=0;
        jugador->vidas-=1;
        jugador->pos->jugador=NULL;
        Reproduce("Sonidos/jugadorherido.wav");
        switch(jugador->pos->enemigo->dir)
        {
        case 0:
            if(jugador->pos->abajo)
                jugador->pos=jugador->pos->abajo;
            break;
        case 1:
            if(jugador->pos->arriba)
                jugador->pos=jugador->pos->arriba;
            break;
        case 2:
            if(jugador->pos->izq)
                jugador->pos=jugador->pos->izq;
            break;
        case 3:
            if(jugador->pos->der)
                jugador->pos=jugador->pos->der;
            break;
        }
        jugador->pos->jugador=jugador;
    }
    if(jugador->espada->pos)Reproduce("Sonidos/espada.wav");
    if(jugador->pos->obj_id==3 && jugador->llaves<5)
    {
        Reproduce("Sonidos/error.wav");
        jugador->pos->jugador=NULL;
        jugador->pos=jugador->pos->abajo;
        jugador->pos->jugador=jugador;
    }
    if(jugador->pos->obj_id==2)
    {
        jugador->espada->band=1;
        Reproduce("Sonidos/item.wav");
        jugador->pos->obj_id=5;
    }
}

void eventosEnemigo(TEnemigo *enemigo)
{
    srand(time(0));
    if(enemigo->pos->espada)
    {
        enemigo->muerto=1;
        enemigo->id=6;
        enemigo->pos->espada->jugador->puntos+=10;
        Reproduce("Sonidos/enemigoatacado.wav");
        if(rand()%10==1)
        {
            enemigo->pos->espada->jugador->llaves+=1;
            Reproduce("Sonidos/llave.wav");
        }else if(rand()%20==5)
        {
            enemigo->pos->espada->jugador->vidas+=1;
            Reproduce("Sonidos/corazon.wav");
        }

        enemigo->pos->enemigo=NULL;
        switch(enemigo->pos->espada->jugador->dir)
        {
        case 0:
            if(enemigo->pos->abajo)enemigo->pos=enemigo->pos->abajo;
            break;
        case 1:
            if(enemigo->pos->arriba)enemigo->pos=enemigo->pos->arriba;
            break;
        case 2:
            if(enemigo->pos->izq)enemigo->pos=enemigo->pos->izq;
            break;
        case 3:
            if(enemigo->pos->der)enemigo->pos=enemigo->pos->der;
            break;
        }

    }
}

void dibujar(TJugador *jugador,TSprite escenario[],TCabeza Cab,TCabezaEnemigo CabE,TSprite HUD[])
{
    dibujaMapa(Cab.Cabprim,escenario);
    dibujaEnemigos(CabE);
    dibujaJugador(jugador,HUD);
}

void dibujaEnemigos(TCabezaEnemigo CabE)
{
    TEnemigo *aux;

    for(aux=CabE.CabPrim; aux; aux=aux->sig)
        putimage(aux->x,aux->y,CabE.sprite[aux->id][aux->dir2].imagen,3);
}

void dibujaMapa(TNodoMalla *Cab,TSprite escenario[])
{
    TNodoMalla *aux1=Cab,*aux2=Cab;
    int i,j;
    for(i=0;i<N && aux1;i++)
    {
        for(j=0;j<M && aux2;j++)
        {
            if(aux2->obj_id>=2)//Evitar esquinas blancas
                putimage(aux2->x,aux2->y,escenario[0].imagen,3);
            putimage(aux2->x,aux2->y,escenario[aux2->obj_id].imagen,3);
            aux2=aux2->der;
        }
        aux1=aux1->abajo;
        aux2=aux1;
    }
}

void dibujaJugador(TJugador *jugador,TSprite HUD[])
{
    int i;
    char auxpuntos[15];
    sprintf(auxpuntos,"%d ",jugador->puntos);

    putimage(jugador->x,jugador->y,jugador->sprite[jugador->dir][jugador->dir2].imagen,3);
    putimage(jugador->navi->x,jugador->navi->y,jugador->navi->sprite[jugador->navi->dir2].imagen,3);
    setcolor(BLACK);

    putimage(608-82,0,HUD[0].imagen,0);
    putimage(0,0,HUD[1].imagen,0);
    settextstyle(2, HORIZ_DIR, 8);
    outtextxy(32,32+5,auxpuntos);
    putimage(0,32,jugador->sprite[6][0].imagen,3);

    putimage(608-80,2,jugador->sprite[4][0].imagen,3);
    sprintf(auxpuntos,"x%d ",jugador->llaves);
    outtextxy(608-48,7,auxpuntos);

    for(i=0;i<jugador->vidas;i++)
        putimage(0+(i*32),0,jugador->sprite[5][0].imagen,3);

    if(jugador->espada->pos)
        putimage(jugador->espada->pos->x,jugador->espada->pos->y,jugador->espada->sprite[jugador->dir].imagen,3);
}

void actualiza(TJugador **jugador,TCabeza *Cab,TMapa *mapaux,TEnemigo **CabEnem)
{
    int recarga;

    recarga=actualizaJugador(jugador);

    if(recarga==1)
    {
        mapaux->limites.i=(*jugador)->cam.i;
        mapaux->limites.j=(*jugador)->cam.j;
        mapaux->limites.k=(*jugador)->cam.k;
        mapaux->limites.m=(*jugador)->cam.m;
        actualizaMapa(*mapaux,Cab,CabEnem,(*jugador)->dir);
    }
    actualizaEnemigos(CabEnem,*jugador);
}

void actualizaEnemigos(TEnemigo **CabEnem,TJugador *jugador)
{
    TEnemigo *aux;
    if(*CabEnem!=NULL)
    {
        srand(time(0));
        for(aux=*CabEnem;aux;aux=aux->sig)
        {
            if(aux->muerto==1 && (aux->x==aux->pos->x && aux->y==aux->pos->y))
                eliminaEnemigo(aux,CabEnem);
            else
            {
                aux->dir=-1;
                if(aux->pos->x==aux->x && aux->pos->y==aux->y)
                    aux->dir=rand()%10;
                aux->pos->enemigo=NULL;
                switch(aux->dir)
                {
                case 0:
                    if(aux->pos->abajo && aux->pos->abajo->obj_id<4 && !aux->pos->abajo->enemigo)
                        aux->pos=aux->pos->abajo;
                    break;
                case 1:
                    if(aux->pos->arriba && aux->pos->arriba->obj_id<4 && !aux->pos->arriba->enemigo)
                        aux->pos=aux->pos->arriba;
                    break;
                case 2:
                    if(aux->pos->izq && aux->pos->izq->obj_id<4 && !aux->pos->izq->enemigo)
                        aux->pos=aux->pos->izq;
                    break;
                case 3:
                    if(aux->pos->der && aux->pos->der->obj_id<4 && !aux->pos->der->enemigo)
                        aux->pos=aux->pos->der;
                    break;
                default:
                    aux->dir2=!aux->dir2;
                    break;
                }
                aux->pos->enemigo=aux;
                if(aux->x!=aux->pos->x || aux->y!=aux->pos->y)
                {
                    if(aux->x<aux->pos->x)aux->x+=8;
                    else if(aux->x>aux->pos->x)aux->x-=8;
                    if(aux->y<aux->pos->y)aux->y+=8;
                    else if(aux->y>aux->pos->y)aux->y-=8;

                    aux->dir2=!aux->dir2;
                }
            }

        }
    }
}

int eliminaEnemigo(TEnemigo *enemigo,TEnemigo **Cab)
{
    if(enemigo->sig)
    {
        if(enemigo->ant)//Si esta enmedio
        {
            enemigo->sig->ant=enemigo->ant;
            enemigo->ant->sig=enemigo->sig;
        }else if(!enemigo->ant)//Es el primero
        {
            *Cab=enemigo->sig;
            enemigo->sig->ant=NULL;
        }
    }else
    {
        if(enemigo->ant)//Es el ultimo
        {
            enemigo->ant->sig=NULL;
        }else if(!enemigo->ant)//Es el unico
        {
            *Cab=NULL;
        }
    }
    enemigo->pos->enemigo=NULL;
    free(enemigo);
}

void actualizaMapa(TMapa mapaux,TCabeza *Cab,TEnemigo **CabEne,int dir)
{
    TNodoMalla *aux1,*aux2;
    int i,j;
    srand(time(0));
    for(i=mapaux.limites.i,aux1=Cab->Cabprim;i<mapaux.limites.k && aux1;i++,aux1=aux1->abajo)
    {
        for(j=mapaux.limites.j,aux2=aux1;j<mapaux.limites.m && aux2;j++,aux2=aux2->der)
        {
            aux2->obj_id=mapaux.mapaaux[i][j];
            if(rand()%500==10 && aux2->obj_id==0 && aux2->enemigo==NULL && aux2)//crear un enemigo
                insertaEnemigo(CabEne,rand()%5,aux2);
            if(aux2->obj_id>9)
                aux2->obj_id=0;
            if(aux2->enemigo!=NULL)
            {
                switch(dir)
                {
                case 0:
                    if(aux2->arriba)
                    {
                        aux2->arriba->enemigo=aux2->enemigo;
                        aux2->enemigo->pos=aux2->arriba;
                    }else aux2->enemigo->muerto=1;
                break;
                case 1:
                    if(aux2->abajo)
                    {
                        //aux2->abajo->enemigo=aux2->enemigo;
                        aux2->enemigo->pos=aux2->abajo;
                    }else aux2->enemigo->muerto=1;
                break;
                case 2:
                    if(aux2->der)
                    {
                        //aux2->der->enemigo=aux2->enemigo;
                        aux2->enemigo->pos=aux2->der;
                    }else aux2->enemigo->muerto=1;
                break;
                case 3:
                    if(aux2->izq)
                    {
                        aux2->izq->enemigo=aux2->enemigo;
                        aux2->enemigo->pos=aux2->izq;
                    }else aux2->enemigo->muerto=1;
                break;
                }
                aux2->enemigo->x=aux2->enemigo->pos->x;
                aux2->enemigo->y=aux2->enemigo->pos->y;
                aux2->enemigo=NULL;
            }
        }
    }
}

int actualizaJugador(TJugador **jugador)
{
    (*jugador)->dir2=0;
    int tecla,band=0,i;
    TNodoMalla *aux;
    if((*jugador)->espada->pos)
        (*jugador)->espada->pos->espada=NULL;
    (*jugador)->espada->pos=NULL;
    if(kbhit())
    {
        (*jugador)->dir2=1;
        (*jugador)->pos->jugador=NULL;
        tecla=getch();
        if(tecla==224)tecla=getch();
        switch(tecla)
        {
            case 77:
                (*jugador)->dir=3;
                for(i=0,aux=(*jugador)->pos;aux->der;aux=aux->der,i++);
                if(i<10 && (*jugador)->cam.m<57 && (*jugador)->cam.m-(*jugador)->cam.j==19 && (*jugador)->pos->der->obj_id<4)
                {
                    //(*jugador)->dir2=1;
                    (*jugador)->cam.m++;
                    (*jugador)->cam.j++;
                    (*jugador)->pos->jugador=NULL;
                    (*jugador)->pos=(*jugador)->pos->izq;
                    band=1;
                }
                if((*jugador)->pos->der && (*jugador)->pos->der->obj_id<4)
                (*jugador)->pos=(*jugador)->pos->der;
                break;

            case 75:
                (*jugador)->dir=2;
                for(i=0,aux=(*jugador)->pos;aux->izq;aux=aux->izq,i++);
                if(i<10 && (*jugador)->cam.j>0 && (*jugador)->cam.m-(*jugador)->cam.j==19 && (*jugador)->pos->izq->obj_id<4)
                {
                    //(*jugador)->dir2=1;
                    (*jugador)->cam.m--;
                    (*jugador)->cam.j--;
                    (*jugador)->pos->jugador=NULL;
                    (*jugador)->pos=(*jugador)->pos->der;
                    band=1;
                }
                if((*jugador)->pos->izq && (*jugador)->pos->izq->obj_id<4)
                    (*jugador)->pos=(*jugador)->pos->izq;
                break;

            case 72:
                (*jugador)->dir=1;
                for(i=0,aux=(*jugador)->pos;aux->arriba;aux=aux->arriba,i++);
                if(i<10 && (*jugador)->cam.i>0 && (*jugador)->cam.k-(*jugador)->cam.i==19 && (*jugador)->pos->arriba->obj_id<4)
                {
                    //(*jugador)->dir2=1;
                    (*jugador)->cam.k--;
                    (*jugador)->cam.i--;
                    (*jugador)->pos->jugador=NULL;
                    (*jugador)->pos=(*jugador)->pos->abajo;
                    band=1;
                }
                if((*jugador)->pos->arriba && (*jugador)->pos->arriba->obj_id<4)
                    (*jugador)->pos=(*jugador)->pos->arriba;
                break;
            case 80:
                (*jugador)->dir=0;
                for(i=0,aux=(*jugador)->pos;aux->abajo;aux=aux->abajo,i++);
                if(i<10 && (*jugador)->cam.k<57 && (*jugador)->cam.k-(*jugador)->cam.i==19 && (*jugador)->pos->abajo->obj_id<4)
                {
                    //(*jugador)->dir2=1;
                    (*jugador)->cam.k++;
                    (*jugador)->cam.i++;
                    (*jugador)->pos->jugador=NULL;
                    (*jugador)->pos=(*jugador)->pos->arriba;
                    band=1;
                }
                if((*jugador)->pos->abajo && (*jugador)->pos->abajo->obj_id<4)
                    (*jugador)->pos=(*jugador)->pos->abajo;
                break;
            case 122://Sí se presiono Z
                //condiciones para posicionar la espada
                if((*jugador)->espada->band!=0)
                {
                    (*jugador)->dir2=1;
                    if((*jugador)->dir==0 && (*jugador)->pos->abajo)(*jugador)->espada->pos=(*jugador)->pos->abajo;
                    else if((*jugador)->dir==1 && (*jugador)->pos->arriba)(*jugador)->espada->pos=(*jugador)->pos->arriba;
                    else if((*jugador)->dir==2 && (*jugador)->pos->izq)(*jugador)->espada->pos=(*jugador)->pos->izq;
                    else if((*jugador)->dir==3 && (*jugador)->pos->der)(*jugador)->espada->pos=(*jugador)->pos->der;
                }
                break;
        }
        (*jugador)->pos->jugador=(*jugador);
        if((*jugador)->espada->pos)
            (*jugador)->espada->pos->espada=(*jugador)->espada;
        tecla=0;
    }

    if((*jugador)->x!=(*jugador)->pos->x || (*jugador)->y!=(*jugador)->pos->y)
    {
        if((*jugador)->x<(*jugador)->pos->x)(*jugador)->x+=8;
        else if((*jugador)->x>(*jugador)->pos->x)(*jugador)->x-=8;

        if((*jugador)->y<(*jugador)->pos->y)(*jugador)->y+=8;
        else if((*jugador)->y>(*jugador)->pos->y)(*jugador)->y-=8;

        (*jugador)->dir2=!(*jugador)->dir2;
    }

    if((*jugador)->pos->obj_id>3)
    {
        switch((*jugador)->dir)
        {
            case 0:
                (*jugador)->pos=(*jugador)->pos->arriba;
                break;
            case 1:
                (*jugador)->pos=(*jugador)->pos->abajo;
                break;
            case 2:
                (*jugador)->pos=(*jugador)->pos->der;
                break;
            case 3:
                (*jugador)->pos=(*jugador)->pos->izq;
                break;
        }
    }

    actualizaNavi(&(*jugador)->navi);
    return band;
}

void actualizaNavi(THada **navi)
{

    switch((*navi)->jugador->dir)
    {
        case 0:
            if((*navi)->pos->arriba)
                (*navi)->pos=(*navi)->jugador->pos->arriba;
            break;
        case 1:
            if((*navi)->pos->abajo)
                (*navi)->pos=(*navi)->jugador->pos->abajo;
            break;
        case 2:
            if((*navi)->pos->der)
                (*navi)->pos=(*navi)->jugador->pos->der;
            break;
        case 3:
            if((*navi)->pos->izq)
                (*navi)->pos=(*navi)->jugador->pos->izq;
            break;
    }

    if((*navi)->x!=(*navi)->pos->x || (*navi)->y!=(*navi)->pos->y)
    {
        if((*navi)->x<(*navi)->pos->x)(*navi)->x+=8;
        else if((*navi)->x>(*navi)->pos->x)(*navi)->x-=8;

        if((*navi)->y<(*navi)->pos->y)(*navi)->y+=8;
        else if((*navi)->y>(*navi)->pos->y)(*navi)->y-=8;
    }
    (*navi)->dir2=!(*navi)->dir2;
}

void cargaMapa(TCabeza Cab,TJugador **jugador,TMapa *mapaux)
{
    int i,j;
    TNodoMalla *aux1,*aux2;
    FILE *a;

    mapaux->limites.i=mapaux->limites.j=0;
    mapaux->limites.k=N;
    mapaux->limites.m=M;
    a=fopen("Mapa/Mapa0.txt","r");

    for(i=0;i<57;i++)
        for(j=0;j<57;j++)
            fscanf(a,"%d",&mapaux->mapaaux[i][j]);

    for(i=0,aux1=Cab.Cabprim;i<19 && aux1;i++,aux1=aux1->abajo)
    {
        for(j=0,aux2=aux1;j<19 && aux2;j++,aux2=aux2->der)
        {
            aux2->obj_id=mapaux->mapaaux[i][j];
            if(aux2->obj_id==10)
            {
                (*jugador)->pos=aux2;
                (*jugador)->navi->pos=(*jugador)->pos->arriba;
                (*jugador)->x=aux2->x;
                (*jugador)->navi->x=(*jugador)->navi->pos->x;
                (*jugador)->y=aux2->y;
                (*jugador)->navi->y=(*jugador)->navi->pos->y;
                aux2->jugador=(*jugador);
                aux2->obj_id=0;
            }
        }
    }
    fclose(a);
}

int insertaEnemigo(TEnemigo **Cab,int info,TNodoMalla *pos)
{
    int ret=0;
    TEnemigo *nuevo,*aux;

    nuevo=creaEnemigo(info,pos);
    if(nuevo)
    {
        if(*Cab==NULL)
        {
            *Cab=nuevo;
        }else
        {
            for(aux=*Cab;aux->sig;aux=aux->sig);
            aux->sig=nuevo;
            nuevo->ant=aux;
        }
        ret++;
    }
    return ret;
}

void creaMalla(TCabeza *Cab)
{
    int i,j;
    TNodoMalla *aux1,*nuevo;
    for(i=0;i<N;i++)
    {
        aux1=creaNodo();
        Cab->Cabprim=aux1;
        for(j=0;j<M;j++)
        {
            nuevo=creaNodo();
            aux1->der=nuevo;
            nuevo->izq=aux1;

            if(Cab->CabSec)
            {
                Cab->CabSec->abajo=aux1;
                aux1->arriba=Cab->CabSec;
            }
            if(i==0)
                aux1->arriba=nuevo->arriba=NULL;
            if(i==N-1)
                aux1->abajo=nuevo->abajo=NULL;
            if(j==0)
                aux1->izq=NULL;
            if(j==M-1)
                aux1->der=NULL;

            aux1->x=j*32;
            aux1->y=i*32;

            aux1=nuevo;
            if(Cab->CabSec)Cab->CabSec=Cab->CabSec->der;
        }
        Cab->CabSec=Cab->Cabprim;
    }
    //Acomodando las cabezas, una en la esquina superior izquierda y la otra en la esquina inferior derecha
    for(;Cab->Cabprim->arriba;Cab->Cabprim=Cab->Cabprim->arriba);
    for(i=0,Cab->CabSec=Cab->Cabprim;Cab->CabSec->arriba && i<19;Cab->CabSec=Cab->CabSec->arriba, i++);
    for(i=0,Cab->CabSec;Cab->CabSec->abajo && i<18;Cab->CabSec=Cab->CabSec->abajo, i++);
    for(i=0,Cab->CabSec;Cab->CabSec->der && i<18;Cab->CabSec=Cab->CabSec->der, i++);
}

void cargaSprites(TSprite sprj[7][2],TSprite spre[],TSprite sprsword[],TSprite sprenem[7][2],TSprite HUD[2],TSprite sprnavi[2])
{
    int i,j=0,c=0;
    char nombre[15];
    char Direccion[30]={"Sprites/"};
    //For para cargar sprites del jugador
    for(i=0;i<7;i++)
    {
        for(j=0;j<2;j++)
        {
            strcat(Direccion,"Jugador/");
            buscaNombreSprite(nombre,c,strcat(Direccion,"ID.txt"));
            guardaSprite(&sprj[i][j],strcat(strcpy(Direccion,"Sprites/Jugador/"),nombre),32,32);
            printf("%s\n",nombre);
            strcpy(Direccion,"Sprites/");
            c++;
        }
    }

    //For para cargar sprites del escenario
    for(i=0;i<9;i++)
    {
        strcat(Direccion,"Escenario/");
        buscaNombreSprite(nombre,i,strcat(Direccion,"ID.txt"));
        printf("%s\n",nombre);
        guardaSprite(&spre[i],strcat(strcpy(Direccion,"Sprites/Escenario/"),nombre),32,32);
        strcpy(Direccion,"Sprites/");
    }

    //For para cargar sprites de la espada
    for(i=0;i<4;i++)
    {
        strcat(Direccion,"Jugador/");
        buscaNombreSprite(nombre,i,strcat(Direccion,"IDEspada.txt"));
        printf("%s\n",nombre);
        guardaSprite(&sprsword[i],strcat(strcpy(Direccion,"Sprites/Jugador/"),nombre),32,32);
        strcpy(Direccion,"Sprites/");
    }

    //For para cargar sprites de los enemigos
    c=0;
    for(i=0;i<7;i++)
    {
        for(j=0;j<2;j++)
        {
            strcat(Direccion,"Enemigos/");
            buscaNombreSprite(nombre,c,strcat(Direccion,"ID.txt"));
            guardaSprite(&sprenem[i][j],strcat(strcpy(Direccion,"Sprites/Enemigos/"),nombre),32,32);
            printf("%s\n",nombre);
            strcpy(Direccion,"Sprites/");
            c++;
        }
    }

    //For para cargar sprites de navi
    for(i=0;i<2;i++)
    {
        strcat(Direccion,"Jugador/");
        buscaNombreSprite(nombre,i,strcat(Direccion,"IDNavi.txt"));
        printf("%s\n",nombre);
        guardaSprite(&sprnavi[i],strcat(strcpy(Direccion,"Sprites/Jugador/"),nombre),32,32);
        strcpy(Direccion,"Sprites/");
    }

    readimagefile("Sprites/Jugador/llaves.jpg",0,0,160,36);
    HUD[0].imagentam=imagesize(0,0,159,35);
    HUD[0].imagen=malloc(HUD[0].imagentam);
    getimage(0,0,159,35,HUD[0].imagen);

    readimagefile("Sprites/Jugador/vidarupias.jpg",0,0,160,64);
    HUD[1].imagentam=imagesize(0,0,159,63);
    HUD[1].imagen=malloc(HUD[1].imagentam);
    getimage(0,0,159,63,HUD[1].imagen);

}
