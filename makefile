salesc: salesbase.c cmdmain.c sqlite3.c
	  gcc `pkg-config --cflags gtk+-3.0` -o SalesC mainwindow.c salesbase.c salesguihelpers.c sqlite3.c `pkg-config --libs gtk+-3.0` -pthread -ldl
