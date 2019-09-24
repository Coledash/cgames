//
// Atlantis made by Jari Räsänen / Coledash
//

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <iostream>

using namespace std;

#define max(a,b) (((a)>(b)) ? (a) : (b))
#define min(a,b) (((a)<(b)) ? (a) : (b))

#define MAX_X 75
#define MAX_Y 75
#define EARTH_SEED 2
#define MOUNTAIN_SEED 1
#define TOWNS 40
#define HOME 'O'
#define TOWN 'o'
#define SHIP 'L'
#define LAND '#'
#define MOUNTAIN '^'
#define WATER ','
#define NORTHWARD 'w'
#define EASTWARD 'd'
#define SOUTHWARD 's'
#define WESTWARD 'a'
#define GOASHORE 'x'
#define END 'q'
#define EMBARK 'e'

struct town
{
	long int people;
	long int gold;
	long int pvm;
	long int food;
	long int x;
	long int y;
	long int friendliness;
	int townid;
	int jewelry;
}ky[TOWNS];

struct city
{
	long int jewelry;
	long int ships;
	long int men;
	long int pvm;
	long int food;
	long int x;
	long int y;
}home;

struct player
{
	char forbidden;
	long int jewelry;
	long int gold;
	long int ships;
	long int men;
	long int food;
	long int x;
	long int y;
}p,l;

char m[MAX_X][MAX_Y];
int pvm=0; // Date is still somewhat experimental.

int gameover() // Not implemented yet.
{
	exit(0);
}

void removecursor()
{
	CONSOLE_CURSOR_INFO info;
	HANDLE hOutput = GetStdHandle (STD_OUTPUT_HANDLE);
	
	//Turn the cursor off
	info.bVisible=FALSE;
	info.dwSize=1;
	if(SetConsoleCursorInfo(hOutput,&info) == 0 )
	{
		cout << endl << "SetConsoleCurInfo failed" << endl;
		DWORD dwError = GetLastError();
		char buf[255];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwError,
		0, buf, sizeof(buf),0);
		cout << buf << endl;
	}
	
	// cout << "Press ENTER key when ready.";
	// cin.ignore();
}

int random(long int numbar)
{
	return rand() % numbar;
}

void clrscr()
{
	system("CLS");
}

void gotoxy(int x, int y)
{
COORD coord;
coord.X = x;
coord.Y = y;
SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void txtcolor(char wantedcolor)
{
	HANDLE colorchanger = GetStdHandle(STD_OUTPUT_HANDLE);
	switch(wantedcolor)
	{
	case 'G':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_GREEN);
	break;
	
	case 'B':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	break;
	
	case 'R':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_RED);
	break;
	
	case 'W':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	break;
	
	case 'M':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_BLUE | FOREGROUND_RED);
	break;
	
	case 'Y':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_GREEN | FOREGROUND_RED);
	break;
	
	case 'C':
	SetConsoleTextAttribute(colorchanger, FOREGROUND_GREEN | FOREGROUND_BLUE);
	break;
	}
	
}

void find_place(long int *x, long int *y, char terrain)
{
	do
	{
		*x=random(MAX_X-1)+1;
		*y=random(MAX_Y-1)+1;
	}while(m[*x][*y]!=terrain);
}

void find_terrain(int *x, int *y, char terrain)
{
	do
	{
		*x=random(MAX_X-1)+1;
		*y=random(MAX_Y-1)+1;
	}while(m[*x][*y]!=terrain);
}

int cargo(char forbidden_terrain)
{
	if(forbidden_terrain==WATER) return p.jewelry+p.food;
	else return p.jewelry+p.food;
}

void lift_earth(int x, int y, int certainty)
{
	if(x>=0 && y>=0 && x<MAX_X && y<MAX_Y && m[x][y]==WATER && random(100)<certainty)
	{
		m[x][y]=LAND;
		lift_earth(x,y-1,certainty-1);
		lift_earth(x-1,y,certainty-1);
		lift_earth(x,y+1,certainty-1);
		lift_earth(x+1,y,certainty-1);
	}
}

void lift_mountain(int x, int y, int certainty)
{
	if(x>=0 && y>= 0 && x<MAX_X && y<MAX_Y && m[x][y]==LAND && random(100)<certainty)
	{
		m[x][y]=MOUNTAIN;
		lift_mountain(x,y-1,certainty-5);
		lift_mountain(x-1,y,certainty-5);
		lift_mountain(x,y+1,certainty-5);
		lift_mountain(x+1,y,certainty-5);
	}
}

void create_world()
{
	int i,j;
	int mo,mox,moy;
	clrscr();
	cout<<"CREATING THE WORLD...\n\r";
	for(j=0;j<MAX_Y;j++) for(i=0;i<MAX_X;i++) m[i][j]=WATER;
	for(i=0;i<EARTH_SEED;i++) lift_earth(random(MAX_X),random(MAX_Y),100);
	for(mo=0;mo<MOUNTAIN_SEED;mo++)
	{
	find_terrain(&mox,&moy,LAND);
	lift_mountain(mox,moy,100);
	}
}

void create_homecity()
{
	cout<<"SEARCHING HOMETOWN...\n\r";
	find_place(&home.x,&home.y,WATER);
	m[home.x][home.y]=HOME;
	m[home.x-1][home.y-1]=LAND;
	m[home.x-1][home.y+1]=LAND;
	m[home.x+1][home.y-1]=LAND;
	m[home.x+1][home.y+1]=LAND;
	home.jewelry=2000;
	home.ships=20;
	home.men=500;
	home.pvm = 0;
	home.food=5000;
}

void create_town()
{
	int i;
	cout<<"PLACING NATIVE VILLAGES...\n\r";
	for(i=0;i<TOWNS;i++)
	{
		ky[i].townid=i;
		ky[i].people=random(2000)+100;
		ky[i].gold=random(ky[i].people);
		ky[i].pvm=0;
		ky[i].food=random(ky[i].people);
		ky[i].friendliness=random(100);
		find_place(&ky[i].x,&ky[i].y,LAND);
		m[ky[i].x][ky[i].y]=TOWN;
	}
	
	cout << "\n\nReady to play! Press any key to continue...\n";
	getch();
}

void town_produce(town *ky) // Produce is not implemented yet.
{
	ky->gold+=10;
	
	ky->food+=10;
	
	ky->pvm=pvm;
}

void home_produce() // Produce is not implemented yet.
{
	home.ships=min( 20,home.ships+(pvm-home.pvm)/100);
	home.men=min( 500,home.men+(pvm-home.pvm));
	home.food=min(5000,home.food+(pvm-home.pvm)*10);
	home.jewelry=min(2000,home.jewelry+(pvm-home.pvm)*4);
	home.pvm=pvm;
}

int choose(int *direction,int lkm, int start)
{
	char command;
	int valinta=start;
	if(valinta>lkm) valinta=1;
	while(1)
	{
		gotoxy(5,3+2*valinta); cout<<"->";
		command=getch();
		gotoxy(5,3+2*valinta); cout<<"  ";
		switch(command)
		{
			case NORTHWARD: 
			if(valinta>1) valinta--;
			break;
			
			case SOUTHWARD: 
			if(valinta<lkm) valinta++;
			break;
			
			case EASTWARD: 
			*direction=-1;
			return valinta;
			
			case WESTWARD: 
			*direction=1;
			return valinta;
			
			case EMBARK: 
			return EMBARK;
		}
	}
}

int choose2(int *direction,int lkm,int start)
{
	char command;
	int valinta=start;
	if(valinta>lkm) valinta=1;
	while(1)
	{
		gotoxy(20,1+2*valinta); cout<<"<->";
		command=getch();
		gotoxy(20,1+2*valinta); cout<<"   ";
		switch(command)
		{
			case NORTHWARD: 
			if(valinta>1) valinta--;
			break;
			
			case SOUTHWARD: 
			if(valinta<lkm) valinta++;
			break;
			
			case EASTWARD: 
			*direction=-1;
			return valinta;
			
			case WESTWARD: 
			*direction=1;
			return valinta;
			
			case EMBARK: 
			return EMBARK;
		}
	}
}	

int reception(town *ky)  // Old style
{
	long int killed;
	int people_initially=ky->people;
	int i=25+random(25);
	
	while(1)
	{
		clrscr();
		
		if(i>ky->friendliness && ky->people > p.men)
		{
			p.men-=(killed=(random(p.men/8)));
			cout << ky->people/2 << " hostile native attack you!\n\r\n\r",ky->people;
			cout << killed << " of your men died! (" << p.men << " remaining)\n\r\n\r";
			
			if(p.men<1)
			{
				cout<<"Your men were butchered!\n\r\n\r";
				getch();
				exit(1);
				p.x=l.x;
				p.y=l.y;
				return 0;
			}
		}		
		
		else if(i>ky->friendliness)
			cout<<"Natives dont look too friendly.\n\r\n\r";
		
		else 
			cout<<"You are in a friendly native village.\n\r\n\r";
			cout<<"What do you want to do?\n\r\n\r";
			cout<<"A)ttack the natives.\n\r";
			cout<<"G)ive gifts to natives.(You have " << p.jewelry << " jewelry)\n\r";
			cout<<"T)rade.\n\r";
			cout<<"L)eave.\n\r";
			gotoxy(45,13);cout<<"Friendliness: " << ky->friendliness << "   ";
			gotoxy(45,15);cout<<"Population: " << ky->people << "   ";
		
		switch(getch())
		{
			case 'a':
			ky->people=(killed=min(ky->people,random(p.men)+1));
			ky->friendliness=0;
			if(random(100)>(200*ky->people/people_initially))
			{
				cout<<"\nNatives surrender. You can loot as much as you can carry.\n\r\n\r";
				getch();

				while(cargo(p.forbidden)<(p.ships*100) && ky->gold>0)
				{
					++p.gold,--ky->gold;
				}
				while(cargo(p.forbidden)<(p.ships*100) && ky->food>0)
				{
					++p.food,--ky->food;
				} 
				/* for(i=0;i<TOWNS;i++)
				{
					if(ky[i].friendliness>5) ky[i].friendliness-=5;
					else ky[i].friendliness=0;
				}
				Something is wrong here */
				return 0;
			}
			else
			{
				cout << "\nNatives refuse to surrender!";
				getch();
			}
			break;
			
			case 'g':
			if(p.jewelry>=10)
			{
				p.jewelry-=10;
				if(ky->friendliness<=95)
				ky->friendliness+=random(5)+1;
			}
			else 
			cout<<"Not enough jewelry!\n\r\n\r";
			break;
			
			case 't':
			if(i>ky->friendliness)
			cout<<"\nChieftain answers: 'Ei kauppoja ilman lahjoja!";
			else 
			{
				return 1;
				getch();
			}
			break;
			
			case 'l':
			return 0;
		}
	}
}

int towntrade(town *ky)
{
	clrscr();
	int exit=0,direction,product=1;

	gotoxy(10,1); cout << "You";
	gotoxy(30,1); cout << "Merchant";
	gotoxy(40,1); cout << "Price";
	gotoxy(1,3); cout << "Gold";
	gotoxy(1,5); cout << "Food";
	gotoxy(40,5); cout << "1";
	gotoxy(1,7); cout << "Jewelry";
	gotoxy(40,7); cout << "10";
	gotoxy(1,9); cout << "Leave";

	// gotoxy(45,11);cout<<"TownID: " << ky[i].townid << "   "; // Towns dont have unique names yet, just ID.
	// gotoxy(45,13);cout<<"Friendliness: " << ky->friendliness << "   ";
	// gotoxy(45,15);cout<<"Population: " << ky->people << "   ";
	
	do
	{
		gotoxy(10,3); cout << p.gold << "   ";
		gotoxy(10,5); cout << p.food << "   ";
		gotoxy(10,7); cout << p.jewelry << "   ";
		gotoxy(30,3); cout << ky->gold << "   ";
		gotoxy(30,5); cout << ky->food << "   ";
		// gotoxy(30,7); cout << ky->jewelry << "   ";
		
		gotoxy(10,18); cout << "Buttons: w,s,a,d";
		gotoxy(10,20); cout << "CURRENT CARGO " << cargo(p.forbidden) << " / " << p.ships*100 << "   ";
	
		switch(product=choose2(&direction,4,product))
		{
			case 1:
			break;
			
			case 2:
			if(direction==1 && p.food>0 && ky->gold>=1 || direction==-1 && p.gold>0 && cargo(p.forbidden<p.ships*100 && ky->food>0))
			{
				p.food-=direction;
				p.gold+=direction;
				ky->gold-=direction;
			}
			break;
			
			case 3:
			if(direction==1 && p.jewelry>0 && ky->gold>=10 || direction==-1 && p.gold>=4 && cargo(p.forbidden)<p.ships*100 && ky->jewelry>0)
			{
				p.jewelry-=direction;
				p.gold+=direction*4;
				ky->gold-=direction*4;
			}
			break;
			
			case 4:
			exit=1;
			break;
		}
	}while(!exit);
	clrscr();
}

void town2()
{
	int i=0,exit=0,direction,product=1;
	while(ky[i].x!=p.x || ky[i].y!=p.y) i++; // Finds the right town.
	
	do
	{
		clrscr();
		gotoxy(10,1); cout << "Native Village";
		gotoxy(30,1); cout << "Population: " << ky[i].people << "   ";
		gotoxy(55,1); cout << "Friendliness: " << ky[i].friendliness << "   ";
		gotoxy(10,5); cout << "Attack the village";
		gotoxy(10,7); cout << "Trade with a merchant";
		gotoxy(10,9); cout << "Leave town";
	
		switch(product=choose(&direction,3,product))
		{
			case 1:
			break;
			
			case 2:
			towntrade(&ky[i]);
			break;
			
			case 3:
			exit=1;
			break;
		}
	}while(!exit);
	clrscr();
}

void town() // Old style
{
	int i=0,exit=0,direction,product=1;
	while(ky[i].x!=p.x || ky[i].y!=p.y) i++; // Finds the right town.
	// Town produce is not implemented yet. town_produce(&ky[i]);
	
	if(reception(&ky[i]))
	{
		clrscr();
		gotoxy(20,1);cout<<"FRIENDLY NATIVE VILLAGE";
		gotoxy(20,3);cout<<"YOU HAVE / IN TOWN";
		gotoxy(10,5);cout<<"FOOD";
		gotoxy(10,7);cout<<"GOLD";
		gotoxy(10,9);cout<<"JEWELRY";
		gotoxy(45,5);cout<<"10 FOOD COSTS 1 JEWELRY";
		gotoxy(45,7);cout<<"4 GOLD COSTS 1 JEWELRY";
		gotoxy(45,9);cout<<"1 GIFT IS 10 JEWELRY";
		// gotoxy(45,11);cout<<"TownID: " << ky[i].townid << "   "; // Towns dont have unique names yet, just ID.
		gotoxy(45,13);cout<<"Friendliness: " << ky[i].friendliness << "   ";
		gotoxy(45,15);cout<<"Population: " << ky[i].people << "   ";
		
		do
		{
			gotoxy(20,5); cout << p.food << " / " << ky[i].food << "   ";
			gotoxy(20,7); cout << p.gold << " / " << ky[i].gold << "   ";
			gotoxy(20,9); cout << p.jewelry << "   ";
			gotoxy(10,18); cout << "Press e to leave";
			gotoxy(10,20); cout << "CURRENT CARGO " << cargo(p.forbidden) << " / " << p.ships*100 << "   ";
		
			switch(product=choose(&direction,3,product))
			{
				case 1:
				if((direction=1 && p.jewelry>=1 && ky[i].food>=10 && cargo(p.forbidden)<(p.ships*100)) || (direction==-1 && p.food>=10))
				{
					p.food+=direction*10;
					ky[i].food-=direction*10;
					p.jewelry-=direction;
				}
				break;
				
				case 2:
				if((direction==1 && p.jewelry>=1 && ky[i].gold>=4) || (direction==-1 && p.gold>=4))
				{
					p.gold+=direction*4;
					ky[i].gold-=direction*4;
					p.jewelry-=direction;
				}
				break;
				
				case 3:
				if(direction==-1 && p.jewelry>=10)
				{
					p.jewelry-=10;
					if(ky[i].friendliness<=95) ky[i].friendliness+=random(5)+1;
				}
				break;
				
				case EMBARK:
				// Max cargo is not monitored at the moment. if(cargo(p.forbidden)<=100) 
				exit=1;
				break;
			}
		}while(!exit);
	}
	clrscr();
}

void athome() // Player hometown
{
	int exit=0,direction,product=1;
	// Town produce is not implemented. home_produce();
	clrscr();
	gotoxy(20,1); cout<<"HOMETOWN";
	gotoxy(20,3); cout<<"YOU HAVE / IN TOWN";
	gotoxy(10,5); cout<<"SHIPS";
	gotoxy(10,7); cout<<"MEN";
	gotoxy(10,9); cout<<"FOOD";
	gotoxy(10,11); cout<<"JEWELRY";
	gotoxy(45,5); cout<<"1 SHIP COSTS 1000 GOLD";
	gotoxy(45,7); cout<<"1 MAN COSTS 10 GOLD";
	gotoxy(45,9); cout<<"10 FOOD COSTS 1 GOLD";
	gotoxy(45,11); cout<<"4 JEWELRY COSTS 1 GOLD";
	
	do
	{
		gotoxy(20,5); cout << p.ships << "  /  " << home.ships << "   ";
		gotoxy(20,7); cout << p.men << "  /  " << home.men << "   ";
		gotoxy(20,9); cout << p.food << "  /  " << home.food << "   ";
		gotoxy(20,11); cout << p.jewelry << "  /  " << home.jewelry << "   ";
		gotoxy(10,20); cout <<"CURRENT GOLD " << p.gold << "   ";
		gotoxy(10,21); cout <<"CURRENT CARGO " << cargo(p.forbidden) << " / " << p.ships*100 << "   ";
		gotoxy(10,22); cout <<"KEYS: w,a,s,d to move, q to quit and e to disembark";
		
		switch(product=choose(&direction,4,product))
		{
			case 1:
			if((direction==1 && p.gold>=1000 && home.ships>0) || (direction==-1 && p.ships>1))
			{
				p.ships+=direction;
				home.ships-=direction;
				p.gold-=direction*1000;
			}
			break;
			
			case 2:
			if((direction==1 && p.gold>=10 && home.men>0 && p.men<=(p.ships*100)) || (direction==-1 && p.men>1))
			{
				p.men+=direction;
				home.men-=direction;
				p.gold-=direction*10;
			}
			break;
			
			case 3:
			if((direction==1 && p.gold>=1 && home.food>=10 && cargo(p.forbidden)<(p.ships*100)) || (direction==-1 && p.food>=10))
			{
				p.food+=direction*10;
				home.food-=direction*10;
				p.gold-=direction;
			}
			break;
			
			case 4:
			if((direction==1 && p.gold>=1 && home.jewelry>=4 && cargo(p.forbidden)<(p.ships*100)) || (direction==-1 && p.jewelry>=4))
			{
				p.jewelry+=direction*4;
				home.jewelry-=direction*4;
				p.gold-=direction;
			}
			break;
			
			case EMBARK:
			// Max cargo is not monitored at the moment. if(cargo(p.forbidden)<=100)
			exit=1;
			break;
			
		}
	}while(!exit);
	
	clrscr();
}

void show_environment() // Draw the map
{
	int i,j;
	for(j=-10;j<=10;j++)
	{
		for(i=-20;i<=20;i++)
		{
			gotoxy(i+21,j+11);
			if(p.x+i<0 || p.y+j<0 || p.x+i>=MAX_X || p.y+j>=MAX_Y)
			{
				printf(" ");
			}
			else 
			{
				switch(m[p.x+i][p.y+j])
				{
					case LAND:
					txtcolor('G');
					cout<<"\"";
					break;
					
					case MOUNTAIN:
					txtcolor('W');
					cout<<"^";
					break;
					
					case WATER:
					txtcolor('B');
					cout<<","; 
					break;
					
					case TOWN: 
					txtcolor('Y');
					cout<<"*"; 
					break;
					
					case HOME:
					txtcolor('R');
					cout<<"O"; 
					break;
					
					case SHIP:
					txtcolor('C');
					cout<<"@"; 
					break;
				}
			}	
		}
	}
	txtcolor('C');
	gotoxy(21,11); 
	if(m[p.x][p.y]==WATER) cout << "@";
	else cout << "&";
	txtcolor('W');
}

void show_status()
{
	gotoxy(50,3); cout << "SHIPS: " << p.ships << "   ";
	gotoxy(50,5); cout << "MEN: " << p.men << "   ";
	gotoxy(50,7); cout << "GOLD: " << p.gold << "   ";
	gotoxy(50,9); cout << "FOOD: " << p.food << "   ";
	gotoxy(50,11); cout << "DAY: " << pvm << "   ";
	// gotoxy(50,13); cout << "JEWELRY: " << p.jewelry << "   ";
	// gotoxy(50,15); cout << "CARGO: " << cargo(p.forbidden) << " / " << p.ships*100 << "   ";
}

void atsea() // Not implemented yet.
{
	// p.forbidden=LAND;
	m[p.x][p.y]=WATER;
	p.men+=l.men;
	p.food+=l.food;
	p.jewelry+=l.jewelry;
	p.gold+=l.gold;
}

 void ashore() // Not implemented yet.
{
	int exit=0,move,direction,product=1;
	if(p.men>1 && p.x>0 && p.y>0 && p.x<MAX_X-1 && p.y<MAX_Y-1)
	{
		// l=p;
		if(m[p.x][p.y-1]==LAND) p.y--;
		else if(m[p.x+1][p.y]==LAND) p.x++;
		else if(m[p.x][p.y+1]==LAND) p.y++;
		else if(m[p.x-1][p.y]==LAND) p.x--;
		if(m[p.x][p.y]==LAND)
		{
			// p.forbidden=WATER;
			
			// l.men--;
			p.men=1;
			p.food=0;
			p.jewelry=0;
			p.gold=0;
			clrscr();
			gotoxy(20,3); cout<<"SHIPEXPEDITION";
			gotoxy(10,5); cout<<"MEN";
			gotoxy(10,7); cout<<"FOOD";
			gotoxy(10,9); cout<<"JEWELRY";
			gotoxy(10,11); cout<<"GOLD";
			
			do
			{
				gotoxy(20,5); cout<<"%7ld%7ld",l.men,p.men;
				gotoxy(20,7); cout<<"%7ld%7ld",l.food/l.men,p.food/p.men;
				gotoxy(20,9); cout<<"%7ld%7ld",l.jewelry,p.jewelry;
				gotoxy(20,11); cout<<"%7ld%7ld",l.gold,p.gold;
				gotoxy(10,20); cout<<"EXPEDITION CARGO %d%%",cargo(p.forbidden);
			
				switch(product=choose(&direction,4,product))
				{
					case 1:
					if((direction==1 && p.men>1) || (direction==-1 && l.men>1))
					{
						l.men+=direction;
						p.men-=direction;
					}
					break;
				
					case 2:
					if((direction==1 && p.food>0) || (direction==-1 && l.food>0))
					{
						if(direction==1)
						move=min(p.men,p.food);
						else 
						move=min(p.men,l.food);
						l.food+=direction*move;
						p.food-=direction*move;
					}
					break;
				
					case 3:
					if((direction==1 && p.jewelry>0) || (direction==-1 && l.jewelry>0))
					{
						l.jewelry+=direction;
						p.jewelry-=direction;
					}
					break;
					
					case 4:
					if((direction==1 && p.gold>0) || (direction==-1 && l.gold>0))
					{
						l.gold+=direction;
						p.gold-=direction;
					}
					break;
					
					case EMBARK:
					if(cargo(p.forbidden)<=100)
					exit=1;
					break;
				}
			}while(!exit);
		
		clrscr();
		}
	}
}

void tee(char command)
{
	switch(command)
	{
		case NORTHWARD:
		if(p.y>0 && m[p.x][p.y-1]!=MOUNTAIN) p.y--;
		break;
		
		case EASTWARD: 
		if(p.x<MAX_X-1 && m[p.x+1][p.y]!=MOUNTAIN) p.x++;
		break;
		
		case SOUTHWARD: 
		if(p.y<MAX_Y-1 && m[p.x][p.y+1]!=MOUNTAIN) p.y++;
		break;
		
		case WESTWARD:
		if(p.x>0 && m[p.x-1][p.y]!=MOUNTAIN) p.x--;
		break;
		
		/* case GOASHORE: 
		if(m[p.x][p.y]==WATER) ashore();
		break; */
		
		case END: 
		exit(0); 
		break;
	}
/* Starving removed for easier testing
	if(++pvm%7==0)
		if((p.food-=p.men)<0)
		{
			p.food=0;
			if(--p.men<1 && (m[p.x][p.y]==LAND || m[p.x][p.y]==WATER))
			{
			clrscr();
			gotoxy(20,1);
			cout<<"Your expedition starved to death!\n\r\n\r";
			getch();
			exit(0);
			}
		}
*/
	if(p.food>=(p.men/100)+1 && random(10)>7) p.food -= (p.men / 100) + 1;
	pvm++;
	
}

void mainmenu() // Needs a lot of work.
{
	int exit=0,direction,product=1;
	clrscr();
	
	gotoxy(20,2); cout << "Welcome to Atlantis!";
	gotoxy(20,5); cout << "New game";
	gotoxy(20,7); cout << "Quit";
	gotoxy(20,20); cout << "Buttons: w,s,a";
	
	do
	{
		switch(product=choose(&direction,2,product))
		{
			case 1:
			exit=1;
			break;
			
			case 2:
			gameover();
			break;
		}
	}while(!exit);
}

int main()
{
	srand(time(NULL));
	int i;
	clrscr();
	removecursor();
	mainmenu(); // Needs a lot of work.
	create_world();
	create_homecity();
	create_town(); 
	p.jewelry=0;
	p.gold=500;
	p.ships=1;
	p.men=1;
	p.food=20;
	p.x=home.x;
	p.y=home.y;
	// p.forbidden=LAND; Forbidden thing removed for some time
	while(1)
	{
		if(m[p.x][p.y]==HOME)athome();
		if(m[p.x][p.y]==TOWN)town2();
		// if(m[p.x][p.y]==SHIP)atsea(); I dont know what to do with this thing yet.
		show_environment();
		show_status();
		tee(getch());
	}
	
	return 0;
}
