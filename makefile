salesc: salesbase.c cmdmain.c sqlite3.c
	  gcc -o salesc salesbase.c cmdmain.c sqlite3.c -pthread -ldl