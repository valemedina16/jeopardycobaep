
run: main.o jeopardy_game_class.o jeopardy_question_class.o TextTable.o
	g++ -w main.o jeopardy_game_class.o jeopardy_question_class.o TextTable.o -o run
	@echo "[ BUILD COMPLETE ] - to run type './run'"

main.o: main.cpp
	g++ -c -w main.cpp

jeopardy_game_class.o: jeopardy_game_class.cpp main.h TextTable.h
	g++ -c -w jeopardy_game_class.cpp

jeopardy_question_class.o: jeopardy_question_class.cpp main.h
	g++ -c -w jeopardy_question_class.cpp

TextTable.o: TextTable.cpp TextTable.h
	g++ -c -w TextTable.cpp

clean:
	rm *.o run