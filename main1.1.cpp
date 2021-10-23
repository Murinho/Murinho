#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <queue>
#include <utility>
#include <vector>
#include "snake.h"
using namespace std;

const int mvx[]={-1,0,1,0};
const int mvy[]={0,1,0,-1};

const int n = 50;
int hisID=-1;
int tam = 2;

int mapita[50][50];
bool visit[50][50];
bool visit2[50][50];
int dist[50][50];

queue <pair <int,int> > q;

bool valid(int y, int x){
    if(!(x>=0&&x<50&&y>=0&&y<50)){return false;}
    if(mapita[y][x]!=VACIO&&mapita[y][x]!=COMIDA){return false;}
    if(visit[y][x]){return false;}
    return true;
}

bool valid2(int y, int x){
    if(!(x>=0&&x<50&&y>=0&&y<50)){return false;}
    if(mapita[y][x]!=VACIO&&mapita[y][x]!=COMIDA){return false;}
    if(visit2[y][x]){return false;}
    return true;
}

void bfs1(int y, int x){

    for(int i=0; i<n; i++) for(int i2=0; i2<n; i2++){
        visit[i][i2]=false;
        dist[i][i2]=0;
    }

    q.push({y,x});

    while(!q.empty()){
        y = q.front().first;
        x = q.front().second;
        q.pop();
        for(int i=0; i<4; i++){
            if(valid(y+mvy[i],x+mvx[i])){
                visit[y+mvy[i]][x+mvx[i]]=true;
                q.push({y+mvy[i],x+mvx[i]});
                dist[y+mvy[i]][x+mvx[i]]=dist[y][x]+1;
            }
        }
    }
}

int bfs2(int y, int x){
    for(int i=0; i<n; i++) for(int i2=0; i2<n; i2++){visit2[i][i2]=false;}

    q.push({y,x});

    int c=0;
    while(!q.empty()){
        y = q.front().first;
        x = q.front().second;
        q.pop();
        for(int i=0; i<4; i++){
            if(valid2(y+mvy[i],x+mvx[i])){
                visit2[y+mvy[i]][x+mvx[i]]=true;
                q.push({y+mvy[i],x+mvx[i]});
                c++;
            }
        }
    }

    return c;
}

int miJugada()
{
    int myID = mi_identificador();
    int my_size;

    if(hisID==-1){  //Busca id del oponente revisando en el mapa
        for(int i=0; i<n; i++) for (int i2=0; i2<n; i2++){
            int x = mapa(i2,i);
            if(x>0&&x!=myID){hisID=x;}
        }
    }

    vector<pair<pair<int,int>,pair<int,int>>> point(10);
    vector<pair<int,int>> ops(4);

    int my_y = fila_cabeza(myID);
    int my_x = columna_cabeza(myID);

    int op_y = fila_cabeza(hisID);
    int op_x = columna_cabeza(hisID);

    vector <pair<pair<int,int>,int>> points_cd;
    for (int i=0; i<n; i++) for (int i2=0; i2<n; i2++){  //Obtiene el mapa
        mapita[i][i2]=mapa(i2,i);
        if(mapita[i][i2]==myID){my_size++;}
        if(mapita[i][i2]==COMIDA){points_cd.push_back({{i,i2},0});}
    }

    for(int i=0; i<10; i++)for(int i2=0; i2<10; i2++){
            if(abs(points_cd[i].first.first - points_cd[i2].first.first) + abs(points_cd[i].first.second - points_cd[i2].first.second)<=10){points_cd[i].second++;}
    }

    bfs1(my_y,my_x);   //BFS para después poder calcular la distancia de cada punto a la snake

    //Sistema para calificar los puntos
    int ip=0;
    for (int i=0; i<n; i++) for (int i2=0; i2<n; i2++){ //i=y  i2=x
        if(mapita[i][i2]==COMIDA){
            int w = 0;
            w-=dist[i][i2];
            if((i2<5 && i<5) || (i2>43 && i>43) || (i2<5&&i>43) || (i<5&&i2>43)){w-=3;}  //Si el punto esta cerca de una esquina se le resta calificación
            if(dist[i][i2]>=(abs(i2-op_x)+abs(i-op_y))){w-=1;}  //si el oponente esta "más cerca" del punto se resta calificación (no es distancia real)
            w+=points_cd[ip].second/2;
            point[ip]={{w,dist[i][i2]},{i,i2}};
            if(dist[i][i2]>20&&abs(25-i)+abs(25-i2)>15){w-=4;}
            ip++;
        }

    }

    sort(point.begin(),point.end());   //Se ordena para obtener más facilmente el punto con mejor calificación

    bfs1(point[9].second.first, point[9].second.second);    //BFS para saber la distancia hacia el punto desde cada lugar del mapa

    //Calificación de cada posible movimiento
    for(int i=0; i<4; i++){

        int w = 0;
        int nx=my_x+mvx[i],ny=my_y+mvy[i];

        if(nx>=0&&nx<50&&ny>=0&&ny<50){  //Si ny y nx son coordenadas validas se le asignará calificación

            int posible = bfs2(ny,nx);   //bfs que determina cuantas posciciones libres rodean a la serpiente al hacer un movimiento

            //Se resta calificación en caso de haber pocas posiciones para evitar que se encierre sola
            if(posible<10){w-=999;}
            else if(posible<my_size/2){w-=200;}
            else if(posible<my_size){w-=100;}
            else if(posible==my_size){w-=5;}

            if(mapita[ny][nx]!=COMIDA && mapita[ny][nx]!=VACIO){w -= 99999;}  //Si la posicion no esta vacia o tiene comida se anula

            if(dist[ny][nx]<point[9].first.second){w+=5;}  //Si la nueva poscicion nos acerca más al punto óptimo se añade calificación
            if(mapita[ny][nx]==COMIDA){w+=5;}    //Si el nuevo punto contiene comida se da calificación extra

            int c=0;


            for(int i2=0; i2<4; i2++){   //Ve al rededor de la posicion a la que se va a mover cuantas casillas vacias hay

                if(ny+mvy[i2]>=0 && ny+mvy[i2]<50 && nx+mvx[i2]>=0 && nx+mvx[i2]<50){
                    int content=mapita[ny+mvy[i2]][nx+mvx[i2]];
                    if(content==COMIDA||content==VACIO){c++;}
                }

            }
            //Según la cantidad de casillas vacias resta calificación
            if(c==0){w-=999;}
            if(c<2){w-=8;}
            else if(c<3){w-=4;}
            if(c<3&&abs(nx-op_x)+abs(ny-op_y)>16){w+=2;}
            //if(nx==0||nx==49||ny==0||ny==49){w-=4;}

            int dop = abs(nx-op_x)+abs(ny-op_y);  //Calcula la distancia hacia la cabeza del enemigo y resta puntos en funcion de ella
            if(dop<=15){w-=3;}
            if(dop<=5){w-=8;}
            if(dop<=2){w-=40;}
            if(dop<=1){w-=400;}


            ops[i]={w,i};
        }else{
            ops[i]={-999999,0};
        }
    }

    sort(ops.begin(),ops.end());  //Ordena para saber cual es la mayor calificación

    return ops[3].second;   //Regresa la dirección de la mejor poscición

}

















































// Modificar esto puede causar error en tu snake
////////////////////////////////////////////////////

int main()
{
    if (!init())
    {
		fprintf (stderr,"ERROR!\n");
		return 0;
	}
    while (comunicando())
    {
		int res=miJugada();
		respuesta_del_jugador(res);
	}
    return EXIT_SUCCESS;
}


////////////////////////////////// = P
int init(void)
{
 myDll = LoadLibrary("Snakec.dll");
 if(!myDll)
    return 0;

 columna_cabeza = (INFO_TYPE_INT)GetProcAddress(myDll,"columna_cabeza");
 fila_cabeza    = (INFO_TYPE_INT)GetProcAddress(myDll,"fila_cabeza");
 esta_viva      = (INFO_TYPE_INT)GetProcAddress(myDll,"esta_viva");
 respuesta_del_jugador    = (INFO_TYPE_INT)GetProcAddress(myDll,"respuesta_del_jugador");
 if (columna_cabeza==NULL|| fila_cabeza==NULL|| esta_viva==NULL|| respuesta_del_jugador==NULL)
 {
 	fprintf (stderr,"ERROR\n");
 	return 0;
 }

 mi_identificador = (INFO_TYPE_SIMPLE)GetProcAddress(myDll,"mi_identificador");
 inicializa = (INFO_TYPE_SIMPLE)GetProcAddress(myDll,"inicializa");
 comunicando=(INFO_TYPE_SIMPLE)GetProcAddress(myDll,"comunicando");
 largo_mundo=(INFO_TYPE_SIMPLE)GetProcAddress(myDll,"largo_mundo");
 if (mi_identificador==NULL|| inicializa==NULL|| comunicando==NULL||largo_mundo==NULL)
 {
 	fprintf (stderr,"ERROR\n");
 	return 0;
 }


 dame_mapa =(INFO_TYPE_MAP)GetProcAddress(myDll,"dame_mapa");
 coordenadas_cabeza =  (INFO_TYPE_COOR)GetProcAddress(myDll,"coordenadas_cabeza");
 mapa =(INFO_TYPE_MAP_VAL)GetProcAddress(myDll,"mapa");
 if (dame_mapa==NULL|| coordenadas_cabeza==NULL|| mapa==NULL)
 {
 	fprintf (stderr,"ERROR\n");
 	return 0;
 }


 return 1;
}

