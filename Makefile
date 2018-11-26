FLAGS= -lncurses -lpthread -lportaudio -std=c++11

all: server client

server: server.cpp oo_model.cpp oo_model.hpp serializable.cpp serializable.hpp
	g++ -oserver server.cpp oo_model.cpp serializable.cpp $(FLAGS)

client: client.cpp oo_model.cpp oo_model.hpp serializable.cpp serializable.hpp
	g++ -oclient client.cpp oo_model.cpp serializable.cpp $(FLAGS)
