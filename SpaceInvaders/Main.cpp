#include"icb_gui.h"
#include <random>
#include <atomic>
#include <fstream>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> ufoScore(0, 3);
std::uniform_int_distribution<> bulletRatio(0, 5);
std::uniform_int_distribution<> bulletType(1, 2);

HANDLE Mutex;

int keyCode;
bool powerOn = false;
bool fullScreen = false;
bool gameOver = false;
bool starterThreadAvailable = true;
int level = 0;
int highScore = 0;
std::atomic_int score;
std::atomic_int playerLife;

struct AlienBullets
{
	bool isBusy = false;
	bool isCollapse = false;
	int phase = 0;
	int type = 2;
	int xStart;
	int yStart;
	int yEnd;

};
struct AlienMatrix  //taken area of alive aliens
{
	int xStart = 10;
	int xEnd = 10;
	int yEnd = 10;
	int side = 0;//left or right
	bool aliensound = false; //play sound flag
};
struct Alien
{
	bool isAlive = true;
	int phase = 0;
	int type = 0;
	int xStart = 0;
	int yStart = 0;
	int xEnd = 0;
	int yEnd = 0;
};
struct Ufo
{
	bool isAlive = false;
	int side = 0;
	int xStart = 0;
	int yStart = 0;
	int xEnd = 0;
	int yEnd = 0;
};
struct Ship
{
	bool isAlive = true;
	bool pressFire = false;
	int xStart = 0;
	int yStart = 0;
	int xEnd = 0;
	int yEnd = 0;
};

AlienMatrix aMatrix;
Ship ship;
Ufo ufo;
Alien aliens[10][5];
AlienBullets alienBullets[10];
std::atomic_int currAlienBullet; //for limiting the number of bullets

int FRMMG, FRMBG, BTN;
ICBYTES fs, m, bg, btn;

ICBYTES ufoHighpitch, ufoLowpitch, fastinvader1, fastinvader2, fastinvader3, fastinvader4; //sound fluids

#pragma region Assets
ICBYTES UFO = {
		                                   {5,0}, {6,0}, {7,0}, {8,0}, {9,0}, {10,0},
		                     {3,1}, {4,1}, {5,1}, {6,1}, {7,1}, {8,1}, {9,1}, {10,1}, {11,1}, {12,1},
		              {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2}, {9,2}, {10,2}, {11,2}, {12,2}, {13,2},
		       {1,3}, {2,3},        {4,3}, {5,3},        {7,3}, {8,3},        {10,3}, {11,3},         {13,3}, {14,3},
		{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4}, {11,4}, {12,4}, {13,4}, {14,4}, {15,4},
		              {2,5}, {3,5}, {4,5},               {7,5}, {8,5},                {11,5}, {12,5}, {13,5},
		                     {3,6},                                                           {12,6},
};
ICBYTES cuttleFish = {
	              {2,0}, {3,0}, {4,0}, {5,0},
	       {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1},
	{0,2}, {1,2},        {3,2}, {4,2},        {6,2}, {7,2},
	{0,3}, {1,3}, {2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3},
	       {1,4}, {2,4},               {5,4}, {6,4},
	              {2,5}, {3,5}, {4,5}, {5,5},
	       {1,6},                             {6,6},
	              {2,7},               {5,7},
};
ICBYTES cuttleFish2 = {
	              {2,0}, {3,0},
	       {1,1}, {2,1}, {3,1}, {4,1},
	{0,2},        {2,2}, {3,2},        {5,2},
	       {1,3}, {2,3}, {3,3}, {4,3},
	              {2,4}, {3,4},
	       {1,5},               {4,5},
};
ICBYTES squid = {
		                     {3,0}, {4,0},
		              {2,1}, {3,1}, {4,1}, {5,1},
		       {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2},
		{0,3}, {1,3},        {3,3}, {4,3},        {6,3}, {7,3},
		{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4},
		              {2,5},               {5,5},
		       {1,6},        {3,6}, {4,6},        {6,6},
		{0,7},        {2,7},               {5,7},        {7,7},
};
ICBYTES squid2 = {
	                     {3,0}, {4,0},
	              {2,1}, {3,1}, {4,1}, {5,1},
	       {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2},
	{0,3}, {1,3},        {3,3}, {4,3},        {6,3}, {7,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4},
	       {1,5},        {3,5}, {4,5},        {6,5},
	{0,6},                                           {7,6},
	       {1,7},                             {6,7},
};
ICBYTES crab = {
	              {2,0},                                   {8,0},
	                     {3,1},                      {7,1},
	              {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2},
	       {1,3}, {2,3},        {4,3}, {5,3}, {6,3},        {8,3}, {9,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4},
	{0,5},        {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5}, {8,5},        {10,5},
	{0,6},        {2,6},                                    {8,6},        {10,6},
	                     {3,7}, {4,7},        {6,7}, {7,7},
};
ICBYTES crab2 = {
	              {2,0},                                    {8,0},
	{0,1},               {3,1}, {7,1},                                    {10,1},
	{0,2},        {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2},        {10,2},
	{0,3}, {1,3}, {2,3},        {4,3}, {5,3}, {6,3},        {8,3}, {9,3}, {10,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4},
	       {1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5}, {8,5}, {9,5},
	              {2,6},                                    {8,6},
	       {1,7},                                                  {9,7},
};
ICBYTES octopus = {
	                            {4,0}, {5,0}, {6,0}, {7,0},
	       {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {7,1}, {8,1}, {9,1},{10,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2}, {9,2}, {10,2}, {11,2},
	{0,3}, {1,3}, {2,3},               {5,3}, {6,3},               {9,3}, {10,3}, {11,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4}, {11,4},
	              {2,5}, {3,5}, {4,5},               {7,5}, {8,5}, {9,5},
	       {1,6}, {2,6},               {5,6}, {6,6},               {9,6}, {10,6},
	              {2,7}, {3,7},                             {8,7}, {9,7},

};
ICBYTES octopus2 = {
	                            {4,0}, {5,0}, {6,0}, {7,0},
	       {1,1}, {2,1}, {3,1}, {4,1}, {5,1}, {6,1}, {7,1}, {8,1}, {9,1},{10,1},
	{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {5,2}, {6,2}, {7,2}, {8,2}, {9,2}, {10,2}, {11,2},
	{0,3}, {1,3}, {2,3},               {5,3}, {6,3},               {9,3}, {10,3}, {11,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4}, {11,4},
	                     {3,5}, {4,5},               {7,5}, {8,5},
	              {2,6}, {3,6},        {5,6}, {6,6},        {8,6}, {9,6},
	{0,7}, {1,7},                                                         {10,7}, {11,7},

};
ICBYTES spaceship = {
	                                          {6,0},
	                                   {5,1}, {6,1}, {7,1},
	                                   {5,2}, {6,2}, {7,2},
	       {1,3}, {2,3}, {3,3}, {4,3}, {5,3}, {6,3}, {7,3}, {8,3}, {9,3}, {10,3}, {11,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4}, {6,4}, {7,4}, {8,4}, {9,4}, {10,4}, {11,4}, {12,4},
	{0,5}, {1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5}, {8,5}, {9,5}, {10,5}, {11,5}, {12,5},
	{0,6}, {1,6}, {2,6}, {3,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6}, {9,6}, {10,6}, {11,6}, {12,6},
	{0,7}, {1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}, {8,7}, {9,7}, {10,7}, {11,7}, {12,7},
	{0,8}, {1,8}, {2,8}, {3,8}, {4,8}, {5,8}, {6,8}, {7,8}, {8,8}, {9,8}, {10,8}, {11,8}, {12,8},
};
ICBYTES barrier{
											{5,0},  {6,0},  {7,0},  {8,0},  {9,0},  {10,0},  {11,0},  {12,0},  {13,0},  {14,0},  {15,0},  {16,0},
									{4,1},  {5,1},  {6,1},  {7,1},  {8,1},  {9,1},  {10,1},  {11,1},  {12,1},  {13,1},  {14,1},  {15,1},  {16,1},  {17,1},
							{3,2},  {4,2},  {5,2},  {6,2},  {7,2},  {8,2},  {9,2},  {10,2},  {11,2},  {12,2},  {13,2},  {14,2},  {15,2},  {16,2},  {17,2},  {18,2},
					{2,3},  {3,3},  {4,3},  {5,3},  {6,3},  {7,3},  {8,3},  {9,3},  {10,3},  {11,3},  {12,3},  {13,3},  {14,3},  {15,3},  {16,3},  {17,3},  {18,3},  {19,3},
			{1,4},  {2,4},  {3,4},  {4,4},  {5,4},  {6,4},  {7,4},  {8,4},  {9,4},  {10,4},  {11,4},  {12,4},  {13,4},  {14,4},  {15,4},  {16,4},  {17,4},  {18,4},  {19,4},  {20,4},
	{0,5},  {1,5},  {2,5},  {3,5},  {4,5},  {5,5},  {6,5},  {7,5},  {8,5},  {9,5},  {10,5},  {11,5},  {12,5},  {13,5},  {14,5},  {15,5},  {16,5},  {17,5},  {18,5},  {19,5},  {20,5},  {21,5},
	{0,6},  {1,6},  {2,6},  {3,6},  {4,6},  {5,6},  {6,6},  {7,6},  {8,6},  {9,6},  {10,6},  {11,6},  {12,6},  {13,6},  {14,6},  {15,6},  {16,6},  {17,6},  {18,6},  {19,6},  {20,6},  {21,6},
	{0,7},  {1,7},  {2,7},  {3,7},  {4,7},  {5,7},  {6,7},  {7,7},  {8,7},  {9,7},  {10,7},  {11,7},  {12,7},  {13,7},  {14,7},  {15,7},  {16,7},  {17,7},  {18,7},  {19,7},  {20,7},  {21,7},
	{0,8},  {1,8},  {2,8},  {3,8},  {4,8},  {5,8},  {6,8},  {7,8},  {8,8},  {9,8},  {10,8},  {11,8},  {12,8},  {13,8},  {14,8},  {15,8},  {16,8},  {17,8},  {18,8},  {19,8},  {20,8},  {21,8},
	{0,9},  {1,9},  {2,9},  {3,9},  {4,9},  {5,9},  {6,9},  {7,9},  {8,9},  {9,9},  {10,9},  {11,9},  {12,9},  {13,9},  {14,9},  {15,9},  {16,9},  {17,9},  {18,9},  {19,9},  {20,9},  {21,9},
	{0,10}, {1,10}, {2,10}, {3,10}, {4,10}, {5,10}, {6,10}, {7,10}, {8,10}, {9,10}, {10,10}, {11,10}, {12,10}, {13,10}, {14,10}, {15,10}, {16,10}, {17,10}, {18,10}, {19,10}, {20,10}, {21,10},
	{0,11}, {1,11}, {2,11}, {3,11}, {4,11}, {5,11}, {6,11}, {7,11}, {8,11}, {9,11}, {10,11}, {11,11}, {12,11}, {13,11}, {14,11}, {15,11}, {16,11}, {17,11}, {18,11}, {19,11}, {20,11}, {21,11},
	{0,12}, {1,12}, {2,12}, {3,12}, {4,12}, {5,12}, {6,12}, {7,12}, {8,12}, {9,12}, {10,12}, {11,12}, {12,12}, {13,12}, {14,12}, {15,12}, {16,12}, {17,12}, {18,12}, {19,12}, {20,12}, {21,12},
	{0,13}, {1,13}, {2,13}, {3,13}, {4,13}, {5,13}, {6,13}, {7,13}, {8,13}, {9,13}, {10,13}, {11,13}, {12,13}, {13,13}, {14,13}, {15,13}, {16,13}, {17,13}, {18,13}, {19,13}, {20,13}, {21,13},
	{0,14}, {1,14}, {2,14}, {3,14}, {4,14}, {5,14}, {6,14}, {7,14},                                                     {14,14}, {15,14}, {16,14}, {17,14}, {18,14}, {19,14}, {20,14}, {21,14},
	{0,15}, {1,15}, {2,15}, {3,15}, {4,15}, {5,15}, {6,15},                                                                      {15,15}, {16,15}, {17,15}, {18,15}, {19,15}, {20,15}, {21,15},
	{0,16}, {1,16}, {2,16}, {3,16}, {4,16}, {5,16},                                                                                       {16,16}, {17,16}, {18,16}, {19,16}, {20,16}, {21,16},
	{0,17}, {1,17}, {2,17}, {3,17}, {4,17}, {5,17},                                                                                       {16,17}, {17,17}, {18,17}, {19,17}, {20,17}, {21,17} };
#pragma endregion

#pragma region Num&Letter
ICBYTES zero{
		{1,0}, {2,0}, {3,0}, {4,0},
	{0,1},						 {5,1},
	{0,2},						 {5,2},
	{0,3}, {1,3},				 {5,3},
	{0,4},		 {2,4},			 {5,4},
	{0,5},			  {3,5},	 {5,5},
	{0,6},				  {4,6}, {5,6},
	{0,7},						 {5,7},
	{0,8},						 {5,8},
		{1,9}, {2,9}, {3,9}, {4,9},
};
ICBYTES one{
			{3,0},
	 {2,1}, {3,1},
			{3,2},
			{3,3},
			{3,4},
			{3,5},
			{3,6},
			{3,7},
			{3,8},
	 {2,9}, {3,9}, {4,9},



};
ICBYTES two{
		{1,0}, {2,0}, {3,0}, {4,0},
   {0,1},						{5,1},
								{5,2},
								{5,3},
						   {4,4},
					 {3,5},
				{2,6},
		  {1,7},
	{0,8},
	{0,9},{1,9}, {2,9}, {3,9}, {4,9}, {5,9},

};
ICBYTES three{
		 {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},							{5,1},
									{5,2},
									{5,3},
					{2,4}, {3,4}, {4,4},
									{5,5},
									{5,6},
									{5,7},
	{0,8},							{5,8},
		{1,9}, {2,9}, {3,9}, {4,9},

};
ICBYTES four{
	{0,0},							{5,0},
	{0,1},							{5,1},
	{0,2},							{5,2},
	{0,3},							{5,3},
	{0,4},{1,4},{2,4},{3,4}, {4,4}, {5,4},
									{5,5},
									{5,6},
									{5,7},
									{5,8},
									{5,9},
				
};
ICBYTES five{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
	{0,1},
	{0,2},
	{0,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4},
									{5,5},
									{5,6},
									{5,7},
	{0,8},							{5,8},
			{1,9}, {2,9}, {3,9}, {4,9},

};
ICBYTES six{
		{1,0},{2,0}, {3,0}, {4,0},
	{0,1},						 {5,1},
	{0,2},
	{0,3},
	{0,4},{1,4}, {2,4}, {3,4}, {4,4},
	{0,5},						 {5,5},
	{0,6},						 {5,6},
	{0,7},						 {5,7},
	{0,8},						 {5,8},
		{1,9}, {2,9}, {3,9}, {4,9},

};
ICBYTES seven{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
										{5,1},
										{5,2},
									{4,3},
							{3,4},
					{2,5},
			{1,6},
			{1,7},
			{1,8},
			{1,9},
};
ICBYTES eight {
		{1,0}, {2,0}, {3,0}, {4,0},
	{0,1},						 {5,1},
	{0,2},						 {5,2},
	{0,3},						 {5,3},
		{1,4}, {2,4}, {3,4}, {4,4},
	{0,5},						 {5,5},
	{0,6},						 {5,6},
	{0,7},						 {5,7},
	{0,8},						 {5,8},
		{1,9}, {2,9}, {3,9}, {4,9},
};
ICBYTES nine = {
		 {1,0} ,{2,0}, {3,0}, {4,0},
	{0,1},							{5,1},
	{0,2},							{5,2},
	{0,3},							{5,3},
		{1,4}, {2,4}, {3,4}, {4,4},	{5,4},
									{5,5},
									{5,6},
									{5,7},
	{0,8}, 							{5,8},
		 {1,9}, {2,9}, {3,9}, {4,9}
};
ICBYTES doubledots{
	{1,2},{2,2},
	{1,3},{2,3},
	{1,6},{2,6},
	{1,7},{2,7}
};
ICBYTES A{
				 {2,0}, {3,0},
		   {1,1},				{4,1},
	{0,2},						       {5,2},
	{0,3},							   {5,3},
	{0,4},							   {5,4},
	{0,5}, {1,5}, {2,5}, {3,5}, {4,5}, {5,5},
	{0,6},							   {5,6},
	{0,7},							   {5,7},
	{0,8},							   {5,8},
	{0,9},							   {5,9},
};
ICBYTES B{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                             {5,1},
	{0,2},                             {5,2},
	{0,3},                             {5,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4},
	{0,5},                             {5,5},
	{0,6},                             {5,6},
	{0,7},                             {5,7},
	{0,8},                             {5,8},
	{0,9}, {1,9}, {2,9}, {3,9}, {4,9},
};
ICBYTES C{
	{1,0}, {2,0}, {3,0}, {4,0}, {5,1},
	{0,1},
	{0,2},
	{0,3},
	{0,4},
	{0,5},
	{0,6},
	{0,7},
	{0,8},
	{1,9}, {2,9}, {3,9}, {4,9}, {5,8},
};
ICBYTES D{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                             {5,1},
	{0,2},                             {5,2},
	{0,3},                             {5,3},
	{0,4},                             {5,4},
	{0,5},                             {5,5},
	{0,6},                             {5,6},
	{0,7},                             {5,7},
	{0,8},                             {5,8},
	{0,9}, {1,9}, {2,9}, {3,9}, {4,9},

};
ICBYTES E{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
	{0,1},
	{0,2},
	{0,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4},
	{0,5},
	{0,6},
	{0,7},
	{0,8},
	{0,9}, {1,9}, {2,9}, {3,9}, {4,9}, {5,9},
};
ICBYTES F{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0}, {5,0},
	{0,1},
	{0,2},
	{0,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4},
	{0,5},
	{0,6},
	{0,7},
	{0,8},
	{0,9},
};
ICBYTES G{
	              {2,0}, {3,0}, {4,0},
	       {1,1},                      {5,1},
	{0,2},
	{0,3},
	{0,4},
	{0,5},               {3,5}, {4,5}, {5,5},
	{0,6},                             {5,6},
	{0,7},                             {5,7},
	{0,8},                             {5,8},
	       {1,9}, {2,9}, {3,9}, {4,9}, {5,9},
};
ICBYTES I{
	{0,0}, {1,0}, {2,0},
	       {1,1},
		   {1,2},
		   {1,3},
		   {1,4},
		   {1,5},
		   {1,6},
		   {1,7},
		   {1,8},
	{0,9}, {1,9}, {2,9},
};
ICBYTES L{
	{0,0}, 
	{0,1}, 
	{0,2}, 
	{0,3}, 
	{0,4}, 
	{0,5}, 
	{0,6}, 
	{0,7}, 
	{0,8}, 
	{0,9}, {1,9},{2,9},{3,9},{4,9},{5,9},
};
ICBYTES M{
	{0,0},                             {5,0},
	{0,1}, {1,1},               {4,1}, {5,1},
	{0,2},        {2,2}, {3,2},        {5,2},
	{0,3},                             {5,3},
	{0,4},                             {5,4},
	{0,5},                             {5,5},
	{0,6},                             {5,6},
	{0,7},                             {5,7},
	{0,8},                             {5,8},
	{0,9},                             {5,9},
};
ICBYTES N{
	{0,0},                             {5,0},
	{0,1},                             {5,1},
	{0,2},                             {5,2},
	{0,3}, {1,3},                      {5,3},
	{0,4},        {2,4},               {5,4},
	{0,5},               {3,5},        {5,5},
	{0,6},                      {4,6}, {5,6},
	{0,7},                             {5,7},
	{0,8},                             {5,8},
	{0,9},                             {5,9},
};
ICBYTES O{
	       {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                              {5,1},
	{0,2},                              {5,2},
	{0,3},                              {5,3},
	{0,4},                              {5,4},
	{0,5},                              {5,5},
	{0,6},                              {5,6},
	{0,7},                              {5,7},
	{0,8},                              {5,8},
	       {1,9}, {2,9}, {3,9}, {4,9},
};
ICBYTES H{
	{0,0},                             {5,0},
	{0,1},                             {5,1},
	{0,2},                             {5,2},
	{0,3},                             {5,3},
	{0,4}, {1,4}, {2,4}, {3,4}, {4,4}, {5,4},
	{0,5},                             {5,5},
	{0,6},                             {5,6},
	{0,7},                             {5,7},
	{0,8},                             {5,8},
	{0,9},                             {5,9},
};
ICBYTES P{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                             {5,1},
	{0,2},                             {5,2},
	{0,3},                             {5,3},
	{0,4},                             {5,4},
	{0,5}, {1,5}, {2,5}, {3,5}, {4,5},
	{0,6},
	{0,7},
	{0,8},
	{0,9},
};
ICBYTES R{
	{0,0}, {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                             {5,1},
	{0,2},                             {5,2},
	{0,3},                             {5,3},
	{0,4},                             {5,4},
	{0,5}, {1,5}, {2,5}, {3,5}, {4,5},
	{0,6},        {2,6},
	{0,7},               {3,7},
	{0,8},                      {4,8},
	{0,9},                            {5,9},
};
ICBYTES S{
	       {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                             {5,1},
	{0,2},
	{0,3},
	       {1,4},{2,4},{3,4},{4,4},
								       {5,5},
								       {5,6},
								       {5,7},
	{0,8},                             {5,8},
	     {1,9}, {2,9}, {3,9}, {4,9},
};
ICBYTES T{
	{0,0},{1,0}, {2,0}, {3,0}, {4,0}, {5,0}, {6,0},
	                    {3,1},
						{3,2},
						{3,3},
						{3,4},
						{3,5},
						{3,6},
						{3,7},
						{3,8},
						{3,9},
};
ICBYTES U{
	{0,0},                            {5,0},
	{0,1},                            {5,1},
	{0,2},                            {5,2},
	{0,3},                            {5,3},
	{0,4},                            {5,4},
	{0,5},                            {5,5},
	{0,6},                            {5,6},
	{0,7},                            {5,7},
	{0,8},                            {5,8},
	      {1,9}, {2,9}, {3,9}, {4,9},
};
ICBYTES V{
	{0,0},                            {5,0},
	{0,1},                            {5,1},
	{0,2},                            {5,2},
	{0,3},                            {5,3},
	{0,4},                            {5,4},
	{0,5},                            {5,5},
	{0,6},                            {5,6},
	{0,7},                            {5,7},
	       {1,8},              {4,8},
	              {2,9}, {3,9},
};
ICBYTES Y{
	{0,0},                      {4,0},
	{0,1},                      {4,1},
	{0,2},                      {4,2},
	       {1,3},        {3,3},
	              {2,4},
				  {2,5},
				  {2,6},
				  {2,7},
				  {2,8},
				  {2,9},
};
ICBYTES tire{
	{0,4}, {1,4}, {2,4}
};
ICBYTES qmark{
	      {1,0}, {2,0}, {3,0}, {4,0},
	{0,1},                            {5,1},
	{0,2},                            {5,2},
	                                  {5,3},
	                            {4,4},
	                     {3,5},
	                     {3,6},
	                     {3,7},

	                     {3,9},
};

#pragma endregion

#pragma region Bullets
ICBYTES spaceshipBullet{
	{0,0},
	{0,1},
	{0,2},
	{0,3},
};
ICBYTES alienZip1{
	{0,0},
	{1,1},
	{2,2},
	{1,3},
	{0,4},
	{1,5},
	{2,6},
};
ICBYTES alienZip2{
	{1,0},
	{2,1},
	{1,2},
	{0,3},
	{1,4},
	{2,5},
	{1,6},
};
ICBYTES alienZip3{
	{2,0},
	{1,1},
	{0,2},
	{1,3},
	{2,4},
	{1,5},
	{0,6},
};
ICBYTES alienZip4{
	{1,0},
	{0,1},
	{1,2},
	{2,3},
	{1,4},
	{0,5},
	{1,6},
};
ICBYTES alienBomb1{
	{1,0},{0,0},{2,0},
	{1,1},
	{1,2},
	{1,3},
	{1,4},
	{1,5},
	{1,6},
};
ICBYTES alienBomb2{
	{1,0},
	{1,1},
	{1,2},{0,2},{2,2},
	{1,3},
	{1,4},
	{1,5},
	{1,6},
};
ICBYTES alienBomb3{
	{1,0},
	{1,1},
	{1,2},
	{1,3},
	{1,4},{0,4},{2,4},
	{1,5},
	{1,6},
};
ICBYTES alienBomb4{
	{1,0},
	{1,1},
	{1,2},
	{1,3},
	{1,4},
	{1,5},
	{1,6},{0,6},{2,6},
};
#pragma endregion

#pragma region Effects
ICBYTES shipRIP{
	{3,0}, {13,0},
	{0,1}, {6,1}, {11,1}, {12,1}, {15,1},
	{3,2}, {8,2}, {9,2},
	{6,3}, {14,3},
	{4,1}, {4,4}, {6,4}, {7,4}, {10,4}, {11,4}, {15,4},
	{2,5}, {7,5}, {8,5}, {9,5}, {13,5},
	{3,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6}, {9,6}, {10,6}, {11,6},
	{2,7}, {3,7}, {5,7}, {6,7}, {7,7}, {8,7}, {9,7}, {10,7}, {11,7}, {14,7},
};
ICBYTES shipRIP2{
	{5,0},
	{10,1},
	{5,2}, {7,2}, {9,2},
	{2,3}, {5,3},
	{6,4}, {7,4}, {9,4}, {10,4},
	{0,5}, {4,5}, {6,5}, {7,5}, {9,5}, {11,5},
	{2,6}, {3,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6}, {9,6}, {12,6},
	{1,7}, {2,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}, {8,7}, {9,7}, {10,7}, {12,7}, {14,7},
};
ICBYTES alienKaboom{
	{1,0}, {4,0}, {8,0}, {11,0},
	{2,1}, {5,1}, {7,1}, {10,1},
	{3,2}, {9,2},
	{0,3}, {1,3}, {11,3}, {12,3},
	{3,4}, {9,4},
	{2,5}, {5,5}, {7,5}, {10,5},
	{1,6}, {4,6}, {8,6}, {11,6},

};
ICBYTES ufoKaboom{
	{2,0}, {5,0}, {7,0}, {14,0}, {16,0}, {19,0},
	{3,1}, {12,1}, {13,1}, {18,1},
	{0,2}, {2,2}, {6,2}, {7,2}, {8,2}, {9,2}, {13,2}, {14,2},
	{5,3}, {6,3}, {7,3}, {8,3}, {9,3}, {10,3}, {11,3}, {14,3}, {15,3}, {16,3}, {19,3},
	{4,4}, {5,4}, {6,4}, {8,4}, {10,4}, {12,4}, {15,4}, {16,4}, {17,4},
	{2,5}, {6,5}, {7,5}, {8,5}, {9,5}, {10,5}, {14,5}, {15,5},
	{0,6}, {7,6}, {9,6}, {13,6}, {14,6}, {18,6},
	{2,7}, {6,7}, {10,7}, {15,7},
};
ICBYTES bulletcollapse{ // merminin hem birbirine çarpýþýnda çýkan efekt
	{3,0},
	{8,1},
	{5,2},
	{2,3}, {8,3},
	{5,4}, {6,4}, {9,4},
	{1,5}, {3,5}, {4,5}, {5,5}, {6,5}, {7,5}, {8,5},
	{2,6}, {4,6}, {5,6}, {6,6}, {7,6}, {8,6}, {10,6},
	{1,7}, {3,7}, {4,7}, {5,7}, {6,7}, {7,7}, {8,7}, {9,7},
	{2,8}, {4,8}, {5,8}, {6,8}, {7,8}, {8,8}, {10,8},
	{3,9}, {5,9},
	{4,10}, {7,10},
};
ICBYTES barriershot{ // merminin bariyere çarpýnca çýkan efekt
	{2,1}, {4,1},
	{3,2},
	{1,3}, {3,3}, {4,3}, {6,3},
	{2,4}, {3,4}, {4,4}, {5,4},
	{1,5}, {2,5}, {3,5}, {4,5}, {5,5}, {6,5},
	{1,6}, {2,6}, {3,6}, {4,6}, {5,6}, {6,6},
	{2,7}, {3,7}, {4,7}, {5,7},
	{1,8}, {3,8}, {4,8}, {6,8},
	{4,9},
	{3,10}, {5,10},
};
#pragma endregion

bool CheckCollision(int x, int y, int xStart, int xEnd, int yStart, int yEnd, bool isAlive)
{
	if (x > xStart && x < xEnd && y < yEnd && y > yStart && isAlive)
	{
		return true;
	}
	return false;
}

void DrawUfo(bool black = false) // Drawing for UFO 
{
	if (ufo.isAlive || black)
	{
		int color = 0;
		if (!black) color = 0xFFF01F;
		SetPixelsX2(UFO, ufo.xStart, ufo.yStart, color, m);
	}
}
void DrawAlien(Alien a, bool black = false) // Drawing for Aliens
{
	if (a.isAlive)
	{
		int color = 0;

		if (a.type == 1)
		{
			if (!black) color = 0x39FF14;
			if (a.phase % 2 == 0) SetPixelsX2(squid, a.xStart, a.yStart, color, m);
			else  SetPixelsX2(squid2, a.xStart, a.yStart, color, m);
		}
		if (a.type == 2)
		{
			if (!black) color = 0x15F4EE;
			if (a.phase % 2 == 0) SetPixelsX2(crab, a.xStart, a.yStart, color, m);
			else  SetPixelsX2(crab2, a.xStart, a.yStart, color, m);
		}
		if (a.type == 3)
		{
			if (!black) color = 0xDF00FF;
			if (a.phase % 2 == 0) SetPixelsX2(octopus, a.xStart, a.yStart, color, m);
			else  SetPixelsX2(octopus2, a.xStart, a.yStart, color, m);
		}
		if (a.type == 4)//cuttlefish alien has different hitboxes in different phases
		{
			if (!black) color = 0xF6890A;
			if (a.phase % 2 == 0)
			{
				a.xStart -= 2;
				a.xEnd += 2;
				a.yStart -= 2;
				a.yEnd += 2;
				SetPixelsX2(cuttleFish, a.xStart, a.yStart, color, m);
			}
			else 
			{
				a.xStart += 2;
				a.xEnd -= 2;
				a.yStart += 2;
				a.yEnd -= 2;
				SetPixelsX2(cuttleFish2, a.xStart, a.yStart, color, m);
			}
		}
	}
}
void DrawShip(bool black = false) // Drawing for Ship
{
	int color = 0;
	if (!black) color = 0xFBFFFF;
	SetPixelsX2(spaceship, ship.xStart, ship.yStart, color, m);
}
void DrawNumber(int number, int x, int y, int color) //Drawing for Numbers
{

	switch (number)
	{
	case 0:
		SetPixelsX2(zero, x, y, color, m);
		break;
	case 1:
		SetPixelsX2(one, x, y, color, m);
		break;
	case 2:
		SetPixelsX2(two, x, y, color, m);
		break;
	case 3:
		SetPixelsX2(three, x, y, color, m);
		break;
	case 4:
		SetPixelsX2(four, x, y, color, m);
		break;
	case 5:
		SetPixelsX2(five, x, y, color, m);
		break;
	case 6:
		SetPixelsX2(six, x, y, color, m);
		break;
	case 7:
		SetPixelsX2(seven, x, y, color, m);
		break;
	case 8:
		SetPixelsX2(eight, x, y, color, m);
		break;
	case 9:
		SetPixelsX2(nine, x, y, color, m);
		break;
	}
}
void DrawScore(int score, int x, int y, bool black = false) // Drawing for Scores
{
	int color = 0;
	if (!black) color = 0xFFFFFF;
	for (int i = 4; i + 1 > 0; i--)
	{
		DrawNumber(score % 10, x + i * 14, y, color);
		score /= 10;
	}

}
void DrawBullet(AlienBullets b, bool black = false) // Drawing for Alien Bullets, alien bullets have animations so this method is used to draw them
{
	int color = 0;
	if (b.type == 1)
	{
		if (!black) color = 0xCCFF00;
		if (b.phase % 8 >= 6)
		{
			SetPixelsX2(alienZip4, b.xStart, b.yStart, color, m);
		}
		else if (b.phase % 8 >= 4)
		{
			SetPixelsX2(alienZip3, b.xStart, b.yStart, color, m);
		}
		else if (b.phase % 8 >= 2)
		{
			SetPixelsX2(alienZip2, b.xStart, b.yStart, color, m);
		}
		else if (b.phase % 8 >= 0)
		{
			SetPixelsX2(alienZip1, b.xStart, b.yStart, color, m);
		}
	}
	if (b.type == 2)
	{
		if (!black) color = 0xFF1818;
		if (b.phase % 8 >= 6)
		{
			SetPixelsX2(alienBomb1, b.xStart, b.yStart, color, m);
		}
		else if (b.phase % 8 >= 4)
		{
			SetPixelsX2(alienBomb2, b.xStart, b.yStart, color, m);
		}
		else if (b.phase % 8 >= 2)
		{
			SetPixelsX2(alienBomb3, b.xStart, b.yStart, color, m);
		}
		else if (b.phase % 8 >= 0)
		{
			SetPixelsX2(alienBomb4, b.xStart, b.yStart, color, m);
		}
	}
}

void InitUfo() // UFO is created with default values
{
	ufo.isAlive = true;
	ufo.side = 0;
	ufo.xStart = -33;
	ufo.yStart = 33;
	ufo.xEnd = ufo.xStart + 32;
	ufo.yEnd = ufo.yStart + 14;
}
void InitAliens() // Aliens and Barriers are created with default values
{
	aMatrix.side = 1;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			aliens[i][j].phase = 0;
			aliens[i][j].isAlive = true;
			aliens[i][j].xStart = 13 + i * 32;
			aliens[i][j].yStart = 68 + j * 32;
			if (j == 0)
			{
				aliens[i][j].xEnd = aliens[i][j].xStart + 16;
				aliens[i][j].type = 1;
			}
			else if (j == 1 || j == 2)
			{
				aliens[i][j].xEnd = aliens[i][j].xStart + 22;
				aliens[i][j].type = 2;
			}
			else if (j == 3 || j == 4)
			{
				aliens[i][j].xEnd = aliens[i][j].xStart + 24;
				aliens[i][j].type = 3;
			}
			aliens[i][j].yEnd = aliens[i][j].yStart + 16;
		}
	}
	SetPixelsX2(barrier, 32, 250, 0xff0000, m);
	SetPixelsX2(barrier, 128, 250, 0xff0000, m);
	SetPixelsX2(barrier, 224, 250, 0xff0000, m);
	SetPixelsX2(barrier, 320, 250, 0xff0000, m);
	SetPixelsX2(barrier, 416, 250, 0xff0000, m);
}
void InitShip() // Ship is created with default values
{
	ship.isAlive = true;
	ship.pressFire = false;
	ship.xStart = 3;
	ship.xEnd = ship.xStart + 26;
	ship.yStart = 318;
	ship.yEnd = ship.yStart + 14;
}

void ICGUI_Create() // Creating the GUI
{
	ICG_MWTitle("Space Invaders");
	ICG_MWSize(800, 1120);
}

VOID CALLBACK ScreenLoader(PVOID lpParam, BOOLEAN TimerOrWaitFired) //ScreenLoader is used to m matrix to be displayed on the screen
{
	if (fullScreen)
	{
		MagnifyX3(m, fs);
		DisplayImage(FRMMG, fs);
	}
	else
	{
		DisplayImage(FRMMG, m);
	}
}
void* UfoSoundControllerThread(PVOID lpParam)//Thread for UFO sound
{
	while (!gameOver)
	{
		if(ufo.isAlive)
		{
			if (ufo.xStart < 483 && ufo.xStart > -33)//Ufo's visible area
			{
				//PlaySound("ufo_highpitch.wav", NULL, SND_ASYNC);
				WaveOut(ufoHighpitch, 0);
				Sleep(200);
			}
		}
		else
		{
			for (int i = 0; i < 50; i++)
			{
				if (gameOver) return(NULL);
				Sleep(500);
			}
		}
	}
	return(NULL);
}
void* UfoControllerThread(PVOID lpParam) //Thread for UFO movements
{
	while (!gameOver) // we move the ufo left and right every 25 seconds until the game is lost.
	{
		InitUfo();
		while (ufo.isAlive)//if ufo is alive, it keeps moving
		{
			DrawUfo(true);
			if (ufo.xStart >= 483 || ufo.xStart <= -33)
			{
				for (int i = 0; i < 50; i++) {
					if (gameOver) return(NULL);
					Sleep(500);
				}
				ufo.side++;
			}
			if (ufo.xStart >= -33 && ufo.side % 2 == 0)
			{
				ufo.xStart -= 4;//left
				ufo.xEnd -= 4;
			}
			if (ufo.xStart <= 483 && ufo.side % 2 == 1)
			{
				ufo.xStart += 4;//right
				ufo.xEnd += 4;
			}
			DrawUfo();
			Sleep(18);
		}
		DrawUfo(true); //when ufo dies, we delete it and
		SetPixelsX2(ufoKaboom, ufo.xStart, ufo.yStart, 0xFFFF66, m); //draw the explosion effect
		WaveOut(ufoLowpitch, 0); //and play the death sound
		Sleep(500);
		SetPixelsX2(ufoKaboom, ufo.xStart, ufo.yStart, 0, m);
	}
	return(NULL);
}
void* AlienSoundControllerThread(PVOID lpParam) //Thread for Alien sounds
{
	int sound = 0;
	while (!gameOver)//4 sounds are played one after the other (when the aliendsound flag is true in the thread, it calls the sound and makes it false)
	{
		if (aMatrix.aliensound)
		{
			aMatrix.aliensound = false;
			switch (sound % 4)
			{
			case 0:
				WaveOut(fastinvader1, 0);
				Sleep(150);
				//Sleep(90);
				//PlaySound("fastinvader1.wav", NULL, SND_SYNC);
				break;
			case 1:
				WaveOut(fastinvader2, 0);
				Sleep(150);
				//Sleep(95);
				//PlaySound("fastinvader2.wav", NULL, SND_SYNC);
				break;
			case 2:
				WaveOut(fastinvader3, 0);
				Sleep(150);
				//Sleep(96);
				//PlaySound("fastinvader3.wav", NULL, SND_SYNC);
				break;
			case 3:
				WaveOut(fastinvader4, 0);
				Sleep(150);
				//Sleep(100);
				//PlaySound("fastinvader4.wav", NULL, SND_SYNC);
				break;
			}
			sound++;
		}
	}
	return(NULL);
}
void* AlienBulletThread(PVOID lpParam)//AlienBulletThread is the thread that controls the bullets fired by the aliens - it fires the bullet according to which column the alien is in (taken as id)
{
	int* tmp= (int*)lpParam;
	int id = *tmp;
	alienBullets[id].isCollapse = false;
	alienBullets[id].phase = 0;
	alienBullets[id].type = bulletType(gen); // bullet type is determined, will it be a red missile or a yellow lightning, according to this, we determine its speed and other things
	int sleepTime = 0;
	if (alienBullets[id].type == 1) sleepTime = 32;
	else if (alienBullets[id].type == 2) sleepTime = 18;

	while (alienBullets[id].yStart < 340) // loop that works until the alien's bullet reaches the bottom of the screen
	{
		DrawBullet(alienBullets[id], true);
		if (m.U((alienBullets[id].xStart - 2), alienBullets[id].yEnd) == 0xff0000) //checking for collision with barriers
		{
			SetPixelsX2(barriershot, alienBullets[id].xStart - 8, alienBullets[id].yEnd-8, 0, m);
			Sleep(500);
			alienBullets[id].isBusy = false;
			currAlienBullet--;
			return(NULL);
		}
		if (alienBullets[id].isCollapse)//checking for collision with the players fired bullet
		{
			SetPixelsX2(bulletcollapse, alienBullets[id].xStart - 5, alienBullets[id].yStart, 0XFF1818, m);
			Sleep(500);
			SetPixelsX2(bulletcollapse, alienBullets[id].xStart - 5, alienBullets[id].yStart, 0, m);
			alienBullets[id].isBusy = false;
			currAlienBullet--;
			return(NULL);
		}
		if (CheckCollision(alienBullets[id].xStart, alienBullets[id].yEnd, ship.xStart, ship.xEnd, ship.yStart, ship.yEnd, true)) //checking for collision with the ship
		{
			playerLife -= 1;
			ship.isAlive = false; //we kill the ship
			Sleep(500);
			alienBullets[id].isBusy = false;
			currAlienBullet--;
			return(NULL);
		}
		alienBullets[id].yStart += 4;
		alienBullets[id].yEnd += 4;
		alienBullets[id].phase++;
		DrawBullet(alienBullets[id]);
		Sleep(sleepTime);
	}
	DrawBullet(alienBullets[id], true);
	Sleep(500);
	alienBullets[id].isBusy = false;
	currAlienBullet--; // to release the limited bullet count 
	return(NULL);
}
void* AlienControllerThread(PVOID lpParam) //Thread that controls the movement of aliens
{
	while (!gameOver)
	{
		bool anyAlienAlive;
		InitAliens();
		do { // loop that will continue as long as any alien is alive
			if (gameOver) return(NULL);
			int tempXStart = 475;
			int tempXEnd = 5;
			int tempYEnd = 5;
			anyAlienAlive = false;
			for (int i = 0; i < 10; i++) // we find the rectangle area of the aliens according to the aliens alive
			{
				for (int j = 4; j + 1 > 0; j--)
				{
					if (aliens[i][j].isAlive)
					{
						if (aliens[i][j].xStart < tempXStart) tempXStart = aliens[i][j].xStart;
						if (aliens[i][j].xEnd > tempXEnd) tempXEnd = aliens[i][j].xEnd;
						if (aliens[i][j].yEnd > tempYEnd) tempYEnd = aliens[i][j].yEnd;
						anyAlienAlive = true;
					}
				}
			}
			aMatrix.xStart = tempXStart;
			aMatrix.xEnd = tempXEnd;
			aMatrix.yEnd = tempYEnd;
			if (aMatrix.xEnd >= 475 || aMatrix.xStart <= 5) // if the right or left edge of the area we learned is close to the edge of our screen, it makes it go down one step and change direction
			{
				aMatrix.side++;
				FillRect(m,0, aMatrix.yEnd-2,480,32,0);
				aMatrix.aliensound = true;
				for (int j = 4; j + 1 > 0; j--)
				{
					for (int i = 0; i < 10; i++)
					{
						if (aliens[i][j].isAlive) 
						{
							DrawAlien(aliens[i][j], true);
							aliens[i][j].yStart += 32;
							aliens[i][j].yEnd += 32;
							aliens[i][j].phase++;
							DrawAlien(aliens[i][j]);
							Sleep(20 - level);
						}
						if (gameOver) return(NULL);
					}
				}
			}
			for (int i = 0; i < 10; i++) // we check each column from the bottom row to the top row for alien bullet fires, it finds the first alien it finds alive in each column and gives the order to fire according to a probability. and moves on to the next column
			{
				for (int j = 4; j + 1 > 0; j--)
				{
					if (!alienBullets[i].isBusy && aliens[i][j].isAlive)
					{
						if (bulletRatio(gen) == 0 && currAlienBullet < 2 + level / 2) //%16 fire chance
						{
							currAlienBullet++;
							alienBullets[i].xStart = aMatrix.side % 2 == 0 ? aliens[i][j].xStart : aliens[i][j].xStart + 16;
							alienBullets[i].yStart = aliens[i][j].yEnd + 16;
							alienBullets[i].yEnd = alienBullets[i].yStart + 8;
							int id = i;
							DWORD dwb;
							CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AlienBulletThread, &id, 0, &dwb);
							alienBullets[i].isBusy = true;
							Sleep(2);
						}
						break; // we break the loop to prevent the aliens above from firing even if the alien at the bottom fires or not
					}
					if (gameOver) return(NULL);
				}
			}
			aMatrix.aliensound = true;
			for (int j = 4; j + 1 > 0; j--) // a part that controls their movements to the right or left
			{
				for (int i = 0; i < 10; i++)
				{
					if (aliens[i][j].isAlive)
					{
						DrawAlien(aliens[i][j], true);

						if (aMatrix.side % 2 == 0)
						{
							aliens[i][j].xStart -= 8;
							aliens[i][j].xEnd -= 8;
						}
						else if (aMatrix.side % 2 == 1)
						{
							aliens[i][j].xStart += 8;
							aliens[i][j].xEnd += 8;
						}
						aliens[i][j].phase++;
						DrawAlien(aliens[i][j]);
						Sleep(20 - level);
						
					}
					if (gameOver) return(NULL);
				}
			}
			if (aMatrix.yEnd >= 324) gameOver = true; // if the bottom of the matrix where the aliens are comes to the level of the spaceship, we lose the game.
		} while (anyAlienAlive);
		if (level < 19)level++; // if we kill all the aliens, the level increases and the game starts again. (Increasing the level reduces the time the aliens wait while moving)
	}// here, since the old atari processor processes objects one by one, we wait for a certain time while each alien moves, giving a more realistic atari feeling.
	// also, as the number of aliens in the old atari decreases, the movement of the aliens increases because it will process the same object in one second, we added this too
	return(NULL);
} 
void* ShipBulletThread(PVOID lpParam)//Thread that controls the bullets fired by the ship
{
	int x = ship.xStart + 12;
	int y = ship.yStart;
	//WaveOut(shoot, 0);
	PlaySound("shoot.wav", NULL, SND_ASYNC);

	while (y > 33) // loop that works until the bullet reaches the top of the screen
	{ 
		SetPixelsX2(spaceshipBullet, x, y, 0, m);

		if (m.U(x, y) == 0xff0000) //checking for collision with barriers
		{  
			SetPixelsX2(barriershot, x-6, y-4, 0, m);
			ship.pressFire = false;
			return(NULL);
		}
		for (int i = 0; i < 10; i++) //checking for collision with alien bullets
		{
			if (CheckCollision(x, y, alienBullets[i].xStart - 10, alienBullets[i].xStart + 6 + 10, alienBullets[i].yStart, alienBullets[i].yEnd + 1, alienBullets[i].isBusy && alienBullets[i].type == 1))
			{
				alienBullets[i].isCollapse = true;
				ship.pressFire = false;
				return(NULL);
			}
		}
		for (int j = 0; j < 5; j++)  //checking for collision with aliens
		{
			for (int i = 0; i < 10; i++)
			{
				if (CheckCollision(x, y, aliens[i][j].xStart, aliens[i][j].xEnd, aliens[i][j].yStart, aliens[i][j].yEnd, aliens[i][j].isAlive))
				{
					SetPixelsX2(spaceshipBullet, x, y, 0, m);
					DrawAlien(aliens[i][j], true);
					Alien a;
					if (ufoScore(gen) == 0 && aliens[i][j].type == 2 && level > 2) //%50 chance of cuttlefish alien after level 2
					{
						aliens[i][j].type = 4;
						aliens[i][j].xStart += 4;
						aliens[i][j].xEnd += 4;
						DrawAlien(aliens[i][j]);
					}
					else //or alien dies
					{
						aliens[i][j].isAlive = false;
						a.phase = aliens[i][j].phase;
						a.type = aliens[i][j].type;
						a.xStart = aliens[i][j].xStart;
						a.yStart = aliens[i][j].yStart;
						DrawAlien(a, true);
					}//and a sound is played according to this
					if (aliens[i][j].isAlive == false )PlaySound("invaderkilled.wav", NULL, SND_ASYNC);
					else if (aliens[i][j].isAlive == true && aliens[i][j].type == 4) PlaySound("invaderpop.wav", NULL, SND_ASYNC);
					switch (a.type)//score is determined according to the type of alien killed
					{
					case 1:
						a.xStart -= 4;
						score += 30;
						break;
					case 2:
						score += 20;
						break;
					case 3:
						score += 10;
						break;
					case 4:
						a.xStart -= 4;
						score += 30;
					}
					if (aliens[i][j].isAlive == false) {  //explosion effect is drawn
						SetPixelsX2(alienKaboom, a.xStart, a.yStart, 0xFFFFFF, m);
						Sleep(150);
						SetPixelsX2(alienKaboom, a.xStart, a.yStart, 0, m);
					}
					ship.pressFire = false; 
					return(NULL);
				}
			}
		}
		if (CheckCollision(x, y, ufo.xStart, ufo.xEnd, ufo.yStart, ufo.yEnd, ufo.isAlive)) //checking for collision with ufo
		{
			SetPixelsX2(spaceshipBullet, x, y, 0, m);
			switch (ufoScore(gen)) //points are given randomly  "MYSTERY ?"
			{
			case 0:
				score += 50;
				break;
			case 1:
				score += 100;
				break;
			case 2:
				score += 150;
				break;
			case 3:
				score += 300;
				break;
			}
			ufo.isAlive = false;
			ship.pressFire = false;
			return(NULL);
		}
		y -= 6;
		SetPixelsX2(spaceshipBullet, x, y, 0xffffff, m);
		Sleep(1);
	}
	SetPixelsX2(spaceshipBullet, x, y, 0, m);
	ship.pressFire = false; // we release the bullet so that the bullet can be fired again without hitting anything or reaching the top of the screen 
	//this is old atari logic, the bullet is released when it reaches the top of the screen or hits something
	return(NULL);
 }
 void* ShipControllerThread(PVOID lpParam) //Thread that controls the movements of the spaceship
{
	while (!gameOver)
	{
		InitShip();
		while (ship.isAlive)  //ship is moving or firing according to the pressed key as long as it is alive
		{
			if (gameOver) return(NULL);
			DrawShip(true);
			if (keyCode == VK_SPACE)
			{
				if (!ship.pressFire)
				{
					DWORD dwb;
					CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShipBulletThread, NULL, 0, &dwb);
				}
				ship.pressFire = true;
				keyCode = VK_UP; // we manually change the pressed key so that it does not fire again after firing
			}
			else if (keyCode == VK_LEFT && ship.xStart > 3)
			{
				ship.xStart -= 4;//left
				ship.xEnd -= 4;
			}
			else if (keyCode == VK_RIGHT && ship.xEnd < 475)
			{
				ship.xStart += 4;//right
				ship.xEnd += 4;
			}
			DrawShip();
			Sleep(20);
		}
		//when the ship dies, that is, when it exits the loop above, the sound plays and the animation plays
		DrawShip(true);
		PlaySound("explosion.wav", NULL, SND_ASYNC);
		for (int i = 0; i < 5; i++)
		{
			if (i % 2 == 0)
			{
				SetPixelsX2(shipRIP, ship.xStart, ship.yStart, 0xFF1818, m);
				Sleep(100);
				SetPixelsX2(shipRIP, ship.xStart, ship.yStart, 0, m);
			}
			else 
			{
				SetPixelsX2(shipRIP2, ship.xStart, ship.yStart, 0xFF1818, m);
				Sleep(100);
				SetPixelsX2(shipRIP2, ship.xStart, ship.yStart, 0, m);
			}
		}
		if(playerLife <= 0) return(NULL);
	}
	return(NULL);
}
 void* LifeAndScoreControllerThread(PVOID lpParam)//Thread that controls lives and scores and waits for the game over screen when the game is over
{
	level = 0;
	DrawScore(highScore, 306, 6);
	int tempScore = -1;
	score = 0;
	playerLife = 3;
	DrawNumber(playerLife, 458, 6, 0xFFFFFF);
	int tempLife = playerLife;
	int tresholdScore = 0;
	while (!gameOver)
	{
		if (tempScore != score)
		{
			DrawScore(tempScore, 88, 6, true);
			tempScore = score;
			DrawScore(tempScore, 88, 6);

			if (tresholdScore + 1000 <= score) //player gains 1 life for every 1000 scored
			{
				tresholdScore += 1000;
				playerLife++;
			}
		}

		if (tempLife != playerLife) // if the player's life increases or decreases, the number of lives is shown in color
		{
			DrawNumber(tempLife, 458, 6, 0);
			if (tempLife < playerLife) //if the player gains life, it is shown in green
			{
				tempLife = playerLife;
				DrawNumber(tempLife, 458, 6, 0xFF00);
				Sleep(1000);
			}
			else if (playerLife <= 0) //if the player loses all lives, the game is over
			{
				tempLife = playerLife;
				gameOver = true;
			}
			else if (tempLife > playerLife) //if the player loses life, it is shown in red
			{
				tempLife = playerLife;
				DrawNumber(tempLife, 458, 6, 0xFF0000);
				Sleep(1000);
			}
			DrawNumber(tempLife, 458, 6, 0xFFFFFF);
		}
		Sleep(50);
	}
	WaitForSingleObject(Mutex, INFINITE); // we use mutex to prevent the game from being restarted until the game is over
	//game over screen
	if (score > highScore) //if the player has reached the high score, the new high score is written above
	{
		DrawScore(highScore, 306, 6, true);
		highScore = score;
		DrawScore(highScore, 306, 6);
		std::ofstream outputFile("scores.txt");
		if (outputFile.is_open()) {
			outputFile << highScore;
			outputFile.close();
		}
	}
	PlaySound("gameover.wav", NULL, SND_ASYNC);
#pragma region GameOverText
	Sleep(300);
	FillRect(m, 0, 30, 480, 317, 0);
	SetPixelsX2(G, 180 + 0 * 16, 140, 0x808080, m);
	Sleep(150);
	SetPixelsX2(A, 180 + 1 * 16, 140, 0x808080, m);
	Sleep(150);
	SetPixelsX2(M, 180 + 2 * 16, 140, 0x808080, m);
	Sleep(150);
	SetPixelsX2(E, 180 + 3 * 16, 140, 0x808080, m);
	Sleep(150);

	SetPixelsX2(O, 180 + 5 * 16, 140, 0x808080, m);
	Sleep(150);
	SetPixelsX2(V, 180 + 6 * 16, 140, 0x808080, m);
	Sleep(150);
	SetPixelsX2(E, 180 + 7 * 16, 140, 0x808080, m);
	Sleep(150);
	SetPixelsX2(R, 180 + 8 * 16, 140, 0x808080, m);
	Sleep(150);

	int color = 0x888888;
	SetPixelsX2(P, 70 + 0 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(R, 70 + 1 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(E, 70 + 2 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(S, 70 + 3 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(S, 70 + 4 * 16, 190, color, m);
	Sleep(50);

	SetPixelsX2(E, 70 + 6 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(N, 70 + 7 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(T, 70 + 8 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(E, 72 + 9 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(R, 72 + 10 * 16, 190, color, m);
	Sleep(50);

	SetPixelsX2(T, 72 + 12 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(O, 74 + 13 * 16, 190, color, m);
	Sleep(50);

	SetPixelsX2(R, 74 + 15 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(E, 74 + 16 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(S, 74 + 17 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(T, 74 + 18 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(A, 76 + 19 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(R, 76 + 20 * 16, 190, color, m);
	Sleep(50);
	SetPixelsX2(T, 76 + 21 * 16, 190, color, m);
	Sleep(300);
#pragma endregion
	ReleaseMutex(Mutex); // we release the mutex
	bool inc = false;
	while (gameOver) //a loop for the flashing text on the screen until the user presses enter
	{
		if (inc) color += 0x010101;
		else color -= 0x010101;
		if (color == 0) inc = true;
		else if (color == 0x888888) inc = false;
		SetPixelsX2(P, 70 + 0 * 16, 190, color, m);
		SetPixelsX2(R, 70 + 1 * 16, 190, color, m);
		SetPixelsX2(E, 70 + 2 * 16, 190, color, m);
		SetPixelsX2(S, 70 + 3 * 16, 190, color, m);
		SetPixelsX2(S, 70 + 4 * 16, 190, color, m);

		SetPixelsX2(E, 70 + 6 * 16, 190, color, m);
		SetPixelsX2(N, 70 + 7 * 16, 190, color, m);
		SetPixelsX2(T, 70 + 8 * 16, 190, color, m);
		SetPixelsX2(E, 72 + 9 * 16, 190, color, m);
		SetPixelsX2(R, 72 + 10 * 16, 190, color, m);

		SetPixelsX2(T, 72 + 12 * 16, 190, color, m);
		SetPixelsX2(O, 74 + 13 * 16, 190, color, m);

		SetPixelsX2(R, 74 + 15 * 16, 190, color, m);
		SetPixelsX2(E, 74 + 16 * 16, 190, color, m);
		SetPixelsX2(S, 74 + 17 * 16, 190, color, m);
		SetPixelsX2(T, 74 + 18 * 16, 190, color, m);
		SetPixelsX2(A, 76 + 19 * 16, 190, color, m);
		SetPixelsX2(R, 76 + 20 * 16, 190, color, m);
		SetPixelsX2(T, 76 + 21 * 16, 190, color, m);
		Sleep(1);
	}//Thread ends when the game is restarted
	return(NULL);
}

void ChangeFullScreen()  //Function that provides the transition between full screen mode and atari mode. It destroys the form and recreates it
{
	if (fullScreen) {
		ICG_MWSize(800, 1120);
		ICG_DestroyWidget(FRMMG);

		FRMBG = ICG_Frameless(0, 0, 300, 300);
		DisplayImage(FRMBG, bg);

		BTN = ICG_BitmapButton(450, 762, 38, 21, ChangeFullScreen);
		SetButtonBitmap(BTN, btn);

		FRMMG = ICG_Frameless(162, 274, 480, 347);
		fullScreen = false;
	}
	else 
	{
		ICG_MWSize(1460, 1080);

		ICG_DestroyWidget(FRMMG);
		ICG_DestroyWidget(BTN);
		ICG_DestroyWidget(FRMBG);

		FRMMG = ICG_Frameless(0, 0, 1440, 1041);
		fullScreen = true;
	}
}

void StartGame() //Function that starts the threads required for the game to run
{
	DWORD dw;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UfoControllerThread, NULL, 0, &dw);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UfoSoundControllerThread, NULL, 0, &dw);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AlienControllerThread, NULL, 0, &dw);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AlienSoundControllerThread, NULL, 0, &dw);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LifeAndScoreControllerThread, NULL, 0, &dw);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShipControllerThread, NULL, 0, &dw);
}

void* StarterThread(PVOID lpParam) // Thread that starts the game
{
	if (!powerOn)  // When the machine is turned on (When the game is first opened), a logo created by the initials of our computer science club and a small intro are entered
	{
		powerOn = true;
		ReadImage("ybtlogo.bmp", m);
		PlaySound("logo.wav", NULL, SND_ASYNC);
		Sleep(1500);
		PlaySound("power_on.wav", NULL, SND_ASYNC);
		m = 0;
		Sleep(500);
		ReadImage("Logo.bmp", m);
		Sleep(1500);
		m = 0;
		#pragma region GameStartText
		SetPixelsX2(P, 210 + 0 * 16, 36, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(L, 210 + 1 * 16, 36, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(A, 210 + 2 * 16, 36, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(Y, 210 + 3 * 16, 36, 0xffffff, m);
		Sleep(150);


		SetPixelsX2(S, 130 + 0 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(P, 130 + 1 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(A, 130 + 2 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(C, 130 + 3 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(E, 130 + 4 * 16, 76, 0xffffff, m);
		Sleep(150);

		SetPixelsX2(I, 130 + 6 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(N, 130 + 10 + 6 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(V, 130 + 10 + 7 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(A, 130 + 10 + 8 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(D, 130 + 10 + 9 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(E, 130 + 10 + 10 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(R, 130 + 10 + 11 * 16, 76, 0xffffff, m);
		Sleep(150);
		SetPixelsX2(S, 130 + 10 + 12 * 16, 76, 0xffffff, m);
		Sleep(150);


		SetPixelsX2(S, 90 + 0 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(C, 90 + 1 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(O, 90 + 2 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(R, 90 + 3 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(E, 90 + 4 * 16, 136, 0xCCCCCC, m);
		Sleep(150);

		SetPixelsX2(A, 90 + 6 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(D, 90 + 7 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(V, 90 + 8 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(A, 90 + 9 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(N, 90 + 10 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(C, 90 + 11 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(E, 90 + 12 * 16, 136, 0xCCCCCC, m);
		Sleep(150);

		SetPixelsX2(T, 90 + 14 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(A, 92 + 15 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(B, 92 + 16 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(L, 92 + 17 * 16, 136, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(E, 92 + 18 * 16, 136, 0xCCCCCC, m);
		Sleep(150);


		SetPixelsX2(UFO, 160 - 20 + 0 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(doubledots, 160 + 2 + 1 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(qmark, 160 + 2 * 16, 176, 0xFFF01F, m);
		Sleep(150);

		SetPixelsX2(M, 160 + 4 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(Y, 160 + 5 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(S, 160 + 6 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(T, 160 + 7 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(E, 162 + 8 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(R, 162 + 9 * 16, 176, 0xFFF01F, m);
		Sleep(150);
		SetPixelsX2(Y, 162 + 10 * 16, 176, 0xFFF01F, m);
		Sleep(150);


		SetPixelsX2(squid, 160 - 6 + 0 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(doubledots, 160 + 2 + 1 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(three, 160 + 2 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(zero, 160 + 3 * 16, 206, 0x39FF14, m);
		Sleep(150);

		SetPixelsX2(P, 160 + 5 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(O, 160 + 6 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(I, 160 + 7 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(N, 160 + 10 + 7 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(T, 160 + 10 + 8 * 16, 206, 0x39FF14, m);
		Sleep(150);
		SetPixelsX2(S, 162 + 10 + 9 * 16, 206, 0x39FF14, m);
		Sleep(150);


		SetPixelsX2(crab, 160 - 10 + 0 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(doubledots, 160 + 2 + 1 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(two, 160 + 2 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(zero, 160 + 3 * 16, 236, 0x15F4EE, m);
		Sleep(150);

		SetPixelsX2(P, 160 + 5 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(O, 160 + 6 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(I, 160 + 7 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(N, 160 + 10 + 7 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(T, 160 + 10 + 8 * 16, 236, 0x15F4EE, m);
		Sleep(150);
		SetPixelsX2(S, 162 + 10 + 9 * 16, 236, 0x15F4EE, m);
		Sleep(150);


		SetPixelsX2(octopus, 160 - 12 + 0 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(doubledots, 160 + 2 + 1 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(one, 160 + 2 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(zero, 160 + 14 + 2 * 16, 266, 0xDF00FF, m);
		Sleep(150);

		SetPixelsX2(P, 160 + 14 + 4 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(O, 160 + 14 + 5 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(I, 160 + 14 + 6 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(N, 160 + 14 + 10 + 6 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(T, 160 + 14 + 10 + 7 * 16, 266, 0xDF00FF, m);
		Sleep(150);
		SetPixelsX2(S, 162 + 14 + 10 + 8 * 16, 266, 0xDF00FF, m);
		Sleep(150);


		SetPixelsX2(P, 70 + 0 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(R, 70 + 1 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(E, 70 + 2 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(S, 70 + 3 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(S, 70 + 4 * 16, 310, 0xCCCCCC, m);
		Sleep(150);

		SetPixelsX2(F, 70 + 6 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(one, 70 + 7 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(one, 70 + 14 + 7 * 16, 310, 0xCCCCCC, m);
		Sleep(150);

		SetPixelsX2(T, 70 + 28 + 8 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(O, 72 + 28 + 9 * 16, 310, 0xCCCCCC, m);
		Sleep(150);

		SetPixelsX2(F, 72 + 28 + 11 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(U, 72 + 28 + 12 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(L, 72 + 28 + 13 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(L, 72 + 28 + 14 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(S, 72 + 28 + 15 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(C, 72 + 28 + 16 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(R, 72 + 28 + 17 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(E, 72 + 28 + 18 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(E, 72 + 28 + 19 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		SetPixelsX2(N, 72 + 28 + 20 * 16, 310, 0xCCCCCC, m);
		Sleep(150);
		#pragma endregion
		Sleep(2000);
	}
	//When the game is restarted, the top part does not work, only the bottom part works, so the user is not bored with the intro and quickly starts playing again
	Sleep(300);
	gameOver = false;
	WaitForSingleObject(Mutex, INFINITE);// we use mutex to prevent the game from being restarted until the game is over
	m = 0;
	SetPixelsX2(S, 6 + 0 * 14, 6, 0xCCCCCC, m);
	SetPixelsX2(C, 6 + 1 * 14, 6, 0xCCCCCC, m);
	SetPixelsX2(O, 6 + 2 * 14, 6, 0xCCCCCC, m);
	SetPixelsX2(R, 6 + 3 * 14, 6, 0xCCCCCC, m);
	SetPixelsX2(E, 6 + 4 * 14, 6, 0xCCCCCC, m);
	SetPixelsX2(doubledots, 76, 6, 0xCCCCCC, m);

	SetPixelsX2(H, 194 + 0 * 14, 6, 0xFF2603, m);
	SetPixelsX2(I, 194 + 1 * 14, 6, 0xFF2603, m);
	SetPixelsX2(tire, 202 + 1 * 14, 6, 0xFF2603, m);
	SetPixelsX2(S, 196 + 2 * 14, 6, 0xFF2603, m);
	SetPixelsX2(C, 196 + 3 * 14, 6, 0xFF2603, m);
	SetPixelsX2(O, 196 + 4 * 14, 6, 0xFF2603, m);
	SetPixelsX2(R, 196 + 5 * 14, 6, 0xFF2603, m);
	SetPixelsX2(E, 196 + 6 * 14, 6, 0xFF2603, m);
	SetPixelsX2(doubledots, 196 + 7 * 14, 6, 0xFF2603, m);

	SetPixelsX2(spaceship, 416, 8, 0xFBFFFF, m);
	SetPixelsX2(doubledots, 446, 6, 0xFBFFFF, m);
	StartGame(); //Function that starts the threads required for the game to run
	ReleaseMutex(Mutex);// we release the mutex for the game to be restarted
	Sleep(2000);
	starterThreadAvailable = true; // we release the flag when the thread exits, allowing the next restart
	return(NULL);
}

void StartMachine() // Function to start the machine, in short, we call the timer thread that will print the screen once
{
	ICG_DestroyWidget(BTN);  // we destroy the start button so that the machine is not restarted again
	ReadImage("ButtonF.bmp", btn);
	BTN = ICG_BitmapButton(450, 762, 38, 21, ChangeFullScreen); //We use the lighted image of the red button to make full screen with the button
	SetButtonBitmap(BTN, btn);

	HANDLE hTimer = NULL;
	HANDLE hTimerQueue = NULL;
	CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)ScreenLoader, NULL, 0, 30, 0); //ScreenLoader thread is called every 30 ms for printing the screen
	DWORD dw;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StarterThread, NULL, 0, &dw); //We create a thread that will run at the opening of the machine, play animations, clean the screen, or call new threads.

}

void WhenKeyPressed(int key)
{
	if (key == VK_RETURN && !powerOn && starterThreadAvailable) //When the machine is first opened, the timer thread (ScreenLoader) is created
	{
		StartMachine();
		Sleep(20);
	}
	if (key == VK_ESCAPE && fullScreen) //When the machine is in full screen mode, it is closed with the ESC key
	{
		ChangeFullScreen();
	}
	if (key == VK_F11 && !fullScreen && powerOn) //When the machine is in atari mode, it goes to full screen mode with the F11 key
	{
		ChangeFullScreen();
	}
	if (key == VK_RETURN && gameOver && starterThreadAvailable)//Restart the game with the Enter key when the game is over
	{
		starterThreadAvailable = false;
		DWORD dw;
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StarterThread, NULL, 0, &dw);
		Sleep(20);
	}
	keyCode = key; // we take the pressed key out for the ship to use
}


/// <summary>
/// Program entry point
/// </summary>
void ICGUI_main()
{
	//We read the sounds to inside of ICBYTES fluids.
	ReadWave(ufoLowpitch, "ufo_lowpitch.wav");
	ReadWave(ufoHighpitch, "ufo_highpitch.wav");
	ReadWave(fastinvader1, "fastinvader1.wav");
	ReadWave(fastinvader2, "fastinvader2.wav");
	ReadWave(fastinvader3, "fastinvader3.wav");
	ReadWave(fastinvader4, "fastinvader4.wav");
	Mutex = CreateMutex(NULL, FALSE, NULL);
	// We create the screen
	FRMBG = ICG_Frameless(0, 0, 300, 300);
	ReadJPG("SpaceInvadersArcade.jpg", bg);
	DisplayImage(FRMBG, bg);

	BTN = ICG_BitmapButton(243, 930, 320, 150, StartMachine);  //We give the same machine start function to the button with Enter
	ReadImage("ButtonL.bmp", btn);
	SetButtonBitmap(BTN, btn);
	FRMMG = ICG_Frameless(162, 274, 480, 347);
	
	//We write the text that indicates that the start button may not fit on the computer screen and can be started with enter on some computers.
	CreateImage(m, 480, 347, ICB_UINT);
	SetPixelsX2(P, 40 + 0 * 16, 170, 0x303030, m);
	SetPixelsX2(R, 40 + 1 * 16, 170, 0x303030, m);
	SetPixelsX2(E, 40 + 2 * 16, 170, 0x303030, m);
	SetPixelsX2(S, 40 + 3 * 16, 170, 0x303030, m);
	SetPixelsX2(S, 40 + 4 * 16, 170, 0x303030, m);

	SetPixelsX2(E, 40 + 6 * 16, 170, 0x303030, m);
	SetPixelsX2(N, 40 + 7 * 16, 170, 0x303030, m);
	SetPixelsX2(T, 40 + 8 * 16, 170, 0x303030, m);
	SetPixelsX2(E, 42 + 9 * 16, 170, 0x303030, m);
	SetPixelsX2(R, 42 + 10 * 16, 170, 0x303030, m);

	SetPixelsX2(T, 42 + 12 * 16, 170, 0x303030, m);
	SetPixelsX2(O, 44 + 13 * 16, 170, 0x303030, m);

	SetPixelsX2(S, 44 + 15 * 16, 170, 0x303030, m);
	SetPixelsX2(T, 44 + 16 * 16, 170, 0x303030, m);
	SetPixelsX2(A, 46 + 17 * 16, 170, 0x303030, m);
	SetPixelsX2(R, 46 + 18 * 16, 170, 0x303030, m);
	SetPixelsX2(T, 46 + 19 * 16, 170, 0x303030, m);

	SetPixelsX2(A, 48 + 21 * 16, 170, 0x303030, m);
	SetPixelsX2(T, 48 + 22 * 16, 170, 0x303030, m);
	SetPixelsX2(A, 50 + 23 * 16, 170, 0x303030, m);
	SetPixelsX2(R, 50 + 24 * 16, 170, 0x303030, m);
	SetPixelsX2(I, 50 + 25 * 16, 170, 0x303030, m);


	DisplayImage(FRMMG, m);
	std::ifstream inputFile("scores.txt");
	if (inputFile.is_open()) {
		inputFile >> highScore;
		inputFile.close();
	}
	ICG_SetOnKeyPressed(WhenKeyPressed);
}